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

Multi_Type_Table csv_to_mtt(std::string file_path,
                            std::map<std::string, std::string> field_type_map,
                            std::set<std::string> fields_to_import = {},
                            char delimiter = ',');

struct Pivot_Row {

  // This struct will hold pivot-table data for a given pivot index.
  // I'm using separate maps for double- and long-typed data,
  // though I could also have tried out a variant-based approach 
  // instead.

  std::map<std::string, double> df{};
  std::map<std::string, long> lf{};
};


Multi_Type_Table mtt_pivot_table(Multi_Type_Table mtt, 
std::vector<std::string> index_fields,
std::vector<std::string> double_value_fields = {},
std::vector<std::string> long_value_fields = {},
std::vector<std::string> int_value_fields = {},
char pivot_index_separator = '|',
bool calculate_means = false);

