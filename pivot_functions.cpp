// Pivot Functions
// By Ken Burchfiel
// Released under the MIT License

#include "pivot_functions.h"

  std::pair<std::map<std::string, std::map<std::string, 
  std::variant<double, long>>>, std::vector<
  std::string>> vmss_pivot_table(
  std::vector<std::map<std::string, std::string>> const &vmss_data, 
  std::vector<std::string> const &index_fields,
    std::vector<std::string> const &value_fields, 
    char pivot_index_separator) {


    /* This function converts a vector of string/string maps into 
    a pivot table. It returns both this table and a vector of
    the table's header rows (which will be useful when exporting
    the data to a CSV file or another format).

    vmss_data: The vector of string/string maps that contains
    the data to be processed by this function.

    index_fields: The fields that should serve as index values
    for the pivot table. The function will calculate statistics
    for each unique group of these fields.

    value_fields: The fields containing numerical values (in string
    format) that this function should sum up and count.

    pivot_index_separator: The character that should be used to
    separate pivot-index values within the index-field column,
    thus making the output more readable. This character should
    *not* be present within any index fields' values.    

    */
  



  /* Determining the header row for the pivot table (which will prove
  useful for CSV exports): */

  std::vector<std::string> header_row = {};

  // Adding the index field to the header row:
  std::string index_name = "";
  for (int i = 0; i < index_fields.size(); i++) {
    index_name += (index_fields[i]);
    // We should add a separator after this field as long as it's
    // not the final entry.
    if (i != index_fields.size() - 1) {
      index_name += pivot_index_separator;
    }
  }

  header_row.push_back(index_name);

  // Adding the value fields to the header row:

  for (int i = 0; i < value_fields.size(); i++) {
    // String-based values will need to get converted to doubles before
    // they can be added to the pivot table.
    header_row.push_back(value_fields[i] + "_sum");
    header_row.push_back(value_fields[i] + "_count");
  }

  std::cout << "Header row: ";
  for (int i = 0; i < header_row.size(); i++) {
    std::cout << header_row[i] << " ";
  }
  std::cout << "\n";

  std::map<std::string, std::map<std::string, std::variant<double, long>>>
      pivot_table;

  for (int i = 0; i < vmss_data.size(); i++)

  {
    // Retrieving the current row of the dataset:
    std::map<std::string, std::string> pivot_row = vmss_data[i];

    // Combining index fields into a single string that can serve
    // as a dictionary key:
    std::string pivot_index = "";
    for (int j = 0; j < index_fields.size(); j++) {
      pivot_index += pivot_row[index_fields[j]];
      // We should add a separator after this field as long as it's
      // not the final entry.
      if (j != index_fields.size() - 1) {
        pivot_index += pivot_index_separator;
      }
    }
    // std::cout << "pivot_index: " << pivot_index << "\n";

    /* Checking whether this field is already present in our list of
    pivot-index keys: (If it isn't, we'll initialize a new value
    for it, which will take the form of a map.)

    NOTE: This check may not actually be necessary. See if you still
    get correct results if you skip it--which should save some processing
    time.*/

    if (pivot_table.contains(pivot_index) == false) {
      // std::cout << "Creating a new pivot_index." << "\n";
      std::map<std::string, std::variant<double, long>> value_map{};

      // For each value in value_fields, we'll add double entries that
      // will store sums, and long entries that will store counts.
      // (This approach won't work for all use cases, of course--but
      // I wanted to keep this function simple for now.)
      for (int j = 0; j < value_fields.size(); j++) {
        // String-based values will need to get converted to doubles before
        // they can be added to the pivot table.
        value_map[value_fields[j] + "_sum"] = stod(pivot_row[value_fields[j]]);
        value_map[value_fields[j] + "_count"] = 1;

        pivot_table[pivot_index] = value_map;

        // pivot_table[pivot_index][value_fields[j]+"_sum"] = std::get<long>(
        // pivot_table[pivot_index][value_fields[j]+"_sum"]) + 3;
      }
    }

    else {
      // std::cout << "Updating existing value for pivot_index." << "\n";
      // Updating the value of the current pivot-index key with
      // this row's values:

      for (int j = 0; j < value_fields.size(); j++) {
        pivot_table[pivot_index][value_fields[j] + "_sum"] =
            std::get<double>(
                pivot_table[pivot_index][value_fields[j] + "_sum"]) +
            stod(pivot_row[value_fields[j]]);
        pivot_table[pivot_index][value_fields[j] + "_count"] =
            std::get<long>(
                pivot_table[pivot_index][value_fields[j] + "_count"]) +
            1;
      }
    }
  }


    return  std::pair<std::map<std::string, std::map<std::string, 
  std::variant<double, long>>>, std::vector<
  std::string>> {pivot_table, header_row};
    }



    std::vector<std::map<std::string, 
    std::string>> pivot_table_to_vmss(std::map<std::string, 
    std::map<std::string, std::variant<double, long>>> const &pivot_table, 
    std::vector<std::string> const &header_row) {
        /* This function converts the output of vmss_pivot_table()
        to a vector of string/string maps, thus allowing it to be
        passed as input to the vmss_to_csv function within
        simple_csv_import_export.h. It takes two inputs:
        the pivot table to convert, and the header row for that table.
        (Both of these files can be retrieved from the output of
        vmss_to_csv.) 
        
        Note: Because this function does not make any modifications
        to header_row, it will not return it. (Instead, it can be 
        retrieved from whatever code is calling this function.)*/



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

      else if (std::holds_alternative<long>(row_value)) {
        mss[header_field] = std::to_string(std::get<long>(row_value));
      }

      // It would be ideal to raise an error here if neither of the previous
      // if statements is true.
    }

    pivot_table_vmss.push_back(mss);
  }
  
  return pivot_table_vmss;

}