/* mtt.cpp
By Ken Burchfiel
Released under the MIT license */

#include "mtt.h"

Multi_Type_Table csv_to_mtt(std::string file_path,
                            std::map<std::string, std::string> field_type_map,
                            std::set<std::string> fields_to_import,
                            char delimiter) {
  /* This function imports CSV-file data into an MTT object.

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


  */

  /* My function-timing code came from
  https://github.com/kburchfiel/cpp_pivot_tables/blob/main/
  pivot_compressors.cpp,
  an earlier pivot-table script I had written. */

  auto function_start_time = std::chrono::high_resolution_clock::now();

  Multi_Type_Table
      mtt{}; // mtto = 'MTT object. I probably could have just gone with \
  'mtt' also.

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

    mtt.length = row_count - 1; // The header row won't get
    // included within the Multi_Type_Table's valarrays, so we should
    // decrement mtt.length by 1 accordingly.
  }

  // Reopening the file so that we can get back to its beginning:
  // (There could very well be a more efficient way to do so.)
  std::ifstream stfs{file_path};
  // if(!stfs) throw("Failed to open input file.");

  std::map<int, std::string> col_indices; // This map will
  // make it easier to look up the field associated with each
  // column index within the CSV file.

  // The following vector of maps will store all rows of our .csv file
  // (aside from the header row).

  // In order to add values into our vector of maps, we'll first
  // use getline() to access each row of the .csv file; next,
  // we'll use getline() to access each column within each row.

  int row_index = -1; // Setting this value to -1 allows
  // the first actual value row (i.e. the first row below the header
  // row) to have a row_index value of 0--which will come in handy
  // when using this value as a valarray index.)

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
        // Adding this column name, together with its index,
        // to col_indices (if instructed to do so by the caller):
        if ((fields_to_import.empty()) ||
            (fields_to_import.contains(field_name)))

        {
          col_indices[col_index] = field_name;
          // Adding this column name to mtt's field-order vector:
          mtt.field_order.push_back(field_name);

          // Determining, based on the requested type for this
          // field, the valarray map to which it should be added--then
          // initializing a valarray that can hold its values:

          // If no field has been specified, it will simply
          // get added as a string.

          if (field_type_map.contains(field_name) == false) {
            std::valarray<std::string> va(mtt.length);
            // Since 'va' is only present within this scope,
            // we can reuse it as a name for other new
            // valarrays.

            mtt.stf[field_name] = va;
          }

          else if (field_type_map[field_name] == "string") {
            std::valarray<std::string> va(mtt.length);

            mtt.stf[field_name] = va;
          }

          else if (field_type_map[field_name] == "double") {
            std::valarray<double> va(mtt.length);

            mtt.df[field_name] = va;
          }

          else if (field_type_map[field_name] == "int") {
            std::valarray<int> va(mtt.length);


            mtt.intf[field_name] = va;
          }

          else if (field_type_map[field_name] == "long") {
            std::valarray<long> va(mtt.length);


            mtt.lf[field_name] = va;
          }

          // You might want to raise an error here if none of the
          // previous conditions was met.
        }
        col_index++; // It's crucial to keep this outside the previous
        // if statement; otherwise, columns may get matched
        // to the wrong index within col_indices.
      }

    }

    // Adding values into the valarrays created within the previous
    // if statement:

    else {
      // std::cout << "Value row\n";
      // Initializing the current row as a new istringstream:
      std::istringstream current_row;
      current_row.str(row);

      int col_index = 0;

      for (std::string field_val; getline(current_row, field_val, delimiter);) {
        // Checking whether the current index is part of our list
        // of indices to include:
        // std::cout << "Column:" << field_val << "\n";
        // std::cout << "col_index:" << col_index << "\n";

        // std::cout << "Row index: " << row_index << " col_index: " <<
        // col_index
        //           << " field_val: " << field_val << "\n";
        if (col_indices.contains(
                col_index)) { // Adding this value to the column's dedicated
                              // valarray (and
          // converting it to the proper type beforehand if necessary):

          std::string field_name = col_indices[col_index];

          // Any items that don't have a specified type are assumed
          // to be strings.
          if (field_type_map.contains(field_name) == false) {

            mtt.stf[field_name][row_index] = field_val;
          }

          else if (field_type_map[field_name] == "string") {
            mtt.stf[field_name][row_index] = field_val;
          }

          else if (field_type_map[field_name] == "double") {
            mtt.df[field_name][row_index] = std::stod(field_val);
          }

          else if (field_type_map[field_name] == "int") {
            mtt.intf[field_name][row_index] = std::stoi(field_val);
          }

          else if (field_type_map[field_name] == "long") {
            mtt.lf[field_name][row_index] = std::stol(field_val);
          };
        }

        col_index++;
      }
    }
    row_index++;
  }

  auto function_end_time = std::chrono::high_resolution_clock::now();
  auto function_run_time =
      std::chrono::duration<double>(function_end_time - function_start_time)
          .count();
  std::cout << "Finished running csv_to_mtt with a " << row_count
            << "-row dataset in " << function_run_time << " seconds.\n";

  return mtt;
}

