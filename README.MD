# C++ Pivot Table Demo

By Ken Burchfiel

Released under the MIT License

This project, which is a rough work in progress and **not** meant for actual production use, demonstrates how C++ can be used to create pivot tables of data stored in CSV files. The code allows for the creation of pivot tables with an arbitrary number of both index and value fields, and also allows certain rows to get filtered out from both the CSV import and individual pivot tables. However, it currently supports a limited number of data types (i.e. integers, longs, doubles, and strings); in addition, it only supports sum, count, and mean operations.

## Overview of code files

I plan to add more documentation on these functions in the future. In the meantime, the relevant code files to check out are:

1. mtt.h and mtt.cpp (which contain a number of functions for importing CSV data; converting that data into pivot tables; and then exporting that data to a CSV. They make use of a 'Multi_Type_Table' struct that stores data as a set of maps of string/valarray pairs; the strings represent field names, and the values represent the CSV data for that particular field.)
2. pivot_table_creation.cpp (which shows how to apply these functions to create a set of pivot tables)
3. config.csv (which specifies what CSV data to import and what parameters to use when creating pivot tables)

You can also review simple_csv_import_export.cpp, which shows how to import CSV data into a vector of string/string pairs, and pivot_functions.cpp, which shows how to convert this
data into a pivot table as well. (Because these functions don't make use of valarray-based
data, they may result in slower performance depending on what types of operations you're performing; therefore, I would suggest using mtt.cpp's functions instead.)

This script makes use of data from the Air Carriers : T-100 Segment (All Carriers) table
  from the Bureau of Transportation Statistics. Link:
  https://www.transtats.bts.gov/DL_SelectFields.aspx?gnoyr_VQ=FMG&QO_fu146_anzr=Nv4+Pn44vr45

## Applying this code in your own work

**DISCLAIMER: This code has not been tested thoroughly and may have a number of bugs or glitches. In addition, it may fail to work correctly when missing data are present within your CSV file. Use at your own risk and check its output!**

To make use of this code for your own work:

1. copy pivot_table_creation, mtt.h, and mtt.cpp into a separate folder. (If you'd like to store your own pivot tables within a Pivot_Tables folder, as this repository does, make sure to create an empty folder with that name within this folder as well.)

2. create a copy of config.csv that matches your own pivot-table data. (More documentation on this file is definitely needed, but the example file in this repository, together with mtt.cpp's import_config_data, can give you some hints.) 

3. compile the code (perhaps using this repository's own compile.sh file as a reference) and run the program. 

### To-dos

1. Create a new csv_to_vvs function within
mtt.cpp, then move your corresponding
code within this file into it.

2. Also convert your config-file-parsing code into a function,
then move it to mtt.cpp as well. (It will be useful to keep
all functions used for MTT processing, even if they don't
directly involve MTTs, within the same header file.) In order to
return all the configuration data parsed by the function, you'll
probably want to define a struct that can store config data. 
You'd then return both your CSV-import struct and your map of
int/Pivot_Config values within a tuple or std::pair.

3. Move additional structs and functions from pivot_demo.cpp
into mtt.cpp as needed, thus reducing the amount of code you need
to include within each use of mtt.cpp.

4. Move your non-MTT code at the end into a separate .cpp file
that will also show how to modify/create new valarrays within
an existing MTT.

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
a certain set of config data (e.g. pcm[0]); running
pivot_mtt; adding additional fields within the script; then exporting
the results to a separate .csv file.)

7. Try out a map of string/valarray-variant pairs as an alternative
to a Multi-Type Table. Your variants could include int, string,
double, and long valarrays. It would help to have both a vector
that specifies your preferred column order *and* a string/set map
whose keys are type names and whose values are sets of all of the fields
that use that type. (This should cut down on the number of times that
you'd need to check what kind of type a particular valarray contains,
as when you iterate through these maps, you'd know which type you're
working with and thus wouldn't have to try out different options.)

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