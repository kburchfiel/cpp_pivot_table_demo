/* mvvt.h: Header files for analyzing data stored
within a map of variants of valarrays
corresponding operations
By Ken Burchfiel
Released under the MIT license 

Much of this code is based on, or directly copied from,
mtt.h.*/

#include <map>
#include <vector>
#include <string>
#include <set>
#include <valarray>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <valarray>
#include <variant>


struct Import_Config {

// This struct holds relevant values for importing a CSV file into
// a Multi_Type_Table (and likely other data formats as well).

  std::string data_file_path;
  std::map<std::string, std::string> field_type_map{};
  std::set<std::string> fields_to_import{};
  char delimiter = ',';

    // The following two maps specify values
    // to include or exclude when importing CSV data. Map keys
    // are variable names, and map values are sets of values
    // to either include or exclude. (Using sets here, rather
    // than vectors, should help speed up the code somewhat.)

  std::map<std::string, std::set<std::string>> csv_vals_to_include{};
  std::map<std::string, std::set<std::string>> csv_vals_to_exclude{};
};



  struct Pivot_Config {


  // This struct holds pivot-table-specific configuration variables.

    bool calculate_means;
    std::vector<std::string> index_fields{};
    std::vector<std::string> double_value_fields{};
    std::vector<std::string> long_value_fields{};
    std::vector<std::string> int_value_fields{};
    std::string table_file_path{"pivot_file.csv"}; // This value should
    // be overridden within config.csv in order to prevent subsequent
    // pivot tables from overwriting previous ones.

    // The following eight maps specify values of various types
    // to include or exclude from this pivot table. Map keys
    // are variable names, and map values are sets of values
    // to either include or exclude.
    std::map<std::string, std::set<std::string>> string_vals_to_include{};
    std::map<std::string, std::set<std::string>> string_vals_to_exclude{};

    std::map<std::string, std::set<double>> double_vals_to_include{};
    std::map<std::string, std::set<double>> double_vals_to_exclude{};

    std::map<std::string, std::set<int>> int_vals_to_include{};
    std::map<std::string, std::set<int>> int_vals_to_exclude{};

    std::map<std::string, std::set<long>> long_vals_to_include{};
    std::map<std::string, std::set<long>> long_vals_to_exclude{};
  };

  std::pair<Import_Config, std::map<int, Pivot_Config>> import_config_data(
  std::vector<std::vector<std::string>> & vvs);


std::vector<std::vector<std::string>> csv_to_vvs(
std::string csv_path);

void print_vvs(std::vector<std::vector<std::string>> &vvs);

void print_pivot_config_data(Pivot_Config& pc);


struct MVV_Table {
/* MVV stands for 'Map of Variant of Valarrays,' the data type 
that we'll use to store our table data. */

std::map<std::string, std::variant<std::valarray<std::string>,
std::valarray<double>, std::valarray<long>,
std::valarray<int>>> mvv {};

std::map<std::string, std::set<std::string>> tm { {"string", {}},
{"int", {}}, {"long", {}}, {"double", {}}};
// tm stands for 'type map.' This attribute stores type names
// as strings and valarray field names as values. This will help
// reduce the amount of 'guessing' we need to do to figure out which
// type a given valarray uses.

// The following vector of fields allows us to keep track
  // of the spreadsheet's column order (which can help export
  // data with the same field order that its source file used.)

  std::vector<std::string> field_order{};

long length = 0; // The length of each valarray.
};


MVV_Table csv_to_mvvt(std::string file_path,
                            std::map<std::string, std::string> field_type_map,
                            std::set<std::string> fields_to_import = {},
                            char delimiter = ',',
    std::map<std::string, std::set<std::string>> csv_vals_to_include = {},
    std::map<std::string, std::set<std::string>> csv_vals_to_exclude = {});


    void print_mvvt(MVV_Table mvvt, 
long n = 0, char print_separator = '\t');


