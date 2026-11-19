/* Valarray-Variant Pivot Table CReation
by Ken Burchfiel
Released under the MIT License
This script is based on pivot_table_creation.cpp, but uses
maps of variants of valarrays, rather than a Multi_Type_Table,
to store table data.
*/

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "mvvt.h"

int main() {

  std::vector<std::vector<std::string>> config_vvs = csv_to_vvs(
  "config.csv");

  print_vvs(config_vvs);


  std::pair<Import_Config, std::map<int, 
  Pivot_Config>> ic_pcm = import_config_data(
  config_vvs);

  Import_Config ic = ic_pcm.first;
  std::map<int, Pivot_Config> pcm = ic_pcm.second;

// Importing CSV data into a map of variants of valarrays:

  MVV_Table csv_mvvt =
      csv_to_mvvt(ic.data_file_path, 
        ic.field_type_map, 
        ic.fields_to_import,
        ic.delimiter, 
        ic.csv_vals_to_include, 
        ic.csv_vals_to_exclude);



  char pivot_index_separator = '|';

  // std::cout << "Checkpoint A.";
  // std::cout << "Checkpoint A2.";
  std::cout << "Length of csv_mvvt:" << csv_mvvt.length << "\n";

  std::cout << "Sample contents of csv_mvvt:\n";

  print_mvvt(csv_mvvt, 10);

  }