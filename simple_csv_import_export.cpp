/* Simple CSV Import/Export:
Functions for importing CSV files into memory, then exporting them back to a
.csv file By Ken Burchfiel Released under the MIT License The import component
of this script is based on code from my CPP Manual Import Test repository at
https://github.com/kburchfiel/cpp_manual_import_test (which is also released
under the MIT License.)

Potential future updates:
1. Try out a csv_to_vvs (CSV to vector of vectors of strings) in which
each row is stored as a vector of strings. You could still map header
values to row indices by returning a separate map that has field names
as keys and column indices as values. (This map could be used 
as follows: std::string data_point = vvs[i][field_index_map[
'Field_to_Retrieve']]. If 'Field_to_Retrieve' was at index 4 in the
original CSV file, this code would simplify to vvs[i][4].)

2. Test out a custom sort order for the map that preserves the original
field order. (See https://stackoverflow.com/questions/15588428/how-can-i-declare-a-custom-sort-function-on-stdmap-declaration
for reference; you could create a map that assigns different numerical codes to each field,
then sorts them based on their code.) This would eliminate the need to return
both a map and a vector of column names--and should also ensure that,
when map keys are printed to .csv files, their original field order will be preserved.
*/




#include "simple_csv_import_export.h"



std::pair<std::vector<std::map<std::string, std::string>>, std::vector<std::string>>
csv_to_vmss(std::string file_path, char delimiter) {
/* vmss stands for 'Vector of maps of strings & strings.
For .csv files, the default ',' delimiter should work fine
*unless* there are commas within the cell values themselves (in which case
you'll need to use a more capable library, such as
Vincent La's CSV parser--available at
https://github.com/vincentlaucsb/csv-parser .)
Meanwhile, for .tsv (tab-separated) files, a delimiter of '\t' should work
well (as long, again, as there are no tabs within your cell values.)
*/

  std::ifstream stfs{file_path};
  // if(!stfs) throw("Failed to open input file.");

  int row_index = 0;

  // The following vector of maps will store all rows of our .csv file
  // (aside from the header row).

  std::vector<std::map<std::string, std::string>> mapped_rowvec;
  std::vector<std::string>
      col_names; // This vector will store
                 // column names so that we can then add those names to maps
                 // that will get created later on.

  // In order to add values into our vector of maps, we'll first
  // use getline() to access each row of the .csv file; next,
  // we'll use getline() to access each column within each row.

  for (std::string row; getline(stfs, row);) {
    if (row_index == 0) // We'll want to treat the first row differently,
    // as we're assuming that it includes column names rather than
    // values.
    {
      std::istringstream first_row;
      first_row.str(row); // Based on:
      // https://en.cppreference.com/w/cpp/string/basic_string/getline

      // Using getline() with a tab delimiter argument in order
      // to go through each line within our istringstream (i.e.
      // the first row) and then extract all field names from it:
      // (See https://en.cppreference.com/w/cpp/string/basic_string/getline
      // for more information.)
      for (std::string column; getline(first_row, column, delimiter);) {
        // Adding this column to our std::vector of columns:
        col_names.push_back(column);
      }
    }

    else { // In this case, the current row will be considered to
      // contain values rather than headers.

      // This section of the code will store all rows as vectors of maps.
      // (This will allow for columns to be labeled, and also doesn't require
      // that we know the number of columns ahead of time.
      // All map values will be initialized as strings, so in order to
      // work with numbers or date/time values, those values' corresponding
      // columns will need to get converted to the proper type.

      // Initializing the current row as a new istringstream:
      std::istringstream current_row;
      current_row.str(row);

      // Creating a field_index integer that will help us keep track
      // of which column we're accessing--*and* will allow us to link
      // each column's value with its corresponding field name:
      int field_index = 0;

      // Creating a map of std::strings that can store labeled column values:
      std::map<std::string, std::string> mapped_row;

      // Adding each column to our map:
      // The key of this map entry will be the corresponding
      // field name (which can be retrieved from col_names using
      // the current field_index position); its value will be
      // the value of the string located at the current field_index
      // position.

      // Note that, in this case, getline() is being used to split
      // data within a single line (e.g. the current row) into
      // a series of delimiter-separated values. This saves us the trouble
      // of manually figuring out where one field ends and another begins
      // (as fields may contain whitespace-separated words).
      // This approach is based on the example code found at
      // https://en.cppreference.com/w/cpp/string/basic_string/getline.

      for (std::string column; getline(current_row, column, delimiter);) {
        mapped_row[col_names[field_index]] = column;
        field_index++; // Incrementing the index so that we can then
                       // move on to the next column
      }
      // Adding this map to our vector of maps:
      mapped_rowvec.push_back(mapped_row);
    }
    row_index++;
  }

return std::pair<std::vector<std::map<std::string, std::string>>, 
std::vector<std::string>> {mapped_rowvec, col_names};
}

void vmss_to_csv(std::string file_path,
std::vector<std::map<std::string, std::string>> vmss,
std::vector<std::string> col_names)
{
/* col_names, which will often get initialized from
the return value of csv_to_vmss (and thus won't need to be
created manually), allows us to control the order in which
columns are saved. If we simply iterated through each set of keys
and values, the fields would be saved in alphabetical order, which
often won't match our desired column order.
*/
// The following line is based on
    // https://en.cppreference.com/cpp/io/basic_ofstream .
std::ofstream ofs(file_path, std::ios::out);

// Writing the header row:

std::string output_string = "";
  for (int i = 0; i < col_names.size(); i++) // Iterating though
  // each field value for the current row
      {
        output_string += col_names[i];
    /* If we're not at the final entry within the list,
    we should add a comma; otherwise, we should add a newline character. */
    if (i == col_names.size() - 1) {
      output_string += "\n";
    } else {
      output_string += ",";
    }
  }
  ofs << output_string;

// Writing the data to the .csv file:
for (int i = 0; i < vmss.size(); i++) // Iterating through each row
{
  std::map<std::string, std::string> current_map = vmss[i];
  std::string output_string = "";
  for (int i = 0; i < col_names.size(); i++) // Iterating though
  // each field value for the current row
      {
        output_string += current_map[col_names[i]];
    /* If we're not at the final entry within the list,
    we should add a comma; otherwise, we should add a newline character. */
    if (i == col_names.size() - 1) {
      output_string += "\n";
    } else {
      output_string += ",";
    }
  }
  ofs << output_string;
}
ofs.close();
}
