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
3. Create a CSV-import function that returns data as vectors
of string/variant maps. (Add integers, longs, doubles, floats,
and strings as variant options.) Next, create a pivot-table
function that can process this data. Finally, compare the time
these functions require to run (on the same dataset) to your
existing CSV-import and pivot-table functions.
4. Update your CSV import (and, possibly, export) scripts to
read or write only a subset of columns.
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
    //std::cout << "Row:" << row << "\n";
    if (row_index == 0) // We'll want to treat the first row differently,
    // as we're assuming that it includes column names rather than
    // values.
    {
//std::cout << "First row.\n";
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
        if ((fields_to_import.empty()) || (fields_to_import.contains(field_name)))

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

      for (std::string field_val; getline(current_row, 
      field_val, delimiter);) {
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

  Multi_Type_Table csv_mtt = csv_to_mtt(file_path, 
  {
    {"PASSENGERS", "long"}, 
  {"FREIGHT", "long"}, 
  {"UNIQUE_CARRIER", "string"}, 
  {"ORIGIN", "string"}},
  {"PASSENGERS", "FREIGHT", 
  "UNIQUE_CARRIER", "ORIGIN"});


  std::cout << "Checking a few values within this new object.\n";
  std::cout << "Field-order size: " << csv_mtt.field_order.size() << "\n";
  std::cout << "Field-order values:\n";
    for (int i = 0; i < csv_mtt.field_order.size(); i++)
    {std::cout << csv_mtt.field_order[i] << " ";
    }
    std::cout << "\n";

std::cout << "ORIGIN-valarray size: " << csv_mtt.stf["ORIGIN"].size() << "\n";

std::cout << "ORIGIN-valarray values:\n";
  for (int i = 0; i < csv_mtt.stf["ORIGIN"].size(); i++){
  std::cout << csv_mtt.stf["ORIGIN"][i] << " ";}

std::cout << "PASSENGERS-valarray size: " << csv_mtt.lf["PASSENGERS"].size() << "\n";

std::cout << "PASSENGERS-valarray values:\n";
  for (int i = 0; i < csv_mtt.lf["PASSENGERS"].size(); i++){
  std::cout << csv_mtt.lf["PASSENGERS"][i] << " ";}


std::cout << "\nFinished checking values.\n";

      return 0;

  


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

  char pivot_index_separator = '|';
  std::vector<std::string> index_fields{"UNIQUE_CARRIER", "ORIGIN"};
  std::vector<std::string> value_fields{"PASSENGERS", "FREIGHT"};

  std::pair<
      std::map<std::string, std::map<std::string, std::variant<double, long>>>,
      std::vector<std::string>> const pivot_and_header =
      vmss_pivot_table(csv_data, index_fields, value_fields);

  std::map<std::string, std::map<std::string, std::variant<double, long>>>
      pivot_table = pivot_and_header.first;

  std::vector<std::string> header_row = pivot_and_header.second;

  std::cout << "Number of keys within pivot_table: " << pivot_table.size()
            << "\n";

  // Adding in mean columns now that sums and counts have been calculated:
  // (vmss_pivot_table could also be updated to perform this task,
  // but I wanted to demonstrate how the output of the pivot-table
  // function can be further updated if needed.)

  // Iterating through all keys of the pivot table:
  // (This code is based on p. 580 of PPAP3 and
  //  https://stackoverflow.com/a/26282004/13097194)

  for (auto &[pivot_index, value_map] : pivot_table) {

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
      pivot_table_to_vmss(pivot_table, header_row);

  vmss_to_csv("pivot_data.csv", pivot_table_vmss, header_row);
}
