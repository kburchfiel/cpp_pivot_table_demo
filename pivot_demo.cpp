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
// 3. Report the time each function takes withinh the terminal.

#include "simple_csv_import_export.h"
#include "pivot_functions.h"
#include <cstdlib>
#include <ctime>
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

  char pivot_index_separator = '|';
  std::vector<std::string> index_fields{"UNIQUE_CARRIER", "ORIGIN"};
  std::vector<std::string> value_fields{"PASSENGERS", "FREIGHT"};

  std::pair<std::map<std::string, std::map<std::string, 
  std::variant<double, long>>>, std::vector<
  std::string>> pivot_and_header = vmss_pivot_table(
  csv_data, index_fields, value_fields);

std::map<std::string, std::map<std::string, 
  std::variant<double, long>>> pivot_table = pivot_and_header.first;

  std::vector<std::string> header_row = pivot_and_header.second;

  std::cout << "Number of keys within pivot_table: " 
  << pivot_table.size() << "\n";

  // Adding in mean columns now that sums and counts have been calculated:
  // (vmss_pivot_table could also be updated to perform this task,
  // but I wanted to demonstrate how the output of the pivot-table
  // function can be further updated if needed.)

  // Iterating through all keys of the pivot table:
  // (This code is based on p. 580 of PPAP3 and
  //  https://stackoverflow.com/a/26282004/13097194)

  for (auto &[pivot_index, value_map] : pivot_table) {

    for (auto &[value_key, value] : value_map) {

          for (int i = 0; i < value_fields.size(); i++)
          {
          value_map[value_fields[i]+"_mean"] = std::get<double>(value_map[
      value_fields[i]+"_sum"]) /
      std::get<long>(value_map[value_fields[i]+"_count"]);
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
    mss[header_row[0]] = pivot_index; // header_row[0] contains the
    // name of the index values.
    // Iterating through the header row columns following the pivot
    // index in order to add those columns' values for the current row
    // to the map:
    for (auto const &[header_field, row_value] :
         value_map) { // Sum columns should be retrieved as doubles, whereas
                      // count columns should be retrieved as longs.

      if (std::holds_alternative<double>(row_value)) {
        mss[header_field] = std::to_string(std::get<double>(row_value));
      }

      // else if (header_field.substr(header_field.size()-8), 7) == "_double")
      else if (std::holds_alternative<long>(row_value)) {
        mss[header_field] = std::to_string(std::get<long>(row_value));
      }

      // It would be ideal to raise an error here if neither of the previous
      // if statements is true.
    }

    pivot_table_vmss.push_back(mss);
  }

  vmss_to_csv("pivot_data.csv", csv_data = pivot_table_vmss,
              col_names = header_row);
}
