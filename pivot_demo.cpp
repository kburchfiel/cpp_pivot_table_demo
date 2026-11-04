/* C++ Pivot-Table Demo
by Ken Burchfiel
Released under the MIT License

The CSV-import portion of this code was based on a small CSV-import
project that I had created in 2025; it's available at
https://github.com/kburchfiel/cpp_manual_import_test. That project,
in turn, is based on the following resources:
https://en.cppreference.com/w/cpp/string/basic_string/getline;
Kerrek SB's response at
https://stackoverflow.com/a/12133398/13097194 ;
Programming: Principles and Practice (3rd Edition)
by Bjarne Stroustrup (especially p. 354); and
A Tour of C++ (2nd Edition) by Bjarne Stroustrup.

Parts of this code were also based on the pivot_compressors.cpp
script within my https://github.com/kburchfiel/cpp_pivot_tables
project.

Other references will be mentioned within the codebase.

To do:

1. Add additional documentation
2. Report the time each function takes to run within this file.
3. Continue working on your code that creates a pivot table
out of a Multi_Type_Table. Specifically, (1) convert the 
table data into a struct of maps of valarrays of various types
(ideally a Multi_Type_Table itself) and (2) use vector division
with this table to calculate means. Don't worry about column
order for now, as you can specify an updated order within your 
CSV-export script.
4. Add in code that can export a multi-type-table to a CSV file.
5. Compare how long your code takes to run via your two different
approaches to storing pivot-table data
(i.e. as a struct of maps of valarrays and as a vector
of string/string maps.)
5. Share pivot-table functions/applications within StackOverflow
post that asked about using C++ for pivot tables (but mention its
limitations, together with better alternatives like
the DataFrame library and Vicent La's library)

*/

#include "pivot_functions.h"
#include "simple_csv_import_export.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <valarray>
#include <variant>
#include <vector>

struct Multi_Type_Table {

  /*This struct is meant
  to allow spreadsheet-type data with multiple fields
  to get stored in memory, thus facilitating various
  calculations.
  Columns of data will get stored as maps,
  with column names as keys and data values as
  valarrays of various types.

  Note: I had also considered storing spreadsheet data as
  a map with strings as keys and variants as values.
  (The variants would include valarrays for various
  field types.) However, StackOverflow responses suggest
  that variants might slow down various processes,
  so I decided to go with a struct instead.

  // Currently, the struct only supports four different types
  // of field data, which
  // may be either too few or too many for certain calculations.
  // However, the struct can also be easily adapted to accommodate
  // additional types.

  */

  // The following vector of fields allows us to keep track
  // of the spreadsheet's column order (which can help export
  // data with the same field order that its source file used.)

  std::vector<std::string> field_order{};

  long length = 0; // This should probably be made
  // private so that users can't mess with it!

  // I could also add a dictionary that maps each field to
  // a given type, but since types can also be checked by
  // determining the map (stf, intf, etc.) within which a given
  // field serves as a key, this may not be necessary.

  // String fields:
  std::map<std::string, std::valarray<std::string>> stf{};
  // Integer fields:
  std::map<std::string, std::valarray<int>> intf{};
  // Long fields:
  std::map<std::string, std::valarray<long>> lf{};
  // Double fields:
  std::map<std::string, std::valarray<double>> df{};
};

struct Pivot_Row {

  // This struct will hold pivot-table data for a given pivot index.
  // I'm using separate maps for double- and long-typed data,
  // though I could also have tried out a variant.

  std::map<std::string, double> df{};
  std::map<std::string, long> lf{};
};

