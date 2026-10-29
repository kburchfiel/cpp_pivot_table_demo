/* Simple CSV Import/Export:
Functions for importing CSV files into memory, then exporting them back to a
.csv file By Ken Burchfiel Released under the MIT License The import component
of this script is based on code from my CPP Manual Import Test repository at
https://github.com/kburchfiel/cpp_manual_import_test (which is also released
under the MIT License.)*/

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

// #include <error>

std::pair<std::vector<std::map<std::string, std::string>>, 
std::vector<std::string>>
csv_to_vmss(std::string file_path, char delimiter = ',');

void vmss_to_csv(std::string file_path,
std::vector<std::map<std::string, std::string>> vmss,
std::vector<std::string> col_names);
