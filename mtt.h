/* mtt.h: Header files for Multi_Type_Table struct and 
corresponding operations
By Ken Burchfiel
Released under the MIT license */

#include <map>
#include <vector>
#include <string>
#include <set>
#include <valarray>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>


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

  long length = 0; // This should probably be made
  // private so that users can't mess with it!

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

struct Pivot_Row {

  // This struct will hold pivot-table data for a given pivot index.
  // I'm using separate maps for double- and long-typed data,
  // though I could also have tried out a variant-based approach 
  // instead.

  std::map<std::string, double> df{};
  std::map<std::string, long> lf{};
};


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




Multi_Type_Table csv_to_mtt(std::string file_path,
                            std::map<std::string, std::string> field_type_map,
                            std::set<std::string> fields_to_import = {},
                            char delimiter = ',',
    std::map<std::string, std::set<std::string>> csv_vals_to_include = {},
    std::map<std::string, std::set<std::string>> csv_vals_to_exclude = {});



Multi_Type_Table mtt_pivot_table(Multi_Type_Table &mtt, 
std::vector<std::string> const index_fields,
std::vector<std::string> const double_value_fields = {},
std::vector<std::string> const long_value_fields = {},
std::vector<std::string> const int_value_fields = {},
char pivot_index_separator = '|',
bool calculate_means = false,
    std::map<std::string, std::set<std::string>> string_vals_to_include = {},
    std::map<std::string, std::set<std::string>> string_vals_to_exclude = {},

    std::map<std::string, std::set<int>> int_vals_to_include = {},
    std::map<std::string, std::set<int>> int_vals_to_exclude = {},

    std::map<std::string, std::set<double>> double_vals_to_include = {},
    std::map<std::string, std::set<double>> double_vals_to_exclude = {},

    std::map<std::string, std::set<long>> long_vals_to_include = {},
    std::map<std::string, std::set<long>> long_vals_to_exclude = {});

void print_mtt(Multi_Type_Table mtt, 
long n = 0, char print_separator = '\t');

void mtt_to_csv(Multi_Type_Table &mtt, std::string file_path, 
char delimiter = ',');


std::vector<std::vector<std::string>> csv_to_vvs(
std::string csv_path);

void print_vvs(std::vector<std::vector<std::string>> &vvs);

void print_pivot_config_data(Pivot_Config& pc);