Multi_Type_Table mtt_pivot_table(
    Multi_Type_Table &mtt, std::vector<std::string> const index_fields,
    std::vector<std::string> const double_value_fields,
    std::vector<std::string> const long_value_fields,
    std::vector<std::string> const int_value_fields, char pivot_index_separator,
    bool calculate_means,
    std::map<std::string, std::set<std::string>> string_vals_to_include,
    std::map<std::string, std::set<std::string>> string_vals_to_exclude,

    std::map<std::string, std::set<int>> int_vals_to_include,
    std::map<std::string, std::set<int>> int_vals_to_exclude,

    std::map<std::string, std::set<double>> double_vals_to_include,
    std::map<std::string, std::set<double>> double_vals_to_exclude,

    std::map<std::string, std::set<long>> long_vals_to_include,
    std::map<std::string, std::set<long>> long_vals_to_exclude)

{

  /* This function creates a pivot table using data from a
    Multi_Type_Table. (More documentation to come)
    */

  auto function_start_time = std::chrono::high_resolution_clock::now();

  std::map<std::string, Pivot_Row> pivot_table;
  for (int i = 0; i < mtt.length; i++)

  {
    // Determining, based on our inclusion/exclusion variables,
    // whether this row's data should get considered for the table:
    // (Note: This code could probably be dramatically simplified
    // by using a template-based approach--which probably holds true
    // for much of the other code within this file as well!)

    bool include_row = true;

    if (string_vals_to_include.size() > 0)

    {
      // We'll now iterate through each set of variables and variable
      // values within string_vals_to_include. If we find that a row's
      // value for a particular variable is not listed within the
      // set of values to include, our include_row flag will be set
      // to false.

      for (auto &[variable, vals] : string_vals_to_include) {
        if (vals.contains(mtt.stf[variable][i]) == false) {
          include_row = false;
          break;
        }
      }
    }

    // There's no need to run the following include/exclude checks
    // if include_row is already false. Instead, we should
    // move to the next row within the for loop.
    if (include_row == false) {
      continue;
    }

    // The following set of code will be similar, except we'll set
    // include_row to false if a given variable's value *is* present
    // within our list of variables to exclude.

    if (string_vals_to_exclude.size() > 0)

    {
      for (auto &[variable, vals] : string_vals_to_exclude) {
        if (vals.contains(mtt.stf[variable][i]) == true) {
          include_row = false;
          break;
        }
      }
    }

    if (include_row == false) {
      continue;
    }

    // The following checks are similar to the previous two, except
    // that they check for double-, int-, and long-typed variables.

    // Double-variable checks:

    if (double_vals_to_include.size() > 0)

    {

      for (auto &[variable, vals] : double_vals_to_include) {
        if (vals.contains(mtt.df[variable][i]) == false) {
          include_row = false;
          break;
        }
      }
    }

    if (include_row == false) {
      continue;
    }

    if (double_vals_to_exclude.size() > 0)

    {
      for (auto &[variable, vals] : double_vals_to_exclude) {
        if (vals.contains(mtt.df[variable][i]) == true) {
          include_row = false;
          break;
        }
      }
    }

    if (include_row == false) {
      continue;
    }

    // Long-variable checks:

    if (long_vals_to_include.size() > 0)

    {
      // We'll now iterate through each set of variables and variable
      // values within long_vals_to_include. If we find that a row's
      // value for a particular variable is not listed within the
      // set of values to include, our include_row flag will be set
      // to false.

      for (auto &[variable, vals] : long_vals_to_include) {
        if (vals.contains(mtt.lf[variable][i]) == false) {
          include_row = false;
          break;
        }
      }
    }

    // There's no need to run the following include/exclude checks
    // if include_row is already false. Instead, we should
    // move to the next row within the for loop.
    if (include_row == false) {
      continue;
    }

    // The following set of code will be similar, except we'll set
    // include_row to false if a given variable's value *is* present
    // within our list of variables to exclude.

    if (long_vals_to_exclude.size() > 0)

    {
      for (auto &[variable, vals] : long_vals_to_exclude) {
        if (vals.contains(mtt.lf[variable][i]) == true) {
          include_row = false;
          break;
        }
      }
    }

    if (include_row == false) {
      continue;
    }

    // Integer-variable checks:

    if (int_vals_to_include.size() > 0)

    {
      // We'll now iterate through each set of variables and variable
      // values within int_vals_to_include. If we find that a row's
      // value for a particular variable is not listed within the
      // set of values to include, our include_row flag will be set
      // to false.

      for (auto &[variable, vals] : int_vals_to_include) {
        if (vals.contains(mtt.intf[variable][i]) == false) {
          include_row = false;
          break;
        }
      }
    }

    // There's no need to run the following include/exclude checks
    // if include_row is already false. Instead, we should
    // move to the next row within the for loop.
    if (include_row == false) {
      continue;
    }

    if (int_vals_to_exclude.size() > 0)

    {
      for (auto &[variable, vals] : int_vals_to_exclude) {
        if (vals.contains(mtt.intf[variable][i]) == true) {
          include_row = false;
          break;
        }
      }
    }

    if (include_row == false) {
      continue;
    }

    // Due to the continue statements added above, it shouldn't be
    // necessary to check whether include_row is true before
    // running the following code.

    // std::cout << "Checkpoint B.";
    // Combining index fields into a single string that can serve
    // as a dictionary key:
    std::string pivot_index = "";
    for (int j = 0; j < index_fields.size(); j++) {
      // Retrieving the index values for this particular
      // row:
      // std::cout << "mtt.stf[index_fields[j]][i]: "
      //          << mtt.stf[index_fields[j]][i] << "\n";
      pivot_index += mtt.stf[index_fields[j]][i];
      // We should add a separator after this field as long as it's
      // not the final entry.
      if (j != index_fields.size() - 1) {
        pivot_index += pivot_index_separator;
      }
    }

    // std::cout << "Checkpoint C";

    // std::cout << "current pivot index: " << pivot_index << "\n";

    if (pivot_table.contains(pivot_index) == false) {
      // std::cout << "Creating a new pivot_index." << "\n";
      // Creating a Pivot_Row that can serve as
      // the value for this key:

      Pivot_Row pr{};

      // Pivot rows store maps of doubles and of
      // longs under the names df and lf, respectively.
      // df will be used to store sums,
      // whereas lf will be used to store counts.
      // (Means will get calculated later on via
      // a separate process, so they won't get stored
      // directly within these tables.)

      // Thus, each entry within long_value_fields,
      // double_value fields, and int_value_fields
      // will get a sum entry (within
      // df) and a count entry (within lf)Each of these entries
      // will have a field name as its key, thus making
      // them easier to keep track of.

      // Initializing the dictionary's values:

      for (int j = 0; j < double_value_fields.size(); j++) {
        // Note that sums will get added to the df (double-field)
        // attribute of pr, whereas counts will get added
        // to its lf attribute. In order to help simplify this function,
        // this will be the case for all metric fields,
        // whether integer-, double-, or long-based. (It may be
        // ideal, however, to update this code in the future
        // so that integer and long sums get stored within
        // lf rather than df.)

        // std::cout << "Double-value field: " << double_value_fields[j] <<
        // "\n";

        pr.df[double_value_fields[j] + "_sum"] = 0.0;
        pr.lf[double_value_fields[j] + "_count"] = 0;

        // std::cout << "Double-field values: " << pivot_table[pivot_index].df[
        // double_value_fields[j] + "_sum"] << " and " <<
        // pivot_table[pivot_index].lf[
        // double_value_fields[j] + "_count"] << "\n";
      }

      for (int j = 0; j < int_value_fields.size(); j++) {
        pr.df[int_value_fields[j] + "_sum"] = 0.0;
        pr.lf[int_value_fields[j] + "_count"] = 0;
      }

      for (int j = 0; j < long_value_fields.size(); j++) {
        pr.df[long_value_fields[j] + "_sum"] = 0.0;
        pr.lf[long_value_fields[j] + "_count"] = 0;
      }

      //   std::cout << "Adding pivot_index to pivot_table.\n";

      pivot_table[pivot_index] = pr;

    }

    else {
      //   std::cout << "Value already present within dictionary. " << "\n";
      // In this case, we'll add new values to the existing
      // dictionary.
    }

    // Even if the previous if statement returned true,
    // we'll still want to run the following block of code
    // in order to add that row's values into the pivot_row
    // that we just created.

    for (int j = 0; j < double_value_fields.size(); j++) {
      // std::cout << "Double-value field: " << double_value_fields[j] << "\n";
      // std::cout << "mtt.df[double_value_fields[j]][i]: "
      //           << mtt.df[double_value_fields[j]][i] << "\n";
      pivot_table[pivot_index].df[double_value_fields[j] + "_sum"] +=
          mtt.df[double_value_fields[j]][i];
      // std::cout << "Now updating count field:" << "\n";
      pivot_table[pivot_index].lf[double_value_fields[j] + "_count"]++;
    }

    //   std::cout << "Updating integer fields." << "\n";

    for (int j = 0; j < int_value_fields.size(); j++) {
      // std::cout << "mtt.intf[int_value_fields[j]][i]: "
      //           << mtt.intf[int_value_fields[j]][i] << "\n";
      pivot_table[pivot_index].df[int_value_fields[j] + "_sum"] +=
          double(mtt.intf[int_value_fields[j]][i]);
      pivot_table[pivot_index].lf[int_value_fields[j] + "_count"]++;
    }

    //   std::cout << "Updating long fields." << "\n";

    for (int j = 0; j < long_value_fields.size(); j++) {

      // std::cout << "mtt.lf[long_value_fields[j]][i]: "
      //           << mtt.lf[long_value_fields[j]][i] << "\n";

      pivot_table[pivot_index].df[long_value_fields[j] + "_sum"] +=
          double(mtt.lf[long_value_fields[j]][i]);
      pivot_table[pivot_index].lf[long_value_fields[j] + "_count"]++;
    }
  }

  //   std::cout << "Map size: " << pivot_table.size() << "\n";

  // Converting pivot_table to a Multi_Type_Table:

  Multi_Type_Table pivot_mtt{};
  pivot_mtt.length = pivot_table.size();

  std::vector<std::string> field_order{}; // This vector will
  // store field orders for the Multi_Type_Table that this
  // function will use to return the pivot-table results.

  // We'll now add valarray entries that can store our index fields
  // and corresponding pivot values. We'll also add field names
  // to field_order as we go along.

  // Determining the name of the index-field key:
  std::string index_field_name = "";
  for (int j = 0; j < index_fields.size(); j++) {
    // Joining the index-field names together:
    index_field_name += index_fields[j];
    // We should add a separator after this field as long as it's
    // not the final entry.
    if (j != index_fields.size() - 1) {
      index_field_name += pivot_index_separator;
    }
  }

  // Adding a string-typed valarray for storing index-field values:
  std::valarray<std::string> va(pivot_mtt.length);
  pivot_mtt.stf[index_field_name] = va;
  field_order.push_back(index_field_name);

  // Adding, for each original value field, double- and long-typed
  // valarrays that will store pivot values:

  std::vector<std::string> combined_value_fields;

  for (std::string s : double_value_fields) {
    combined_value_fields.push_back(s);
  }
  for (std::string s : long_value_fields) {
    combined_value_fields.push_back(s);
  }
  for (std::string s : int_value_fields) {
    combined_value_fields.push_back(s);
  }

  for (int j = 0; j < combined_value_fields.size(); j++) {
    std::valarray<double> vad(pivot_mtt.length);
    pivot_mtt.df[combined_value_fields[j] + "_sum"] = vad;
    field_order.push_back(combined_value_fields[j] + "_sum");

    std::valarray<long> val(pivot_mtt.length);
    pivot_mtt.lf[combined_value_fields[j] + "_count"] = val;
    field_order.push_back(combined_value_fields[j] + "_count");
  }

  pivot_mtt.field_order = field_order;

  // Adding all index and value data into these new valarrays:

  // std::cout << "Checkpoint F.";

  int valarray_index = 0;
  for (auto &[index_field, pivot_row] : pivot_table) {
    // Updating the index valarray:
    pivot_mtt.stf[index_field_name][valarray_index] = index_field;

    // Updating the double and long valarrays that hold sum and count
    // data, respectively, for each value variable:

    // std::cout << "Checkpoint G.";

    for (int j = 0; j < combined_value_fields.size(); j++) {
      pivot_mtt.df[combined_value_fields[j] + "_sum"][valarray_index] =
          pivot_row.df[combined_value_fields[j] + "_sum"];

      pivot_mtt.lf[combined_value_fields[j] + "_count"][valarray_index] =
          pivot_row.lf[combined_value_fields[j] + "_count"];
    }
    valarray_index++;
  }

  auto function_end_time = std::chrono::high_resolution_clock::now();
  auto function_run_time =
      std::chrono::duration<double>(function_end_time - function_start_time)
          .count();
  std::cout << "Finished running pivot_mtt with a " << mtt.length
            << "-row dataset in " << function_run_time << " seconds.\n";

  return pivot_mtt;
}

