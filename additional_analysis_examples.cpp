// Additional analysis examples
// By Ken Burchfiel
// Released under the MIT license
// This script will show a few alternative analyses, including:
// (I've commented out the code for now because it will require
// some additional updates before it can compile.)

// 1. Updating a Multi_Type_Table outside of the standard functions 
// within mtt.cpp
// 2. Using the functions in simple_csv_import.cpp
// and pivot_functions.cpp to create pivot tables out of 
// string-based data


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

/*

 Alternative set of code that stores data as
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

  Pivot_Config pc = pcm[0];

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

  */