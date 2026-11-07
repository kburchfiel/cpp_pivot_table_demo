// Pivot Functions
// By Ken Burchfiel
// Released under the MIT License

#include <iostream>
#include <map>
#include <variant>
#include <vector>

  std::pair<std::map<std::string, std::map<std::string, 
  std::variant<double, long>>>, std::vector<
  std::string>> vmss_pivot_table(
  std::vector<std::map<std::string, std::string>> const &vmss_data, 
  std::vector<std::string> const &index_fields,
    std::vector<std::string> const &value_fields, 
                 char pivot_index_separator = '|');

std::vector<std::map<std::string, std::string>> pivot_table_to_vmss(std::map<std::string, 
    std::map<std::string, std::variant<double, long>>> const &pivot_table, 
    std::vector<std::string> const &header_row);