void print_mtt(Multi_Type_Table mtt, long n, char print_separator) {
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

  for (int i = 0; i < mtt.field_order.size(); i++) {
    std::cout << mtt.field_order[i] << print_separator;
  }
  std::cout << "\n";

  // Printing values:

  int rows_to_print = 0;

  if (n == 0) // In this case, all values will be printed.
  {
    rows_to_print = mtt.length;
  }

  else // Making sure we don't attempt to print more rows than
  // the Multi_Type_Table contains

  {
    rows_to_print = std::min(long(mtt.length), n);
  }

  for (int i = 0; i < rows_to_print; i++)

  {

    for (int j = 0; j < mtt.field_order.size(); j++) {
      // Determining which type-specific valarray this field is in,
      // then retrieving it from that valarray:

      if (mtt.stf.contains(mtt.field_order[j])) {
        std::cout << mtt.stf[mtt.field_order[j]][i] << print_separator;
      }

      else if (mtt.df.contains(mtt.field_order[j])) {
        std::cout << mtt.df[mtt.field_order[j]][i] << print_separator;
      }

      else if (mtt.lf.contains(mtt.field_order[j])) {
        std::cout << mtt.lf[mtt.field_order[j]][i] << print_separator;
      }

      else if (mtt.intf.contains(mtt.field_order[j])) {
        std::cout << mtt.intf[mtt.field_order[j]][i] << print_separator;
      }
    }
    std::cout << "\n";
  }
}

