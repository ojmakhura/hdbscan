# HDBSCAN
This is an implementation of the hdbscan algorithm RICARDO J. G. B. CAMPELLO et al. This is a translation from Java to C/C++. The build uses cmake to compile. You will need the math library and the glib2 installed.

## Compile
To install the library at /usr/local:
1. git clone https://github.com/ojmakhura/hdbscan.git
2. cd hdbscan
3. mkdir build
4. cd build
5. cmake ..
6. make
7. make install

This will build C, C++, Java and Python 2 and Python 3 libraries. To run PyHdbscan, make sure the python libraries are in the proper python path.

## CMake Options
This implementation is designed to allow for optimisation using data types. Since the big part of HDBSCAN is memor requirements, we have included 3 options to control the types of data that are used. The options are for distances, indices and labels. The types are writen into the config.h files that gets generated and are used during compilation.

### DISTANCE_TYPE
This option allows for either using doubles or floats for storing distance calculations. By default this option is set to ```double```, but can be changed to ```float```. The difference is that the memory requirement for storing distances is reduced by half.

### INDEX_TYPE
This option is related to the amount of data in the dataset. It is used to index into the arrays. The indices are also used in the MST creation. The options are ```unsigned int, unsigned long, unsigned short```

### LABEL_TYPE
The data type of label used. The options are ```unsigned int, unsigned long, unsigned short```. This is mostly used in cluster structures.

The library comes with 2 tester files in the samples folder. The file tester.c shows how to use the library in a C implementation. THe file tester.cpp shows how to use the library with a C++ implementation. The implementation has bee designed to work with a dataset of 5 different types of data, float, double, int, long and short.

To let the algorithm know which datatype you are working with, you pass the H\_{FLOAT, DOUBLE, INT, SHORT, FLOAT} as a parameter to the run() method. Python code uses numpy and will determine the datatypes from the numpy array. The code expectes a 1 or two dimensional dataset.

