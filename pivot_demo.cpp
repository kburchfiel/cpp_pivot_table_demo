/* C++ Pivot-Table Demo
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

To do:

1. Create a new csv_to_vvs function within 
mtt.cpp, then move your corresponding
code within this file into it.

2. Also convert your config-file-parsing code into a function, 
then move it to mtt.cpp as well. (It will be useful to keep
all functions used for MTT processing, even if they don't
directly involve MTTs, within the same header file.)

3. Update your csv_to_mtt table as follows: 

  a. Update your first-row script so that it stores all columns,
  rather than just those you want to add to your MTT, into a map
  of string/string pairs. (This will allow you to filter rows
  based on values that won't actually get added into your MTT.
  You should still only add fields into mtt.field_order that will
  actually get included within the MTT, though.)

  b. Have the function read, for each row, all field/value 
  pairs into a string/string map.
  
  c. Add maps of string/set pairs that will let you determine, based
  on whether variables within a given row include/do not include
  certain values, which rows to include. (You can base this code
  off the include/exclude maps within your pivot-configuration setup,
  except that you'll only need two (string-typed) values, since
  all of the data you're importing will originally be in string format.

  d. Use these maps to determine whether or not to include a particular
  row.
  
  c. For rows that *will* get included based on these filters:
  Use the string/string-map representation of the row's data
  to add items into your MTT. You can do so by iterating through
  all field name/valarray pairs within your MTT's df, lf, intf,
  and stf attributes, then adding the value in the row corresponding
  to a given field-name key into the valarray. (Make sure to cast it
  to that valarray's data type when necessary.)

  d. Keep track of how many rows you've actually added into the
  MTT. Next, at the end of the function, resize each valarray
  to that specific number. This will prevent any filter operations
  from resulting in extra rows at the end of each valarray.

  )

4. Add additional documentation, particularly for the configuration
file. (Explain that a JSON setup would make more sense, but that 
you wanted to avoid external headers for this project, both to
keep it simple and to better learn the principles underlying
those libraries.)

5. Add code to mtt_pivot_table() that uses vector division
with this table to calculate means. This may require adding
additional code that casts long and int values to doubles.

6. Also show how you can add custom fields to a given valarray
(such as the output of pivot_mtt). This would involve accessing
a certain set of config data (e.g. config_map[0]); running
pivot_mtt; adding additional fields within the script; then exporting
the results to a separate .csv file.)

7. Compare how long your code takes to run via your two different
approaches to storing pivot-table data
(i.e. as a struct of maps of valarrays and as a vector
of string/string maps.)

8. Share pivot-table functions/applications within StackOverflow
post that asked about using C++ for pivot tables (but mention its
limitations, together with better alternatives like
the DataFrame library and Vicent La's library)

9. Add in some equivalent Python code in order to (1) check your
output and (2) compare speeds. (Chances are the Python code will
be faster, given that Pandas uses Numpy under the hood)

10. Compile the code to assembly to see what the output looks like.

*/

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "mtt.h"
#include "pivot_functions.h"
#include "simple_csv_import_export.h"

