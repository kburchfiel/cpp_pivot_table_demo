/* mvvt.cpp: Header files for analyzing data stored
within a map of variants of valarrays
corresponding operations
By Ken Burchfiel
Released under the MIT license 

Much of this code is based on, or directly copied from,
mtt.cpp.

Note: Consider adding an alias for std::get<std::valarray> operations
of various types in order to reduce the time you have to spend
writing that function!
*/

#include "mvvt.h"

// Adding some type aliases in order to cut down on the amount
// of typing needed to do when accessing a valarray of a certain
// type:
// (See https://en.cppreference.com/cpp/language/type_alias)

using sva = std::valarray<std::string>; // sva is short for
// 'string valarray'.
using dva = std::valarray<double>;

using iva = std::valarray<int>;

using lva = std::valarray<long>;

using std::get; // Allows us to type 'get' rather than 'std::get' .
// Inspired by Kevin Anderson's answer at
// https://stackoverflow.com/a/54040080/13097194

// As a result of these additions, we can now replace the following
// code:

// std::get<std::valarray<std::string>>() [38 characters]

// With:

// get<sva>() [10 characters--or around 26% of the original]




std::vector<std::vector<std::string>> csv_to_vvs(std::string csv_path) {
  /* This function converts a CSV file into a vector of vectors
  of strings (or vvs for short). It doesn't treat the first row
  (which is often the header row) any differently from
  subsequent rows.
  This function's code is based in part on csv_to_vmss().
  */

std::vector<std::vector<std::string>> vvs;

  std::ifstream stfs{csv_path};

  for (std::string row; getline(stfs, row);) {
    std::vector<std::string> vs{}; // vs = 'vector of strings'
    std::istringstream current_row;
    current_row.str(row);
    for (std::string column; getline(current_row, column, ',');) {
      vs.push_back(column);
    }
    // Adding this vector (which contains data for a single
    // row) to our vector of vector of strings:
    vvs.push_back(vs);
  }


  return vvs;
}

void print_vvs(std::vector<std::vector<std::string>> &vvs)

{
  /* This function prints out all row/column pairs within a vector
  of vectors of strings in tab-separated format.*/


std::cout << "Length of vvs:" << vvs.size() << "\n";

for (int i = 0; i < vvs.size(); i++) {
  std::vector<std::string> vs = vvs[i];
  for (int j = 0; j < vs.size(); j++) {
    std::cout << vs[j] << "\t";
  }
  std::cout << "\n";
}

}

