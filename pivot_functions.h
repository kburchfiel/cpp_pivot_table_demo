// Pivot Functions
// By Ken Burchfiel
// Released under the MIT License

#include <iostream>
#include <vector>
#include <map>
#include <variant>

  std::pair<std::map<std::string, std::map<std::string, 
  std::variant<double, long>>>, std::vector<
  std::string>> vmss_pivot_table(
  std::vector<std::map<std::string, std::string>> vmss_data, 
  std::vector<std::string> index_fields,
    std::vector<std::string> value_fields, 
    char pivot_index_separator = '|');