void mtt_to_csv(Multi_Type_Table &mtt, std::string file_path, char delimiter) {
  // This function exports a Multi_Type_Table to a CSV file.

  // delimiter specifies which character to use to separate
  // field entries. ',', the default, should work well as long as
  // no commas are present within field names (in which case you
  // may want to use '/t' instead).

  // This function will add all of each field's values
  // at once rather than entering data for all fields row by row.
  // This is meant to help speed up
  // the function, as it will reduce the number of checks needed
  // to determine in which typed valarray a given field lies.

  auto function_start_time = std::chrono::high_resolution_clock::now();

  std::vector<std::string> csv_data{
      size_t(mtt.length + 1)}; // All values will need to
  // get exported to string format in order to get correctly exported
  // to the CSV file.
  // Initializing the vector with blank empty strings will make it
  // easier to access each entry using bracket notation. (We'll need
  // to add one additional entry to account for the header row.)

  for (int i = 0; i < mtt.field_order.size(); i++) {
    std::string field_name = mtt.field_order[i];
    // Adding this value to the table's header row:
    csv_data[0] += field_name;

    bool last_column = false;
    if (i == (mtt.field_order.size() - 1)) {
      last_column = true;
    }

    // We'll need to add a line break at the end of csv_data[0]
    // if this is the last field in field_order;
    // otherwise, we'll add a separator.

    if (last_column == true) {
      csv_data[0] += "\n";
    }

    else {
      csv_data[0] += delimiter;
    }

    // Determining which type-specific valarray this field is in,
    // then retrieving it from that valarray:

    // The following code could likely be simplified by creating
    // a separate function for adding in row data that takes
    // a template as an argument.

    if (mtt.stf.contains(field_name)) {

      std::valarray<std::string> va = mtt.stf[field_name];

      for (int j = 0; j < mtt.length; j++) {
        // Adding this field to the string for a given row:
        // Offsetting j by 1 prevents this code from overwriting the
        // header row.

        csv_data[j + 1] += va[j];

        if (last_column == true) {
          csv_data[j + 1] += "\n";
        }

        else {
          csv_data[j + 1] += delimiter;
        }
      }

    }

    else if (mtt.df.contains(field_name)) {

      std::valarray<double> va = mtt.df[field_name];

      for (int j = 0; j < mtt.length; j++) {
        // Non-string values will need to get converted to strings
        // before they can get added into the CSV data.
        csv_data[j + 1] += std::to_string(va[j]);

        if (last_column == true) {
          csv_data[j + 1] += "\n";
        }

        else {
          csv_data[j + 1] += delimiter;
        }
      }

    }

    else if (mtt.lf.contains(field_name)) {

      std::valarray<long> va = mtt.lf[field_name];

      for (int j = 0; j < mtt.length; j++) {

        csv_data[j + 1] += std::to_string(va[j]);

        if (last_column == true) {
          csv_data[j + 1] += "\n";
        }

        else {
          csv_data[j + 1] += delimiter;
        }
      }

    }

    else if (mtt.intf.contains(field_name)) {

      std::valarray<int> va = mtt.intf[field_name];

      for (int j = 0; j < mtt.length; j++) {

        csv_data[j + 1] += std::to_string(va[j]);

        if (last_column == true) {
          csv_data[j + 1] += "\n";
        }

        else {
          csv_data[j + 1] += delimiter;
        }
      }
    }
  }

  // Exporting the contents of csv_data to a CSV file:

  // The following line is based on
  // https://en.cppreference.com/cpp/io/basic_ofstream .
  std::ofstream ofs(file_path, std::ios::out);

  for (int i = 0; i < csv_data.size(); i++)

  {

    ofs << csv_data[i];
  }

  auto function_end_time = std::chrono::high_resolution_clock::now();
  auto function_run_time =
      std::chrono::duration<double>(function_end_time - function_start_time)
          .count();
  std::cout << "Finished running mtt_to_csv with a " << mtt.length
            << "-row dataset in " << function_run_time << " seconds.\n";
}