std::pair<Import_Config, std::map<int, Pivot_Config>>
import_config_data(std::vector<std::vector<std::string>> &vvs) {

  /* This function imports both import- and pivot-table-related data
  from a configuration dataset into both an Import_Config struct
  and a map of int/Pivot_Config pairs. It's assumed that the
  configuration dataset was imported by csv_to_vvs into a vector of
  vectors of strings.
  */

  Import_Config ic;

  // The following map will hold configuration data for one or more
  // pivot tables. (Using integer-based keys lets us ensure that
  // pivot tables are created in the numerical order specified
  // by our key values.)

  std::map<int, Pivot_Config> pcm{}; // pcm is short for
  // 'pivot config map.'

  // (Adding Pivot_Config structs into a map will allow us to more easily
  // create multiple pivot tables from the same data source.)

  for (int i = 1; i < vvs.size(); i++) { // The first row is a
    // header row for users' reference, which we can skip here.

    std::vector<std::string> vs = vvs[i];
    // The first column of each configuration entry
    // will specify to which pivot-table configuration
    // (if any) a particular row applies; the second
    // column will specify a variable for that configuration;
    // and subsequent columns will specify one or more values
    // for that variable.
    // Note that certain variables may have multiple entries
    // within the configuration file. This will allow
    // multiple items (e.g. multiple vector or map values)
    // to get added in for that file without making
    // the CSV file wider than necessary.

    if (vs[0] == "Import") // This row contains import-related data.

    {

      if (vs[1] == "data_file_path") // In this case, vs[0] will be
      // a configuration key, and vs[1] will be its corresponding
      // value. We can ignore any columns that follow vs[1].
      {
        ic.data_file_path = vs[2];
      }

      else if (vs[1] == "field_type_map") {
        ic.field_type_map[vs[2]] = vs[3];
      }

      else if (vs[1] == "delimiter") {

        if (vs[2] == "comma") {
          ic.delimiter = ',';
        }

        else if (vs[2] == "tab") {
          ic.delimiter = '\t';
        }

        else { // In this case, the first character within vs[2]
          // will be used as the delimiter.

          ic.delimiter = vs[2][0];
        }
      }

      else if (vs[1] == "fields_to_import") {
        ic.fields_to_import.insert(vs[2]);
      }

      else if (vs[1] == "csv_vals_to_include") {
        ic.csv_vals_to_include[vs[2]].insert(vs[3]);
      }

      else if (vs[1] == "csv_vals_to_exclude") {
        ic.csv_vals_to_exclude[vs[2]].insert(vs[3]);
      }

    }

    else // This row contains data for a particular pivot table.
    {
      int pivot_index = std::stoi(vs[0]);
      if (pcm.contains(pivot_index) ==
          false) { // Adding an entry for this particular pivot index:

        Pivot_Config pc{};
        pcm[pivot_index] = pc;
      }

      // Updating a particular pivot table's configuration info
      // with the data stored in this row:
      if (vs[1] == "calculate_means") {
        if (vs[2] == "Y") {
          pcm[pivot_index].calculate_means = true;
        } else {
          pcm[pivot_index].calculate_means = false;
        }
      }

      else if (vs[1] == "index_fields") {
        pcm[pivot_index].index_fields.push_back(vs[2]);
      }

      else if (vs[1] == "double_value_fields") {
        pcm[pivot_index].double_value_fields.push_back(vs[2]);
      }

      else if (vs[1] == "long_value_fields") {
        pcm[pivot_index].long_value_fields.push_back(vs[2]);
      }

      else if (vs[1] == "int_value_fields") {
        pcm[pivot_index].int_value_fields.push_back(vs[2]);
      }

      else if (vs[1] == "string_vals_to_include") {
        pcm[pivot_index].string_vals_to_include[vs[2]].insert(vs[3]);
      }

      else if (vs[1] == "string_vals_to_exclude") {
        pcm[pivot_index].string_vals_to_exclude[vs[2]].insert(vs[3]);
      }

      else if (vs[1] == "int_vals_to_include") {
        pcm[pivot_index].int_vals_to_include[vs[2]].insert(stoi(vs[3]));
      }

      else if (vs[1] == "int_vals_to_exclude") {
        pcm[pivot_index].int_vals_to_exclude[vs[2]].insert(stoi(vs[3]));
      }

      else if (vs[1] == "double_vals_to_include") {
        pcm[pivot_index].double_vals_to_include[vs[2]].insert(stod(vs[3]));
      }

      else if (vs[1] == "double_vals_to_exclude") {
        pcm[pivot_index].double_vals_to_exclude[vs[2]].insert(stod(vs[3]));
      }

      else if (vs[1] == "long_vals_to_include") {
        pcm[pivot_index].long_vals_to_include[vs[2]].insert(stol(vs[3]));
      }

      else if (vs[1] == "long_vals_to_exclude") {
        pcm[pivot_index].long_vals_to_exclude[vs[2]].insert(stol(vs[3]));
      }

      else if (vs[1] == "table_file_path") // In this case, vs[0] will be
      // a configuration key, and vs[1] will be its corresponding
      // value. We can ignore any columns that follow vs[1].
      {
        pcm[pivot_index].table_file_path = vs[2];
      }
    }
  }


  std::cout << "Import configuration info:\n";
  std::cout << "Data file path:\t" << ic.data_file_path << "\n";
  std::cout << "delimiter:\t" << ic.delimiter << "\n";
  std::cout << "field_type_map:\t";

  for (auto &[field_name, field_type] : ic.field_type_map) {
    std::cout << field_name << ": " << field_type << "\t";
  }
  std::cout << "\n";

  std::cout << "fields_to_import:\t";
  for (std::string field : ic.fields_to_import) {
    std::cout << field << "\t";
  }
  std::cout << "\n";



  std::pair<Import_Config, std::map<int, 
  Pivot_Config>> ic_pcm {ic, pcm};

  return ic_pcm;
}


