/* Script for creating pivot tables in C++
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
*/

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "mtt.h"

int main() {

  std::vector<std::vector<std::string>> config_vvs = csv_to_vvs(
  "config.csv");

  print_vvs(config_vvs);


  std::pair<Import_Config, std::map<int, 
  Pivot_Config>> ic_pcm = import_config_data(
  config_vvs);

  Import_Config ic = ic_pcm.first;
  std::map<int, Pivot_Config> pcm = ic_pcm.second;

// Importing CSV data into a Multi_Type_Table:

  Multi_Type_Table csv_mtt =
      csv_to_mtt(ic.data_file_path, 
        ic.field_type_map, 
        ic.fields_to_import,
        ic.delimiter, 
        ic.csv_vals_to_include, 
        ic.csv_vals_to_exclude);

  char pivot_index_separator = '|';

  // std::cout << "Checkpoint A.";
  // std::cout << "Checkpoint A2.";
  std::cout << "Length of csv_mtt:" << csv_mtt.length << "\n";

  std::cout << "Sample contents of csv_mtt:\n";

  print_mtt(csv_mtt, 10);

  // Calling pivot_mtt for each set of pivot-table configuration
  // values specified within pcm in order to convert the data
  // in csv_mtt into a pivot table, then calling mtt_to_csv
  // to export this table to a CSV file:

  for (auto &[pivot_index, pc] : pcm)

  {
    std::cout << "Creating pivot table #" << pivot_index << ".\n";

    print_pivot_config_data(pc);

    Multi_Type_Table pivot_mtt =
        mtt_pivot_table(csv_mtt, pc.index_fields, pc.double_value_fields,
                        pc.long_value_fields, pc.int_value_fields,
                        '|', // We could also update the code to allow the user
                        // to select this option.
                        pc.calculate_means, pc.string_vals_to_include,
                        pc.string_vals_to_exclude, pc.int_vals_to_include,
                        pc.int_vals_to_exclude, pc.double_vals_to_include,
                        pc.double_vals_to_exclude, pc.long_vals_to_include,
                        pc.long_vals_to_exclude);
    // );


    std::cout << "\nSample pivot_mtt content:\n\n";

    print_mtt(pivot_mtt, 10);

    std::cout << "Exporting pivot_mtt to a CSV file.\n";

    mtt_to_csv(pivot_mtt, pc.table_file_path);

    std::cout << "Finished pivot_mtt export for table #" << pivot_index
              << ".\n";
  }
}
