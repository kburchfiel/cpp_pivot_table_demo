// C++ Pivot-Table Demo
// by Ken Burchfiel
// Released under the MIT License

// The CSV-import portion of this code was based on a small CSV-import
// project that I had created in 2025; it's available at
// https://github.com/kburchfiel/cpp_manual_import_test. That project,
// in turn, is based on the following resources:
// https://en.cppreference.com/w/cpp/string/basic_string/getline;
// Kerrek SB's response at
// https://stackoverflow.com/a/12133398/13097194 ;
// Programming: Principles and Practice (3rd Edition)
// by Bjarne Stroustrup (especially p. 354); and
// A Tour of C++ (2nd Edition) by Bjarne Stroustrup.

// Parts of this code were also based on the pivot_compressors.cpp
// script within my https://github.com/kburchfiel/cpp_pivot_tables
// project.

// Other references will be mentioned within the codebase.

// To do:

// 1. Add additional documentation
// 2. Convert the pivot-table-generation script into a function
// and move it into a separate file. (Have it return the header
// rows so that they can be applied later on.)

#include "simple_csv_import_export.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

int main() {

  // Importing CSV data. Source:
//   Air Carriers : T-100 Segment (All Carriers) table
// from the Bureau of Transportation Statistics
// https://www.transtats.bts.gov/DL_SelectFields.aspx?gnoyr_VQ=FMG&QO_
// fu146_anzr=Nv4+Pn44vr45

  std::pair<std::vector<std::map<std::string, std::string>>,
            std::vector<std::string>>
      csv_data_and_header = csv_to_vmss(
        //   "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols_truncated.csv"
          "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols.csv"
        );
    

  std::vector<std::map<std::string, std::string>> csv_data =
      csv_data_and_header.first;
  std::vector<std::string> col_names = csv_data_and_header.second;

  std::cout << "Length of csv_data: " << csv_data.size() << "\n";

  // Creating a pivot table:
  // (Move this function over to a separate header file once it's ready.)

  char pivot_index_separator = '|';

  std::vector<std::string> index_fields{"UNIQUE_CARRIER", "ORIGIN"};

  std::vector<std::string> value_fields{"PASSENGERS", "FREIGHT"};

  /* Determining the header row for the pivot table (which will prove
  useful for CSV exports): */

  std::vector<std::string> header_row = {};

  // Adding the index field to the header row:
  std::string index_name = "";
  for (int i = 0; i < index_fields.size(); i++) {
    index_name += (index_fields[i]);
    // We should add a separator after this field as long as it's
    // not the final entry.
    if (i != index_fields.size() - 1) {
      index_name += pivot_index_separator;
    }
  }

  header_row.push_back(index_name);

  // Adding the value fields to the header row:

  for (int i = 0; i < value_fields.size(); i++) {
    // String-based values will need to get converted to doubles before
    // they can be added to the pivot table.
    header_row.push_back(value_fields[i] + "_sum");
    header_row.push_back(value_fields[i] + "_count");
  }

  std::cout << "Header row: ";
  for (int i = 0; i < header_row.size(); i++) {
    std::cout << header_row[i] << " ";
  }
  std::cout << "\n";

  std::map<std::string, std::map<std::string, std::variant<double, int>>>
      pivot_table;

  for (int i = 0; i < csv_data.size(); i++)

  {
    // Retrieving the current row of the dataset:
    std::map<std::string, std::string> pivot_row = csv_data[i];

    // Combining index fields into a single string that can serve
    // as a dictionary key:
    std::string pivot_index = "";
    for (int j = 0; j < index_fields.size(); j++) {
      pivot_index += pivot_row[index_fields[j]];
      // We should add a separator after this field as long as it's
      // not the final entry.
      if (j != index_fields.size() - 1) {
        pivot_index += pivot_index_separator;
      }
    }
    // std::cout << "pivot_index: " << pivot_index << "\n";

    /* Checking whether this field is already present in our list of
    pivot-index keys: (If it isn't, we'll initialize a new value
    for it, which will take the form of a map.)

    NOTE: This check may not actually be necessary. See if you still
    get correct results if you skip it--which would save some processing
    time.*/

    if (pivot_table.contains(pivot_index) == false) {
      // std::cout << "Creating a new pivot_index." << "\n";
      std::map<std::string, std::variant<double, int>> value_map{};

      // For each value in value_fields, we'll add double entries that
      // will store sums, and integer entries that will store counts.
      // (This approach won't work for all use cases, of course--but
      // I wanted to keep this function simple for now.)
      for (int j = 0; j < value_fields.size(); j++) {
        // String-based values will need to get converted to doubles before
        // they can be added to the pivot table.
        value_map[value_fields[j] + "_sum"] = stod(pivot_row[value_fields[j]]);
        value_map[value_fields[j] + "_count"] = 1;

        pivot_table[pivot_index] = value_map;

        // pivot_table[pivot_index][value_fields[j]+"_sum"] = std::get<int>(
        // pivot_table[pivot_index][value_fields[j]+"_sum"]) + 3;
      }
    }

    else {
      // std::cout << "Updating existing value for pivot_index." << "\n";
      // Updating the value of the current pivot-index key with
      // this row's values:

      for (int j = 0; j < value_fields.size(); j++) {
        // String-based values will need to get converted to doubles before
        // they can be added to the pivot table.
        pivot_table[pivot_index][value_fields[j] + "_sum"] =
            std::get<double>(
                pivot_table[pivot_index][value_fields[j] + "_sum"]) +
            stod(pivot_row[value_fields[j]]);
        pivot_table[pivot_index][value_fields[j] + "_count"] =
            std::get<int>(
                pivot_table[pivot_index][value_fields[j] + "_count"]) +
            1;
      }
    }
  }

  std::cout << "Number of keys within pivot_table:" << pivot_table.size() << "\n";

  // Adding in mean columns now that sums and counts have been calculated:

  // Iterating through all keys of the pivot table:
  // (This code is based on p. 580 of PPAP3 and
  //  https://stackoverflow.com/a/26282004/13097194)

  for (auto &[pivot_index, value_map] : pivot_table) {

    for (auto &[value_key, value] : value_map) {
    //   if (std::holds_alternative<int>(value)) {
    //     std::cout << std::get<int>(value) << " ";
    //   } else if (std::holds_alternative<double>(value)) {
    //     std::cout << std::get<double>(value) << " ";
    //   }

      //std::cout << "\n";

          for (int i = 0; i < value_fields.size(); i++)
          {
          value_map[value_fields[i]+"_mean"] = std::get<double>(value_map[
      value_fields[i]+"_sum"]) /
      std::get<int>(value_map[value_fields[i]+"_count"]);
      }
    }
  }

  // We'll also update our header-column variable with these new
  // values:

  for (int i = 0; i < value_fields.size(); i++)
  {
  header_row.push_back(value_fields[i]+"_mean");
  }

  // Exporting the pivot table: (I'll do so by first converting the pivot-table
  // data into a vector of string/string maps, which is what my CSV-export
  // function accepts as input, together with a list of header fields.)

  // (Alternatively, I could have created a function that would accept
  // a vector of vectors of strings, which would work fine here also
  // provided that I make sure to add values to each vector in the
  // correct order.)

  std::vector<std::map<std::string, std::string>> pivot_table_vmss;
  // vmss is short for "vector of maps of strings & strings"

  for (auto const &[pivot_index, value_map] : pivot_table) {
    std::map<std::string, std::string> mss{};
    // Adding the pivot index as the first entry for the map:
    mss[index_name] = pivot_index;
    // Iterating through the header row columns following the pivot
    // index in order to add those columns' values for the current row
    // to the map:
    for (auto const &[header_field, row_value] :
         value_map) { // Sum columns should be retrieved as doubles, whereas
                      // count
      // columns should be retrieved as integers.

      // std::cout << "Last 4 digits of field:" <<
      // header_field.substr(header_field.size()-5, 4); if
      // (header_field.substr(header_field.size()-5, 4) == "_sum") Subtracting 5
      // from the header field should take us to the start of the '_sum' suffix
      // (if it exists). Note that this code should precede the check for
      // "_double" because the latter might result in out-of-bounds access for
      // particularly-short column names.
      if (std::holds_alternative<double>(row_value)) {
        mss[header_field] = std::to_string(std::get<double>(row_value));
      }

      // else if (header_field.substr(header_field.size()-8), 7) == "_double")
      else if (std::holds_alternative<int>(row_value)) {
        mss[header_field] = std::to_string(std::get<int>(row_value));
      }

      // It would be ideal to raise an error here if neither of the previous
      // if statements is true.
    }

    pivot_table_vmss.push_back(mss);
  }

  vmss_to_csv("pivot_data.csv", csv_data = pivot_table_vmss,
              col_names = header_row);
}