void print_pivot_config_data(Pivot_Config& pc)
{

  // This function prints out selected configuration information
  // from a Pivot_Config object.
    std::cout << "calculate_means:\t" << pc.calculate_means << "\n";

    // Range for loop:
    std::cout << "index_fields:\t";
    for (std::string field : pc.index_fields) {
      std::cout << field << "\t";
    }

    // Older alternative to range for loops:
    std::cout << "\n";
    std::cout << "double_value_fields:\t";
    for (int i = 0; i < pc.double_value_fields.size(); i++) {
      std::cout << pc.double_value_fields[i] << "\t";
    }
    std::cout << "\n";

    // You also don't need to specify the value type within
    // range for loops:
    std::cout << "long_value_fields:\t";
    for (auto field : pc.long_value_fields) {
      std::cout << field << "\t";
    }
    std::cout << "\n";
    std::cout << "int_value_fields:\t";

    for (auto field : pc.int_value_fields) {
      std::cout << field << "\t";
    }
    std::cout << "\n";

    std::cout << "Table file path:\t" << pc.table_file_path << "\n";
}



MVV_Table csv_to_mvvt(std::string file_path,
           std::map<std::string, std::string> field_type_map,
           std::set<std::string> fields_to_import, char delimiter,
           std::map<std::string, std::set<std::string>> csv_vals_to_include,
           std::map<std::string, std::set<std::string>> vals_to_exclude) {
  /* This function imports CSV-file data into an MVV_Table object.

  // field_type_map stores field names as keys and your desired
  // field types as values. Any field not present within this
  // dictionary is assumed to be a string.)
  // The following field types are recognized by the function:
  // 'string'; 'double'; 'int'; and 'long'.

  fields_to_import: The fields from the CSV file that you would like
  to import. If this vector is left blank, all fields will get imported.


  // delimiter: The character used to separate fields within the
  // original CSV file. The default ',' delimiter will often work fine
  *unless* there are commas within the cell values themselves (in which case
  you'll need to use a more capable library, such as
  Vincent La's CSV parser--available at
  https://github.com/vincentlaucsb/csv-parser .)
  Meanwhile, for .tsv (tab-separated) files, a delimiter of '\t' should work
  well (as long, again, as there are no tabs within your cell values.)

  // csv_vals_to_include: This map stores field names as keys and sets
  of field values as values. Only the rows within the CSV file
  whose field values, for a given variable, are present within
  that variable's set of field values will get included.

  // vals_to_exclude: Similar to csv_vals_to_include, except that
  only those rows whose values are *not* present within a given
  field's set of values will get included.

  */

  /* My function-timing code came from
  https://github.com/kburchfiel/cpp_pivot_tables/blob/main/
  pivot_compressors.cpp,
  an earlier pivot-table script I had written. */

  auto function_start_time = std::chrono::high_resolution_clock::now();

  MVV_Table mvvt{}; 

  int row_count = 0;

  // Determining the length of the CSV file (which will also determine
  // how long our valarrays should be):
  { // Blocking off this code with brackets so that the stfs file
    // stream goes out of scope afterwards)
    std::ifstream stfs{file_path};
    // if(!stfs) throw("Failed to open input file.");

    for (std::string row; getline(stfs, row);) {
      row_count++;
    }

    std::cout << "CSV-file row count: " << row_count << "\n";

    mvvt.length = row_count - 1; // The header row won't get
    // included within the Multi_Type_Table's valarrays, so we should
    // decrement mvvt.length by 1 accordingly.
  }

  // Reopening the file so that we can get back to its beginning:
  // (There could very well be a more efficient way to take this step.)
  std::ifstream stfs{file_path};
  // if(!stfs) throw("Failed to open input file.");

  std::map<int, std::string> selected_col_indices; // This map will
  // make it easier to look up the field associated with each
  // column index within the CSV file.

  std::map<int, std::string> all_col_indices; // This map will contain
  // all indices of all headers, rather than only the ones we wish
  // to import.

  // The following vector of maps will store all rows of our .csv file
  // (aside from the header row).

  // In order to add values into our vector of maps, we'll first
  // use getline() to access each row of the .csv file; next,
  // we'll use getline() to access each column within each row.

  int row_index = -1; // Setting this value to -1 allows
  // the first actual value row (i.e. the first row below the header
  // row) to have a row_index value of 0--which will come in handy
  // when using this value as a valarray index.)
  // (Note: Now that I'm using a separate variable (valarray_row_index)
  // for valarray indices, the code could probably be updated
  // to have row_index start at 0.)

  int valarray_row_index = 0; // If not all rows are getting added,
  // we'll need to keep track of both our overall row index
  // (for the CSV file) and the number of rows we've added
  // (for use as an index within our valarrays).

  for (std::string row; getline(stfs, row);) {
    // std::cout << "Row:" << row << "\n";
    if (row_index == -1) // We'll want to treat the first row differently,
    // as we're assuming that it includes column names rather than
    // values.
    {
      // std::cout << "First row.\n";
      std::istringstream first_row;
      first_row.str(row); // Based on:
      // https://en.cppreference.com/w/cpp/string/basic_string/getline

      // Using getline() with a delimiter argument in order
      // to go through each line within our istringstream for
      // the first row and then extract all field names from it:
      // (See https://en.cppreference.com/w/cpp/string/basic_string/getline
      // for more information.)

      // Note that, in this case, getline() is being used to split
      // data within a single line (e.g. the current row) into
      // a series of delimiter-separated values. This saves us the trouble
      // of manually figuring out where one field ends and another begins
      // (as fields may contain whitespace-separated words).
      // This approach is based on the example code found at
      // https://en.cppreference.com/w/cpp/string/basic_string/getline.

      int col_index = 0;
      for (std::string field_name; getline(first_row, field_name, delimiter);) {

        all_col_indices[col_index] = field_name; // All fields
        // will get added into all_col_indices.

        // Adding this column name, together with its index,
        // to selected_col_indices (if instructed to do so by the caller):
        if ((fields_to_import.empty()) ||
            (fields_to_import.contains(field_name)))

        {
          selected_col_indices[col_index] = field_name;
          // Adding this column name to mvvt's field-order vector:
          mvvt.field_order.push_back(field_name);

          // Determining, based on the requested type for this
          // field, the valarray map to which it should be added--then
          // initializing a valarray that can hold its values:

          // If no field has been specified, it will simply
          // get added as a string.

          if (field_type_map.contains(field_name) == false) {
            // Adding this field to its type's set 
            // within our type map:
            mvvt.tm["string"].insert(field_name);
            std::valarray<std::string> va(mvvt.length);
            // Since 'va' is only present within this scope,
            // we can reuse it as a name for other new
            // valarrays.

            
            // When adding a new valarray to mvv, it's not necessary
            // to specify the type (though we *will* have to do
            // so when retrieving an existing valarray). See
            // https://en.cppreference.com/cpp/utility/variant
            mvvt.mvv[field_name] = va;
          }

          else if (field_type_map[field_name] == "string") {
            mvvt.tm["string"].insert(field_name);
            sva va(mvvt.length);

            mvvt.mvv[field_name] = va;
          }

          else if (field_type_map[field_name] == "double") {
            mvvt.tm["double"].insert(field_name);
            dva va(mvvt.length);

            mvvt.mvv[field_name] = va;
          }

          else if (field_type_map[field_name] == "int") {
            mvvt.tm["int"].insert(field_name);
            iva va(mvvt.length);

            mvvt.mvv[field_name] = va;
          }

          else if (field_type_map[field_name] == "long") {
            lva va(mvvt.length);
            mvvt.tm["long"].insert(field_name);
            mvvt.mvv[field_name] = va;
          }

          // You might want to raise an error here if none of the
          // previous conditions was met.
        }
        col_index++; // It's crucial to keep this outside the previous
        // if statement; otherwise, columns may get matched
        // to the wrong index within selected_col_indices.
      }

    }

    // Adding values into the valarrays created within the previous
    // if statement:

    else {
      // std::cout << "Value row\n";
      // Initializing the current row as a new istringstream:
      std::istringstream current_row;
      current_row.str(row);

      bool include_row = true;

      int col_index = 0;
      std::map<std::string, std::string> row_map;
      for (std::string field_val; getline(current_row, field_val, delimiter);)
      // We'll import all fields within this row (including those that
      // won't get added into our valarray, as they might still show up
      // in our include/exclude criteria) into a map of field/value
      // pairs. We can then iterate through our vals_to_exclude and
      // csv_vals_to_include maps to determine whether a particular row
      // should get included or excluded.)
      {
        row_map[all_col_indices[col_index]] = field_val;
        col_index++;
      }

      // Determining whether this particular row should get included:

      if ((vals_to_exclude.size() > 0) || (csv_vals_to_include.size() > 0)) {

        // We'll now iterate through each set of variables and variable
        // values within string_vals_to_include. If we find that a row's
        // value for a particular variable is not listed within the
        // set of values to include, our include_row flag will be set
        // to false.

        for (auto &[variable, vals] : csv_vals_to_include) {
          // Since all headers within the CSV file are present
          // within row_map, we won't need to first check whether
          // row_map contains the current variable within our loop.
          if (vals.contains(row_map[variable]) == false) {
            include_row = false;
            break;
          }
        }

        // There's no need to run the following include/exclude checks
        // if include_row is already false. Instead, we should
        // move to the next row of the CSV file within the for loop.
        if (include_row == false) {
          continue;
        }

        // Using a similar setup for vals_to_exclude:

        for (auto &[variable, vals] : vals_to_exclude) {
          // Since all headers within the CSV file are present
          // within row_map, we won't need to first check whether
          // row_map contains the current variable within our loop.
          if (vals.contains(row_map[variable]) == true) {
            include_row = false;
            break;
          }
        }

        if (include_row == false) {
          continue;
        }

        // Due to the continue statements added above, it shouldn't be
        // necessary to check whether include_row is true before
        // running the following code.
      }

      // We'll now import all values within this row whose
      // corresponding headers are located into mvvt.field_order
      // into our Multi_Type_Table.
      for (std::string field_name : mvvt.field_order) {
        // Adding this value to the column's dedicated
        // valarray (and
        // converting it to the proper type beforehand if necessary):

        // Any items that don't have a specified type are assumed
        // to be strings.
        if (field_type_map.contains(field_name) == false) {

          get<sva>(
        mvvt.mvv[field_name])[valarray_row_index] = row_map[field_name];
        }

        else if (field_type_map[field_name] == "string") {
          get<sva>(
        mvvt.mvv[field_name])[valarray_row_index] = row_map[field_name];
        }

        else if (field_type_map[field_name] == "double") {
          get<dva>(
        mvvt.mvv[field_name])[valarray_row_index] =
              std::stod(row_map[field_name]);
        }

        else if (field_type_map[field_name] == "int") {
        get<dva>(mvvt.mvv[field_name])[
        valarray_row_index] = std::stoi(row_map[field_name]);
        }

        else if (field_type_map[field_name] == "long") {
        get<lva>(
          mvvt.mvv[field_name])[valarray_row_index] =
              std::stol(row_map[field_name]);
        };
      }

      valarray_row_index++; // This variable should only be incremented
      // when adding a given row into our valarrays.
    }
    row_index++; // This variable should be incremented for all CSV rows.
  }

  std::cout << "Number of rows within Multi_Type_Table: " << valarray_row_index
            << "\n";
  std::cout << "Number of CSV rows processed: " << row_index << "\n";

  // If we requested any filter operations, it's likely that the
  // valarrays have fewer actual entries than their size makes room
  // for. Therefore, we'll go ahead and resize these valarrays
  // in order to exclude any blank entries.

  // Note: Because we incremented valarray_row_index by 1, it will
  // currently represent the total number of valid elements rather
  // than the actual final index. Therefore, we can use it to specify
  // the ideal size of each valarray in light of our filter
  // operations.

  // (See https://en.cppreference.com/cpp/numeric/valarray/slice
  // for more details on std::slice.)

  if ((vals_to_exclude.size() > 0) || (csv_vals_to_include.size() > 0)) {

    mvvt.length = valarray_row_index;

    // Resizing all string-typed valarrays:

    for (std::string field_name: mvvt.tm["string"])

    {
        get<sva>(mvvt.mvv[field_name]) = 
    get<sva>(
    mvvt.mvv[field_name])[std::slice(0, mvvt.length, 1)];
    }

    // Performing the same action for valarrays of other types:

    for (std::string field_name: mvvt.tm["long"])

    {
        get<lva>(mvvt.mvv[field_name]) = 
    get<lva>(
    mvvt.mvv[field_name])[std::slice(0, mvvt.length, 1)];
    }

    for (std::string field_name: mvvt.tm["double"])

    {
        get<dva>(mvvt.mvv[field_name]) = 
    get<dva>(
    mvvt.mvv[field_name])[std::slice(0, mvvt.length, 1)];
    }

    for (std::string field_name: mvvt.tm["int"])

    {
        get<iva>(mvvt.mvv[field_name]) = 
    get<iva>(
    mvvt.mvv[field_name])[std::slice(0, mvvt.length, 1)];
    }

  }

  auto function_end_time = std::chrono::high_resolution_clock::now();
  auto function_run_time =
      std::chrono::duration<double>(function_end_time - function_start_time)
          .count();
  std::cout << "Finished running csv_to_mvvt with a " << row_count
            << "-row dataset in " << function_run_time << " seconds.\n";

  return mvvt;
}


