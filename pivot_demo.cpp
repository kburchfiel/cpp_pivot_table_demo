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
      const csv_data_and_header = csv_to_vmss(
        // The following, commented-out file is useful for testing
        // and debugging purposes, as it's much smaller (and thus won't
        // take as long to process).
        // "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols_truncated.csv"
        "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols.csv"
        );
    

  std::vector<std::map<std::string, std::string>> const csv_data =
      csv_data_and_header.first;
  std::vector<std::string> const col_names = csv_data_and_header.second;

  std::cout << "Length of csv_data: " << csv_data.size() << "\n";

  // Creating a pivot table:

  char pivot_index_separator = '|';
  std::vector<std::string> index_fields{"UNIQUE_CARRIER", "ORIGIN"};
  std::vector<std::string> value_fields{"PASSENGERS", "FREIGHT"};

  std::pair<std::map<std::string, std::map<std::string, 
  std::variant<double, long>>>, std::vector<
  std::string>> const pivot_and_header = vmss_pivot_table(
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

  // (Alternatively, I could have created a CSV-export function that 
  // would accept a vector of vectors of strings, which would work fine 
  // here also provided that I make sure to add values to each vector 
  // in the correct order.)

  std::vector<std::map<std::string, 
    std::string>> pivot_table_vmss = pivot_table_to_vmss(pivot_table,
    header_row);

  vmss_to_csv("pivot_data.csv", pivot_table_vmss,
              header_row);
}