Multi_Type_Table csv_to_mtt(std::string file_path,
                            std::map<std::string, std::string> field_type_map,
                            std::set<std::string> fields_to_import = {},
                            char delimiter = ',') {
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
    // valarrays will be set to row_count's size. I confirmed
    // that this is the correct approach; decreasing this size by 1
    // resulted in an error, and increasing it by 1 created a spare
    // 0 at the end.

    mtt.length = row_count;
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

  int row_index = 0;

  for (std::string row; getline(stfs, row);) {
    // std::cout << "Row:" << row << "\n";
    if (row_index == 0) // We'll want to treat the first row differently,
    // as we're assuming that it includes column names rather than
    // values.
    {
      // std::cout << "First row.\n";
      std::istringstream first_row;
      first_row.str(row); // Based on:
      // https://en.cppreference.com/w/cpp/string/basic_string/getline

      // Using getline() with a tab delimiter argument in order
      // to go through each line within our istringstream (i.e.
      // the first row) and then extract all field names from it:
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
            std::valarray<std::string> va(row_count);
            // Since 'va' is only present within this scope,
            // we can reuse it as a name for other new
            // valarrays.

            mtt.stf[field_name] = va;
          }

          else if (field_type_map[field_name] == "string") {
            std::valarray<std::string> va(row_count);
            // Since 'va' is only present within this scope,
            // we can reuse it as a name for other new
            // valarrays.

            mtt.stf[field_name] = va;
          }

          else if (field_type_map[field_name] == "double") {
            std::valarray<double> va(row_count);
            // Since 'va' is only present within this scope,
            // we can reuse it as a name for other new
            // valarrays.

            mtt.df[field_name] = va;
          }

          else if (field_type_map[field_name] == "int") {
            std::valarray<int> va(row_count);
            // Since 'va' is only present within this scope,
            // we can reuse it as a name for other new
            // valarrays.

            mtt.intf[field_name] = va;
          }

          else if (field_type_map[field_name] == "long") {
            std::valarray<long> va(row_count);
            // Since 'va' is only present within this scope,
            // we can reuse it as a name for other new
            // valarrays.

            mtt.lf[field_name] = va;
          }

          // You might want to raise an error here if none of the
          // previous conditions was met.

          col_index++;
        }
      }

    }

    else { // In this case, the current row will be considered to
      // contain values rather than headers.

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
        if (col_indices.contains(
                col_index)) { // Adding this value to the column's dedicated
                              // valarray (and
          // converting it to the proper type beforehand if necessary):

          std::string field_name = col_indices[col_index];

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

  return mtt;
}

int main() {

  // Testing out an import of CSV data into a Multi_Type_Table:

  // Importing CSV data. Source:
  //   Air Carriers : T-100 Segment (All Carriers) table
  // from the Bureau of Transportation Statistics
  // https://www.transtats.bts.gov/DL_SelectFields.aspx?gnoyr_VQ=FMG&QO_
  // fu146_anzr=Nv4+Pn44vr45

  std::string file_path =
      "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols_truncated.csv";

  // Passengers should really be a long-type field as well,
  // but I'm making it a double field for testing purposes.
  Multi_Type_Table csv_mtt =
      csv_to_mtt(file_path,
                 {{"PASSENGERS", "double"},
                  {"FREIGHT", "long"},
                  {"UNIQUE_CARRIER", "string"},
                  {"ORIGIN", "string"}},
                 {"PASSENGERS", "FREIGHT", "UNIQUE_CARRIER", "ORIGIN"});

  std::cout << "Checking a few values within this new object.\n";
  std::cout << "Field-order size: " << csv_mtt.field_order.size() << "\n";
  std::cout << "Field-order values:\n";
  for (int i = 0; i < csv_mtt.field_order.size(); i++) {
    std::cout << csv_mtt.field_order[i] << " ";
  }
  std::cout << "\n";

  std::cout << "ORIGIN-valarray size: " << csv_mtt.stf["ORIGIN"].size() << "\n";

  std::cout << "ORIGIN-valarray values:\n";
  for (int i = 0; i < csv_mtt.stf["ORIGIN"].size(); i++) {
    std::cout << csv_mtt.stf["ORIGIN"][i] << " ";
  }

  std::cout << "PASSENGERS-valarray size: " << csv_mtt.df["PASSENGERS"].size()
            << "\n";

  std::cout << "PASSENGERS-valarray values:\n";
  for (int i = 0; i < csv_mtt.df["PASSENGERS"].size(); i++) {
    std::cout << csv_mtt.df["PASSENGERS"][i] << " ";
  }

  std::cout << "\nFinished checking values.\n";

  // Testing out code that creates a pivot table using MTT data:

  // This code creates a pivot table using
  // Multi_Type_Table data.

  std::map<std::string, Pivot_Row> pivot_table;
  Multi_Type_Table mtt = csv_mtt;

  // std::cout << "Checkpoint Z.";

  bool calculate_means = true;

  std::vector<std::string> double_value_fields{"PASSENGERS"};

  std::vector<std::string> long_value_fields{"FREIGHT"};

  std::vector<std::string> int_value_fields{};

  std::vector<std::string> index_fields{"UNIQUE_CARRIER", "ORIGIN"};

  char pivot_index_separator = '|';

  // std::cout << "Checkpoint A.";
  // std::cout << "Checkpoint A2.";
  std::cout << "Length of MTT:" << mtt.length << "\n";

  for (int i = 0; i < mtt.length; i++)

  {
    // std::cout << "Checkpoint B.";
    // Combining index fields into a single string that can serve
    // as a dictionary key:
    std::string pivot_index = "";
    for (int j = 0; j < index_fields.size(); j++) {
      // Retrieving the index values for this particular
      // row:
      pivot_index += csv_mtt.stf[index_fields[j]][i];
      // We should add a separator after this field as long as it's
      // not the final entry.
      if (j != index_fields.size() - 1) {
        pivot_index += pivot_index_separator;
      }
    }

    // std::cout << "Checkpoint C";

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

      for (int j = 0; j < double_value_fields.size(); j++)
      {
        // Note that sums will get added to the df (double-field)
        // attribute of pr, whereas counts will get added
        // to the lf. In order to help simplify this function,
        // this will be the case for all metric fields,
        // whether integer-, double-, or long-based. (It may be
        // ideal, however, to update this code in the future
        // so that integer and long sums get stored within
        // lf rather than df.)
  pr.df[double_value_fields[j]+"_sum"] = 0.0;
  pr.lf[double_value_fields[j]+"_count"] = 0;
      }


      for (int j = 0; j < int_value_fields.size(); j++)
      {
  pr.df[int_value_fields[j]+"_sum"] = 0.0;
  pr.lf[int_value_fields[j]+"_count"] = 0;
      }


      for (int j = 0; j < long_value_fields.size(); j++)
      {
  pr.df[long_value_fields[j]+"_sum"] = 0.0;
  pr.lf[long_value_fields[j]+"_count"] = 0;
      }



    }

    else {
          // In this case, we'll add new values to the existing 
      // dictionary.


      for (int j = 0; j < double_value_fields.size(); j++)
      {
  pivot_table[pivot_index].df[double_value_fields[j]+"_sum"] += mtt.df[
  double_value_fields[j]][i];
  pivot_table[pivot_index].lf[double_value_fields[j]+"_count"] ++;
      }


      for (int j = 0; j < int_value_fields.size(); j++)
      {
  pivot_table[pivot_index].df[int_value_fields[j]+"_sum"] += double(
  mtt.df[int_value_fields[j]][i]);
  pivot_table[pivot_index].lf[int_value_fields[j]+"_count"] ++;
      }


      for (int j = 0; j < long_value_fields.size(); j++)
      {
  pivot_table[pivot_index].df[long_value_fields[j]+"_sum"] += double(
  mtt.df[long_value_fields[j]][i]);
  pivot_table[pivot_index].lf[long_value_fields[j]+"_count"] ++;
      }


    }
  }

  // std::cout << "Returning 0.\n";
  // return 0;

  std::pair<std::vector<std::map<std::string, std::string>>,
            std::vector<std::string>> const csv_data_and_header =
      csv_to_vmss(
          // The following, commented-out file is useful for testing
          // and debugging purposes, as it's much smaller (and thus won't
          // take as long to process).
          // "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols_truncated.csv"
          "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols.csv");

  std::vector<std::map<std::string, std::string>> const csv_data =
      csv_data_and_header.first;
  std::vector<std::string> const col_names = csv_data_and_header.second;

  std::cout << "Length of csv_data: " << csv_data.size() << "\n";

  // Creating a pivot table:

  // char pivot_index_separator = '|'; // Defined earlier
  //std::vector<std::string> index_fields{"UNIQUE_CARRIER", "ORIGIN"};
  // defined earlier
  std::vector<std::string> value_fields{"PASSENGERS", "FREIGHT"};

  std::pair<
      std::map<std::string, std::map<std::string, std::variant<double, long>>>,
      std::vector<std::string>> const pivot_and_header =
      vmss_pivot_table(csv_data, index_fields, value_fields);

  std::map<std::string, std::map<std::string, std::variant<double, long>>>
      pivot_table_from_vmss = pivot_and_header.first;

  std::vector<std::string> header_row = pivot_and_header.second;

  std::cout << "Number of keys within pivot_table_vmss: " << pivot_table_from_vmss.size()
            << "\n";

  // Adding in mean columns now that sums and counts have been calculated:
  // (vmss_pivot_table could also be updated to perform this task,
  // but I wanted to demonstrate how the output of the pivot-table
  // function can be further updated if needed.)

  // Iterating through all keys of the pivot table:
  // (This code is based on p. 580 of PPAP3 and
  //  https://stackoverflow.com/a/26282004/13097194)

  for (auto &[pivot_index, 
    value_map] : pivot_table_from_vmss) {

    for (auto &[value_key, value] : value_map) {

      for (int i = 0; i < value_fields.size(); i++) {
        value_map[value_fields[i] + "_mean"] =
            std::get<double>(value_map[value_fields[i] + "_sum"]) /
            std::get<long>(value_map[value_fields[i] + "_count"]);
      }
    }
  }

  // We'll also update our header-column variable with these new
  // values:

  for (int i = 0; i < value_fields.size(); i++) {
    header_row.push_back(value_fields[i] + "_mean");
  }

  // Exporting the pivot table: (I'll do so by first converting the
  // pivot-table data into a vector of string/string maps, which is what my
  // CSV-export function accepts as input, together with a list of header
  // fields.)

  // (Alternatively, I could have created a CSV-export function that
  // would accept a vector of vectors of strings, which would work fine
  // here also provided that I make sure to add values to each vector
  // in the correct order.)

  std::vector<std::map<std::string, std::string>> pivot_table_vmss =
      pivot_table_to_vmss(pivot_table_from_vmss, header_row);

  vmss_to_csv("pivot_data.csv", pivot_table_vmss, header_row);

}