void print_mvvt(MVV_Table mvvt, long n, char print_separator) {
  // This function prints the headers and data stored within a
  // given multi_type table.

  // n specifies how many rows of data to print. If set to 0,
  // all rows will be printed.

  // print_separator specifies which character to use to separate
  // printed values. '\t', the default, should work well for most
  // use cases.

  // NOTE: When I pass a Multi_Type_Table reference to this function
  // rather than a copy, nothing gets printed and my demo code
  // ends with a segmentation fault. I'll need to need to figure
  // out why this is the case.

  // Printing headers:

  for (int i = 0; i < mvvt.field_order.size(); i++) {
    std::cout << mvvt.field_order[i] << print_separator;
  }
  std::cout << "\n";

  // Printing values:

  int rows_to_print = 0;

  if (n == 0) // In this case, all values will be printed.
  {
    rows_to_print = mvvt.length;
  }

  else // Making sure we don't attempt to print more rows than
  // the Multi_Type_Table contains

  {
    rows_to_print = std::min(long(mvvt.length), n);
  }

  for (int i = 0; i < rows_to_print; i++)

  {

    for (int j = 0; j < mvvt.field_order.size(); j++) {
      // Determining which type-specific valarray this field is in,
      // then retrieving it from that valarray:

      // We're iterating through mvvt.field_order in order to print
      // out fields in the order specified within that vector.
      // However, this means that, before we can print out a value,
      // we'll need to check which type its valarray uses.
      // (Alternatively, we could iterate through each key 
      // within mvvt.tm in order to print out these values,
      // which would let us know the type of each value beforehand.)

      if (mvvt.tm["string"].contains(mvvt.field_order[j]))
{
      std::cout << get<sva>(
      mvvt.mvv[mvvt.field_order[j]])[i] << print_separator;
      }


      if (mvvt.tm["double"].contains(mvvt.field_order[j]))
{
      std::cout << get<dva>(
      mvvt.mvv[mvvt.field_order[j]])[i] << print_separator;
      }


      if (mvvt.tm["long"].contains(mvvt.field_order[j]))
{
      std::cout << get<lva>(
      mvvt.mvv[mvvt.field_order[j]])[i] << print_separator;
      }


      if (mvvt.tm["int"].contains(mvvt.field_order[j]))
{
      std::cout << get<iva>(mvvt.mvv[mvvt.field_order[j]])[i] << print_separator;
      }


    }
    std::cout << "\n";
  }
}