int main() {

  // Testing out an import of configuration data:

  // This code is based in part on csv_to_vmss().

  // To do: Convert this into a function and add it to
  // simple_csv_import_export.cpp.

  std::vector<std::vector<std::string>> vvs;
  // vvs = 'vector of vectors of strings'

  char delimiter = ',';

  std::string config_file_path = "config.csv";

  std::ifstream stfs{config_file_path};

  for (std::string row; getline(stfs, row);) {
    std::vector<std::string> vs{}; // vs = 'vector of strings'
    std::istringstream current_row;
    current_row.str(row);
    for (std::string column; getline(current_row, column, delimiter);) {
      vs.push_back(column);
    }
    // Adding this vector (which contains data for a single
    // row) to our vector of vector of strings:
    vvs.push_back(vs);
  }

  std::cout << "Length of vvs:" << vvs.size() << "\n";

  for (int i = 0; i < vvs.size(); i++) {
    std::vector<std::string> vs = vvs[i];
    for (int j = 0; j < vs.size(); j++) {
      std::cout << vs[j] << "\t";
    }
    std::cout << "\n";
  }

  // Adding configuration info from vvs into our program:

  // Data-import variables:

  std::string data_file_path;
  std::map<std::string, std::string> field_type_map{};
  std::set<std::string> fields_to_import{};

// Defining a struct that can hold pivot-table-specific variables:
// (Adding these structs to a map will allow us to more easily
// create multiple pivot tables from the same data source.)

struct Pivot_Config {
bool calculate_means;
  std::vector<std::string> index_fields{};
  std::vector<std::string> double_value_fields{};
  std::vector<std::string> long_value_fields{};
  std::vector<std::string> int_value_fields{};
  std::string table_file_path {"pivot_file.csv"}; // This value should
  // be overridden within config.csv in order to prevent subsequent
  // pivot tables from overwriting previous ones.

  // The following eight maps specify values of various types 
  // to include or exclude from this pivot table. Map keys
  // are variable names, and map values are sets of values
  // to either include or exclude. (Using sets here, rather 
  // than vectors, should help speed up the code somewhat.)
  std::map<std::string,std::set<std::string>> string_vals_to_include {};
  std::map<std::string,std::set<std::string>> string_vals_to_exclude {};

    std::map<std::string,std::set<double>> double_vals_to_include {};
  std::map<std::string,std::set<double>> double_vals_to_exclude {};

    std::map<std::string,std::set<int>> int_vals_to_include {};
  std::map<std::string,std::set<int>> int_vals_to_exclude {};

    std::map<std::string,std::set<long>> long_vals_to_include {};
  std::map<std::string,std::set<long>> long_vals_to_exclude {};


};

// The following map will hold configuration data for one or more
// pivot tables. (Using integer-based keys lets us ensure that
// pivot tables are created in the numerical order specified
// by our key values.)

std::map<int, Pivot_Config> config_map {};

  
  for (int i = 1; i < vvs.size(); i++) { // The first row is a 
    // header row for users' reference, which we can skip here.

    std::vector<std::string> vs = vvs[i];
    // The first column of each configuration entry
    // will specify to which pivot-table configuration
    // (if any) a particular row applies; the second 
    // column will specify a variable for that configuration;
    // and subsequent columns will specify one or more values
    // for that variable.
    // Note that certain variables may have multiple entries
    // within the configuration file. This will allow
    // multiple items (e.g. multiple vector or map values)
    // to get added in for that file without making
    // the CSV file wider than necessary.

    if (vs[0] == "Import") // This row contains import-related data.


    {

    if (vs[1] == "data_file_path") // In this case, vs[0] will be
    // a configuration key, and vs[1] will be its corresponding
    // value. We can ignore any columns that follow vs[1].
    {
      data_file_path = vs[2];
    }


    else if (vs[1] == "field_type_map") {
    field_type_map[vs[2]] = vs[3];
  }

    else if (vs[1] == "fields_to_import") {
      fields_to_import.insert(vs[2]);
    }



    }

    else // This row contains data for a particular pivot table.
  {
    int pivot_index = std::stoi(vs[0]);
    if (config_map.contains(pivot_index) == false)
    {// Adding an entry for this particular pivot index:

      Pivot_Config pc {};
      config_map[pivot_index] = pc;
    }


    // Updating a particular pivot table's configuration info
    // with the data stored in this row:
    if (vs[1] == "calculate_means") {
      if (vs[2] == "Y") {
        config_map[pivot_index].calculate_means = true;
      } else {
        config_map[pivot_index].calculate_means = false;
      }
    }

    else if (vs[1] == "index_fields") {
      config_map[pivot_index].index_fields.push_back(vs[2]);
    }

    else if (vs[1] == "double_value_fields") {
      config_map[pivot_index].double_value_fields.push_back(vs[2]);
    }

    else if (vs[1] == "long_value_fields") {
      config_map[pivot_index].long_value_fields.push_back(vs[2]);
    }

    else if (vs[1] == "int_value_fields") {
      config_map[pivot_index].int_value_fields.push_back(vs[2]);
    }


      else if (vs[1] == "string_vals_to_include") {
      config_map[pivot_index].string_vals_to_include[vs[2]].insert(vs[3]);
    }

          else if (vs[1] == "string_vals_to_exclude") {
      config_map[pivot_index].string_vals_to_exclude[vs[2]].insert(vs[3]);
    }

      else if (vs[1] == "int_vals_to_include") {
      config_map[pivot_index].int_vals_to_include[vs[2]].insert(stoi(vs[3]));
    }

      else if (vs[1] == "int_vals_to_exclude") {
      config_map[pivot_index].int_vals_to_exclude[vs[2]].insert(stoi(vs[3]));
      }

      else if (vs[1] == "double_vals_to_include") {
      config_map[pivot_index].double_vals_to_include[vs[2]].insert(stod(vs[3]));
    }

      else if (vs[1] == "double_vals_to_exclude") {
      config_map[pivot_index].double_vals_to_exclude[vs[2]].insert(stod(vs[3]));
      }
  
      else if (vs[1] == "long_vals_to_include") {
      config_map[pivot_index].long_vals_to_include[vs[2]].insert(stol(vs[3]));
    }

      else if (vs[1] == "long_vals_to_exclude") {
      config_map[pivot_index].long_vals_to_exclude[vs[2]].insert(stol(vs[3]));
      }

    else if (vs[1] == "table_file_path") // In this case, vs[0] will be
    // a configuration key, and vs[1] will be its corresponding
    // value. We can ignore any columns that follow vs[1].
    {
      config_map[pivot_index].table_file_path = vs[2];
    }

  }
}

  std::cout << "Import configuration info:\n";
  std::cout << "Data file path:\t" << data_file_path << "\n";
  std::cout << "field_type_map:\t";

for (auto &[field_name, 
field_type] : field_type_map) {
std::cout << field_name << ": " << field_type << "\t";
}
std::cout << "\n";

  std::cout << "fields_to_import:\t" ;
for (std::string field: fields_to_import)
{std::cout << field << "\t";
}
std::cout << "\n";


  // Testing out an import of CSV data into a Multi_Type_Table:

  // Importing CSV data. Source:
  //   Air Carriers : T-100 Segment (All Carriers) table
  // from the Bureau of Transportation Statistics
  // https://www.transtats.bts.gov/DL_SelectFields.aspx?gnoyr_VQ=FMG&QO_
  // fu146_anzr=Nv4+Pn44vr45

  // Passengers should really be a long-type field as well,
  // but I'm making it a double field for testing purposes.
  Multi_Type_Table csv_mtt =
      csv_to_mtt(data_file_path, field_type_map, fields_to_import);

  char pivot_index_separator = '|';

  // std::cout << "Checkpoint A.";
  // std::cout << "Checkpoint A2.";
  std::cout << "Length of csv_mtt:" << csv_mtt.length << "\n";

  std::cout << "Sample contents of csv_mtt:\n";

  print_mtt(csv_mtt, 10);



// Calling pivot_mtt for each set of pivot-table configuration
// values specified within config_map:



for (auto &[pivot_index, pc]: config_map)


{
std::cout << "Creating pivot table #" << pivot_index << ".\n";



std::cout << "calculate_means:\t" << pc.calculate_means << "\n";

// Range for loop:
  std::cout << "index_fields:\t" ;
for (std::string field: pc.index_fields)
{std::cout << field << "\t";
}

// Older method:
std::cout << "\n";
  std::cout << "double_value_fields:\t" ;
for (int i = 0; i < pc.double_value_fields.size(); i++)
{std::cout << pc.double_value_fields[i] << "\t";
}
std::cout << "\n";

// You also don't need to specify the value type within 
// range for loops:
  std::cout << "long_value_fields:\t" ;
for (auto field: pc.long_value_fields)
{std::cout << field << "\t";
}
std::cout << "\n";
  std::cout << "int_value_fields:\t" ;

  for (auto field: pc.int_value_fields)
{std::cout << field << "\t";
}
std::cout << "\n";

std::cout << "Table file path:\t" << pc.table_file_path << "\n";


  Multi_Type_Table pivot_mtt =
      mtt_pivot_table(csv_mtt, pc.index_fields, 
        pc.double_value_fields,
        pc.long_value_fields, 
        pc.int_value_fields,
      '|', // We could also update the code to allow the user
      // to select this option.
      pc.calculate_means,
    pc.string_vals_to_include,
  pc.string_vals_to_exclude,
pc.int_vals_to_include,
pc.int_vals_to_exclude,
pc.double_vals_to_include,
pc.double_vals_to_exclude,
pc.long_vals_to_include,
pc.long_vals_to_exclude);
      // );

  // Testing out the addition of a new field:
  // (This sort of operation is much easier to do with
  // valarrays than with vectors, as it eliminates
  // the need to specify a loop.)

  // std::valarray<double> test_sum =
  //     (pivot_mtt.df["PASSENGERS_sum"] + pivot_mtt.df["PASSENGERS_sum"]);

  // // Adding this field to our list of fields so that it can
  // // get included within printouts and CSV exports:

  // pivot_mtt.df["2x_PASSENGERS"] = test_sum;

  // pivot_mtt.field_order.push_back("2x_PASSENGERS");

  std::cout << "\nSample pivot_mtt content:\n\n";

  print_mtt(pivot_mtt, 10);

  std::cout << "Exporting pivot_mtt to a CSV file.\n";

  mtt_to_csv(pivot_mtt, pc.table_file_path);

  std::cout << "Finished pivot_mtt export for table #" << pivot_index << ".\n";


}

  // Part 2: Alternative set of code that stores data as
  // vectors of string/string maps

  // std::cout << "Returning 0.\n";
  // return 0;

  std::pair<std::vector<std::map<std::string, std::string>>,
            std::vector<std::string>> const csv_data_and_header =
      csv_to_vmss(
          // The following, commented-out file is useful for testing
          // and debugging purposes, as it's much smaller (and thus won't
          // take as long to process).
          "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols_truncated.csv"
          // "T_T100_SEGMENT_ALL_CARRIER_2024_selected_cols.csv"
      );

  std::vector<std::map<std::string, std::string>> const csv_data =
      csv_data_and_header.first;
  std::vector<std::string> const col_names = csv_data_and_header.second;

  std::cout << "Length of csv_data: " << csv_data.size() << "\n";

  // Creating a pivot table:

  Pivot_Config pc = config_map[0];

  std::vector<std::string> value_fields{"PASSENGERS", "FREIGHT"};

  std::pair<
      std::map<std::string, std::map<std::string, std::variant<double, long>>>,
      std::vector<std::string>> const pivot_and_header =
      vmss_pivot_table(csv_data, pc.index_fields, value_fields);

  std::map<std::string, std::map<std::string, std::variant<double, long>>>
      pivot_table_from_vmss = pivot_and_header.first;

  std::vector<std::string> header_row = pivot_and_header.second;

  std::cout << "Number of keys within pivot_table_vmss: "
            << pivot_table_from_vmss.size() << "\n";

  // Adding in mean columns now that sums and counts have been calculated:
  // (vmss_pivot_table could also be updated to perform this task,
  // but I wanted to demonstrate how the output of the pivot-table
  // function can be further updated if needed.)

  // Iterating through all keys of the pivot table:
  // (This code is based on p. 580 of PPAP3 and
  //  https://stackoverflow.com/a/26282004/13097194)

  for (auto &[pivot_index, value_map] : pivot_table_from_vmss) {

    for (auto &[value_key, value] : value_map) {

      for (int i = 0; i < value_fields.size(); i++) {
        value_map[value_fields[i] + "_mean"] =
            std::get<double>(value_map[value_fields[i] + "_sum"]) /
            std::get<long>(value_map[value_fields[i] + "_count"]);
      }
    }
  }

  // We'll also update our header-column variable with these new
  // values:

  for (int i = 0; i < value_fields.size(); i++) {
    header_row.push_back(value_fields[i] + "_mean");
  }

  // Exporting the pivot table: (I'll do so by first converting the
  // pivot-table data into a vector of string/string maps, which is what my
  // CSV-export function accepts as input, together with a list of header
  // fields.)

  // (Alternatively, I could have created a CSV-export function that
  // would accept a vector of vectors of strings, which would work fine
  // here also provided that I make sure to add values to each vector
  // in the correct order.)

  std::vector<std::map<std::string, std::string>> pivot_table_vmss =
      pivot_table_to_vmss(pivot_table_from_vmss, header_row);

  vmss_to_csv("pivot_table_from_vmss.csv", pivot_table_vmss, header_row);
}
