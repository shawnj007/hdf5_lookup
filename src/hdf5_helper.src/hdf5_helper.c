/*
 *	Program: hdf5 helper v0.1
 *
 *	Programmer: Shawn L. Jaker
 *
 *	Usage: This code is free to use and modify, so long as this header remains intact, except for "Modifications" below.
 *
 *	Purpose: Extract ids, dimensions, type size, and data using only
 * 				filename
 *				group name
 *				variable name
 *
 *	COMPILE COMMANDS (for integration in other projects):
 * 		cc -g -c ./src/dimalloc3.c -o ./obj/dimalloc3.o -Wall
 * 		cc -g -c ./src/hdf5_helper.c -o ./obj/hdf5_helper.o -lhdf5 -Wall
 *
 *	COMPILE COMMAND (for standalone tester, after uncommenting main()):
 *		cc -g -c ./src/dimalloc3.c -o ./obj/dimalloc3.o -Wall
 * 		cc -g ./src/hdf5_helper.c ./obj/dimalloc3.o -o hdf5_helper -lhdf5 -Wall
 *
 *		...> Inlcude generated objects when compiling source where used (append to compile command)
 *
 *	Functions
 *		void* 		get_variable_dims_by_name			- wrapper for internal function, below
 *		hid_t	 	get_variable_type_by_name			- wrapper for internal function, below
 *		void* 		get_variable_data_by_name			- returns a void* to 1-d data
 *		void* 		get_variable_data_by_name_dimalloc2	- returns a void* to 2-d data, cast as (type**),  reference as array[row][col]
 *		void* 		get_variable_data_by_name_dimalloc3	- returns a void* to 3-d data, cast as (type***), reference as array[lev][row][col]
 *
 *	Internal functions
 *		void* 		get_variable_ids_by_name 			- returns a 3 element int array containing { ncid, grp_ncid, varid }
 *		void* 		get_variable_dims					- returns a 3 element size_t array containing { levs, rows, cols }
 *		hid_t	 	get_variable_type					- returns an enumeration value
 *		ssize_t		get_type_size						- helper for get size
 *		
 *	Utility functions
 *		void*		convert_2d_to_1d					- data conversion 2D --> 1D
 *		void*		convert_1d_to_2d					- data conversion 1D --> 2D
 *		void*		convert_3d_to_1d					- data conversion 3D --> 1D
 *		void*		convert_1d_to_3d					- data conversion 1D --> 3D
 *
 *	Function Parameters
 *		const char* path	pointer to string array containing absolute file path
 *		const char* group	pointer to string array containing group name (in file)
 *		const char* name	pointer to string array containing variable name (in group)
 *
 *	Constants
 *		DEBUG_HDF5_HELPER 		- 1/0 - display DEBUG_HDF5_HELPER information while executing (default 0)
 *		MAX_DIMS 	- 1-3 - Don't change this. (4+)D needs more routines as above TODO?
 *
 *	Modifications:
 *		2021-04-29		v 0.1	(SLJ)		Original VIIRS Version 
 *		2021-05			v 0.1	(SLJ)		Modified for HDF5 files
 *		2021-05-14				(SLJ)		Modified to use local HDF5 source codes
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "/usr/include/hdf5/serial/hdf5.h"
#include "../../../hdf5-1.12.0/src/hdf5.h"
#include "dimalloc3.c"

#define DEBUG_HDF5_HELPER 0

#define TEST 1
#define MAX_DIMS 3

void* dimalloc(size_t size, size_t dimensions, ...);

void handle_error(const char* err_name, int err_code) {
	printf("Error: %s = %d\n", err_name, err_code);
	exit(1);
}

void* convert_2d_to_1d(void **data_in, hid_t type, size_t rows, size_t cols) {

	char				*data_char;
	signed char			*data_schar;
	unsigned char		*data_uchar;
	short				*data_short;
	unsigned short		*data_ushort;
	long				*data_int;
	unsigned			*data_unsigned;
	long				*data_long;
	unsigned long		*data_ulong;
	long long			*data_llong;
	unsigned long long	*data_ullong;
	float				*data_float;
	double				*data_double;
	long double			*data_ldouble;
	hsize_t				*data_hsize_t;
	hssize_t			*data_hssize_t;
	herr_t				*data_herr_t;
	hbool_t				*data_hbool_t;
	
	char				**data2d_char;
	signed char			**data2d_schar;
	unsigned char		**data2d_uchar;
	short				**data2d_short;
	unsigned short		**data2d_ushort;
	long				**data2d_int;
	unsigned			**data2d_unsigned;
	long				**data2d_long;
	unsigned long		**data2d_ulong;
	long long			**data2d_llong;
	unsigned long long	**data2d_ullong;
	float				**data2d_float;
	double				**data2d_double;
	long double			**data2d_ldouble;
	hsize_t				**data2d_hsize_t;
	hssize_t			**data2d_hssize_t;
	herr_t				**data2d_herr_t;
	hbool_t				**data2d_hbool_t;
	
	if (type == H5T_NATIVE_CHAR)  	data_char = malloc(sizeof(char) * rows * cols);
	if (type == H5T_NATIVE_SCHAR)	data_schar = malloc(sizeof(signed char) * rows * cols);
	if (type == H5T_NATIVE_UCHAR) 	data_uchar = malloc(sizeof(unsigned char) * rows * cols);
	if (type == H5T_NATIVE_SHORT) 	data_short = malloc(sizeof(short) * rows * cols);
	if (type == H5T_NATIVE_USHORT) 	data_ushort = malloc(sizeof(unsigned short) * rows * cols);
	if (type == H5T_NATIVE_INT) 	data_int = malloc(sizeof(long) * rows * cols);
	if (type == H5T_INTEGER)	 	data_int = malloc(sizeof(long) * rows * cols);
	if (type == H5T_NATIVE_UINT) 	data_unsigned = malloc(sizeof(unsigned) * rows * cols);
	if (type == H5T_NATIVE_LONG) 	data_long = malloc(sizeof(long) * rows * cols);
	if (type == H5T_NATIVE_ULONG) 	data_ulong = malloc(sizeof(unsigned long) * rows * cols);
	if (type == H5T_NATIVE_LLONG) 	data_llong = malloc(sizeof(long long) * rows * cols);
	if (type == H5T_NATIVE_ULLONG) 	data_ullong = malloc(sizeof(unsigned long long) * rows * cols);
	if (type == H5T_NATIVE_FLOAT) 	data_float = malloc(sizeof(float) * rows * cols);
	if (type == H5T_FLOAT)			data_float = malloc(sizeof(float) * rows * cols);
	if (type == H5T_NATIVE_DOUBLE) 	data_double = malloc(sizeof(double) * rows * cols);
	if (type == H5T_NATIVE_LDOUBLE) data_ldouble = malloc(sizeof(long double) * rows * cols);
	if (type == H5T_NATIVE_HSIZE) 	data_hsize_t = malloc(sizeof(hsize_t) * rows * cols);
	if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t = malloc(sizeof(hssize_t) * rows * cols);
	if (type == H5T_NATIVE_HERR) 	data_herr_t = malloc(sizeof(herr_t) * rows * cols);
	if (type == H5T_NATIVE_HBOOL) 	data_hbool_t = malloc(sizeof(hbool_t) * rows * cols);
	
	if (type == H5T_NATIVE_CHAR)  	data2d_char = (char**) data_in;
	if (type == H5T_NATIVE_SCHAR) 	data2d_schar = (signed char**) data_in;
	if (type == H5T_NATIVE_UCHAR) 	data2d_uchar = (unsigned char**) data_in;
	if (type == H5T_NATIVE_SHORT) 	data2d_short = (short**) data_in;
	if (type == H5T_NATIVE_USHORT) 	data2d_ushort = (unsigned short**) data_in;
	if (type == H5T_NATIVE_INT) 	data2d_int = (long**) data_in;
	if (type == H5T_INTEGER)	 	data2d_int = (long**) data_in;
	if (type == H5T_NATIVE_UINT) 	data2d_unsigned = (unsigned**) data_in;
	if (type == H5T_NATIVE_LONG) 	data2d_long = (long**) data_in;
	if (type == H5T_NATIVE_ULONG) 	data2d_ulong = (unsigned long**) data_in;
	if (type == H5T_NATIVE_LLONG) 	data2d_llong = (long long**) data_in;
	if (type == H5T_NATIVE_ULLONG) 	data2d_ullong = (unsigned long long**) data_in;
	if (type == H5T_NATIVE_FLOAT) 	data2d_float = (float**) data_in;
	if (type == H5T_FLOAT)			data2d_float = (float**) data_in;
	if (type == H5T_NATIVE_DOUBLE) 	data2d_double = (double**) data_in;
	if (type == H5T_NATIVE_LDOUBLE) data2d_ldouble = (long double**) data_in;
	if (type == H5T_NATIVE_HSIZE) 	data2d_hsize_t = (hsize_t**) data_in;
	if (type == H5T_NATIVE_HSSIZE) 	data2d_hssize_t = (hssize_t**) data_in;
	if (type == H5T_NATIVE_HERR) 	data2d_herr_t = (herr_t**) data_in;
	if (type == H5T_NATIVE_HBOOL) 	data2d_hbool_t = (hbool_t**) data_in;
	
	if (DEBUG_HDF5_HELPER) printf("new data shape is: r %4lu c %4lu\n", rows, cols);
	
	size_t col, row;
	for (row = 0; row < rows; row++) {
		for (col = 0; col < cols; col++) {
			size_t index = (row * cols) + col;
	
			if (type == H5T_NATIVE_CHAR)  	data_char[index] = data2d_char[row][col];
			if (type == H5T_NATIVE_SCHAR) 	data_schar[index] = data2d_schar[row][col];
			if (type == H5T_NATIVE_UCHAR) 	data_uchar[index] = data2d_uchar[row][col];
			if (type == H5T_NATIVE_SHORT) 	data_short[index] = data2d_short[row][col];
			if (type == H5T_NATIVE_USHORT) 	data_ushort[index] = data2d_ushort[row][col];
			if (type == H5T_NATIVE_INT) 	data_int[index] = data2d_int[row][col];
			if (type == H5T_INTEGER)	 	data_int[index] = data2d_int[row][col];
			if (type == H5T_NATIVE_UINT) 	data_unsigned[index] = data2d_unsigned[row][col];
			if (type == H5T_NATIVE_LONG) 	data_long[index] = data2d_long[row][col];
			if (type == H5T_NATIVE_ULONG) 	data_ulong[index] = data2d_ulong[row][col];
			if (type == H5T_NATIVE_LLONG) 	data_llong[index] = data2d_llong[row][col];
			if (type == H5T_NATIVE_ULLONG) 	data_ullong[index] = data2d_ullong[row][col];
			if (type == H5T_NATIVE_FLOAT) 	data_float[index] = data2d_float[row][col];
			if (type == H5T_FLOAT) 			data_float[index] = data2d_float[row][col];
			if (type == H5T_NATIVE_DOUBLE) 	data_double[index] = data2d_double[row][col];
			if (type == H5T_NATIVE_LDOUBLE) data_ldouble[index] = data2d_ldouble[row][col];
			if (type == H5T_NATIVE_HSIZE) 	data_hsize_t[index] = data2d_hsize_t[row][col];
			if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t[index] = data2d_hssize_t[row][col];
			if (type == H5T_NATIVE_HERR) 	data_herr_t[index] = data2d_herr_t[row][col];
			if (type == H5T_NATIVE_HBOOL) 	data_hbool_t[index] = data2d_hbool_t[row][col];
		}
	}
	
	if (type == H5T_NATIVE_CHAR)  	free(data2d_char);
	if (type == H5T_NATIVE_SCHAR) 	free(data2d_schar);
	if (type == H5T_NATIVE_UCHAR) 	free(data2d_uchar);
	if (type == H5T_NATIVE_SHORT) 	free(data2d_short);
	if (type == H5T_NATIVE_USHORT) 	free(data2d_ushort);
	if (type == H5T_NATIVE_INT) 	free(data2d_int);
	if (type == H5T_INTEGER)	 	free(data2d_int);
	if (type == H5T_NATIVE_UINT) 	free(data2d_unsigned);
	if (type == H5T_NATIVE_LONG) 	free(data2d_long);
	if (type == H5T_NATIVE_ULONG) 	free(data2d_ulong);
	if (type == H5T_NATIVE_LLONG) 	free(data2d_llong);
	if (type == H5T_NATIVE_ULLONG) 	free(data2d_ullong);
	if (type == H5T_NATIVE_FLOAT) 	free(data2d_float);
	if (type == H5T_FLOAT) 			free(data2d_float);
	if (type == H5T_NATIVE_DOUBLE) 	free(data2d_double);
	if (type == H5T_NATIVE_LDOUBLE) free(data2d_ldouble);
	if (type == H5T_NATIVE_HSIZE) 	free(data2d_hsize_t);
	if (type == H5T_NATIVE_HSSIZE) 	free(data2d_hssize_t);
	if (type == H5T_NATIVE_HERR) 	free(data2d_herr_t);
	if (type == H5T_NATIVE_HBOOL) 	free(data2d_hbool_t);
	
	void *dataout;

	if (type == H5T_NATIVE_CHAR)  	dataout = (void*) data_char;
	if (type == H5T_NATIVE_SCHAR) 	dataout = (void*) data_schar;
	if (type == H5T_NATIVE_UCHAR) 	dataout = (void*) data_uchar;
	if (type == H5T_NATIVE_SHORT) 	dataout = (void*) data_short;
	if (type == H5T_NATIVE_USHORT) 	dataout = (void*) data_ushort;
	if (type == H5T_NATIVE_INT) 	dataout = (void*) data_int;
	if (type == H5T_INTEGER)	 	dataout = (void*) data_int;
	if (type == H5T_NATIVE_UINT) 	dataout = (void*) data_unsigned;
	if (type == H5T_NATIVE_LONG) 	dataout = (void*) data_long;
	if (type == H5T_NATIVE_ULONG) 	dataout = (void*) data_ulong;
	if (type == H5T_NATIVE_LLONG) 	dataout = (void*) data_llong;
	if (type == H5T_NATIVE_ULLONG) 	dataout = (void*) data_ullong;
	if (type == H5T_NATIVE_FLOAT) 	dataout = (void*) data_float;
	if (type == H5T_FLOAT) 			dataout = (void*) data_float;
	if (type == H5T_NATIVE_DOUBLE) 	dataout = (void*) data_double;
	if (type == H5T_NATIVE_LDOUBLE) dataout = (void*) data_ldouble;
	if (type == H5T_NATIVE_HSIZE) 	dataout = (void*) data_hsize_t;
	if (type == H5T_NATIVE_HSSIZE) 	dataout = (void*) data_hssize_t;
	if (type == H5T_NATIVE_HERR) 	dataout = (void*) data_herr_t;
	if (type == H5T_NATIVE_HBOOL) 	dataout = (void*) data_hbool_t;
	
	return dataout;
}

void* convert_1d_to_2d(void *data_in, hid_t type, size_t rows, size_t cols) {

	char				**data2d_char;
	signed char			**data2d_schar;
	unsigned char		**data2d_uchar;
	short				**data2d_short;
	unsigned short		**data2d_ushort;
	long				**data2d_int;
	unsigned			**data2d_unsigned;
	long				**data2d_long;
	unsigned long		**data2d_ulong;
	long long			**data2d_llong;
	unsigned long long	**data2d_ullong;
	float				**data2d_float;
	double				**data2d_double;
	long double			**data2d_ldouble;
	hsize_t				**data2d_hsize_t;
	hssize_t			**data2d_hssize_t;
	herr_t				**data2d_herr_t;
	hbool_t				**data2d_hbool_t;
	
	char				*data_char;
	signed char			*data_schar;
	unsigned char		*data_uchar;
	short				*data_short;
	unsigned short		*data_ushort;
	long				*data_int;
	unsigned			*data_unsigned;
	long				*data_long;
	unsigned long		*data_ulong;
	long long			*data_llong;
	unsigned long long	*data_ullong;
	float				*data_float;
	double				*data_double;
	long double			*data_ldouble;
	hsize_t				*data_hsize_t;
	hssize_t			*data_hssize_t;
	herr_t				*data_herr_t;
	hbool_t				*data_hbool_t;
	
	if (type == H5T_NATIVE_CHAR)  	data2d_char = dimalloc(sizeof(char), 2, rows, cols);
	if (type == H5T_NATIVE_SCHAR) 	data2d_schar = dimalloc(sizeof(signed char), 2, rows, cols);
	if (type == H5T_NATIVE_UCHAR) 	data2d_uchar = dimalloc(sizeof(unsigned char), 2, rows, cols);
	if (type == H5T_NATIVE_SHORT) 	data2d_short = dimalloc(sizeof(short), 2, rows, cols);
	if (type == H5T_NATIVE_USHORT) 	data2d_ushort = dimalloc(sizeof(unsigned short), 2, rows, cols);
	if (type == H5T_NATIVE_INT) 	data2d_int = dimalloc(sizeof(long), 2, rows, cols);
	if (type == H5T_INTEGER)	 	data2d_int = dimalloc(sizeof(long), 2, rows, cols);
	if (type == H5T_NATIVE_UINT) 	data2d_unsigned = dimalloc(sizeof(unsigned), 2, rows, cols);
	if (type == H5T_NATIVE_LONG) 	data2d_long = dimalloc(sizeof(long), 2, rows, cols);
	if (type == H5T_NATIVE_ULONG) 	data2d_ulong = dimalloc(sizeof(unsigned long), 2, rows, cols);
	if (type == H5T_NATIVE_LLONG) 	data2d_llong = dimalloc(sizeof(long long), 2, rows, cols);
	if (type == H5T_NATIVE_ULLONG) 	data2d_ullong = dimalloc(sizeof(unsigned long long), 2, rows, cols);
	if (type == H5T_NATIVE_FLOAT) 	data2d_float = dimalloc(sizeof(float), 2, rows, cols);
	if (type == H5T_FLOAT) 			data2d_float = dimalloc(sizeof(float), 2, rows, cols);
	if (type == H5T_NATIVE_DOUBLE) 	data2d_double = dimalloc(sizeof(double), 2, rows, cols);
	if (type == H5T_NATIVE_LDOUBLE) data2d_ldouble = dimalloc(sizeof(long double), 2, rows, cols);
	if (type == H5T_NATIVE_HSIZE) 	data2d_hsize_t = dimalloc(sizeof(hsize_t), 2, rows, cols);
	if (type == H5T_NATIVE_HSSIZE) 	data2d_hssize_t = dimalloc(sizeof(hssize_t), 2, rows, cols);
	if (type == H5T_NATIVE_HERR) 	data2d_herr_t = dimalloc(sizeof(herr_t), 2, rows, cols);
	if (type == H5T_NATIVE_HBOOL) 	data2d_hbool_t = dimalloc(sizeof(hbool_t), 2, rows, cols);
	
	if (type == H5T_NATIVE_CHAR)  	data_char = (char*) data_in;
	if (type == H5T_NATIVE_SCHAR) 	data_schar = (signed char*) data_in;
	if (type == H5T_NATIVE_UCHAR) 	data_uchar = (unsigned char*) data_in;
	if (type == H5T_NATIVE_SHORT) 	data_short = (short*) data_in;
	if (type == H5T_NATIVE_USHORT) 	data_ushort = (unsigned short*) data_in;
	if (type == H5T_NATIVE_INT) 	data_int = (long*) data_in;
	if (type == H5T_INTEGER)	 	data_int = (long*) data_in;
	if (type == H5T_NATIVE_UINT) 	data_unsigned = (unsigned*) data_in;
	if (type == H5T_NATIVE_LONG) 	data_long = (long*) data_in;
	if (type == H5T_NATIVE_ULONG) 	data_ulong = (unsigned long*) data_in;
	if (type == H5T_NATIVE_LLONG) 	data_llong = (long long*) data_in;
	if (type == H5T_NATIVE_ULLONG) 	data_ullong = (unsigned long long*) data_in;
	if (type == H5T_NATIVE_FLOAT) 	data_float = (float*) data_in;
	if (type == H5T_FLOAT) 			data_float = (float*) data_in;
	if (type == H5T_NATIVE_DOUBLE) 	data_double = (double*) data_in;
	if (type == H5T_NATIVE_LDOUBLE) data_ldouble = (long double*) data_in;
	if (type == H5T_NATIVE_HSIZE) 	data_hsize_t = (hsize_t*) data_in;
	if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t = (hssize_t*) data_in;
	if (type == H5T_NATIVE_HERR) 	data_herr_t = (herr_t*) data_in;
	if (type == H5T_NATIVE_HBOOL) 	data_hbool_t = (hbool_t*) data_in;
	
	if (DEBUG_HDF5_HELPER) printf("src data shape is: r %4lu c %4lu\n", rows, cols);
	
	size_t col, row;
	for (row = 0; row < rows; row++) {
		for (col = 0; col < cols; col++) {
			size_t index = (row * cols) + col;
	
			if (type == H5T_NATIVE_CHAR)  	data2d_char[row][col] = data_char[index];
			if (type == H5T_NATIVE_SCHAR) 	data2d_schar[row][col] = data_schar[index];
			if (type == H5T_NATIVE_UCHAR) 	data2d_uchar[row][col] = data_uchar[index];
			if (type == H5T_NATIVE_SHORT) 	data2d_short[row][col] = data_short[index];
			if (type == H5T_NATIVE_USHORT) 	data2d_ushort[row][col] = data_ushort[index];
			if (type == H5T_NATIVE_INT) 	data2d_int[row][col] = data_int[index];
			if (type == H5T_INTEGER)	 	data2d_int[row][col] = data_int[index];
			if (type == H5T_NATIVE_UINT) 	data2d_unsigned[row][col] = data_unsigned[index];
			if (type == H5T_NATIVE_LONG) 	data2d_long[row][col] = data_long[index];
			if (type == H5T_NATIVE_ULONG) 	data2d_ulong[row][col] = data_ulong[index];
			if (type == H5T_NATIVE_LLONG) 	data2d_llong[row][col] = data_llong[index];
			if (type == H5T_NATIVE_ULLONG) 	data2d_ullong[row][col] = data_ullong[index];
			if (type == H5T_NATIVE_FLOAT) 	data2d_float[row][col] = data_float[index];
			if (type == H5T_FLOAT) 			data2d_float[row][col] = data_float[index];
			if (type == H5T_NATIVE_DOUBLE) 	data2d_double[row][col] = data_double[index];
			if (type == H5T_NATIVE_LDOUBLE) data2d_ldouble[row][col] = data_ldouble[index];
			if (type == H5T_NATIVE_HSIZE) 	data2d_hsize_t[row][col] = data_hsize_t[index];
			if (type == H5T_NATIVE_HSSIZE) 	data2d_hssize_t[row][col] = data_hssize_t[index];
			if (type == H5T_NATIVE_HERR) 	data2d_herr_t[row][col] = data_herr_t[index];
			if (type == H5T_NATIVE_HBOOL) 	data2d_hbool_t[row][col] = data_hbool_t[index];
		}
	}
	
	if (type == H5T_NATIVE_CHAR)  	free(data_char);
	if (type == H5T_NATIVE_SCHAR) 	free(data_schar);
	if (type == H5T_NATIVE_UCHAR) 	free(data_uchar);
	if (type == H5T_NATIVE_SHORT) 	free(data_short);
	if (type == H5T_NATIVE_USHORT) 	free(data_ushort);
	if (type == H5T_NATIVE_INT) 	free(data_int);
	if (type == H5T_INTEGER)	 	free(data_int);
	if (type == H5T_NATIVE_UINT) 	free(data_unsigned);
	if (type == H5T_NATIVE_LONG) 	free(data_long);
	if (type == H5T_NATIVE_ULONG) 	free(data_ulong);
	if (type == H5T_NATIVE_LLONG) 	free(data_llong);
	if (type == H5T_NATIVE_ULLONG) 	free(data_ullong);
	if (type == H5T_NATIVE_FLOAT) 	free(data_float);
	if (type == H5T_FLOAT) 			free(data_float);
	if (type == H5T_NATIVE_DOUBLE) 	free(data_double);
	if (type == H5T_NATIVE_LDOUBLE) free(data_ldouble);
	if (type == H5T_NATIVE_HSIZE) 	free(data_hsize_t);
	if (type == H5T_NATIVE_HSSIZE) 	free(data_hssize_t);
	if (type == H5T_NATIVE_HERR) 	free(data_herr_t);
	if (type == H5T_NATIVE_HBOOL) 	free(data_hbool_t);
	
	void *dataout;

	if (type == H5T_NATIVE_CHAR)  	dataout = (void*) data2d_char;
	if (type == H5T_NATIVE_SCHAR) 	dataout = (void*) data2d_schar;
	if (type == H5T_NATIVE_UCHAR) 	dataout = (void*) data2d_uchar;
	if (type == H5T_NATIVE_SHORT) 	dataout = (void*) data2d_short;
	if (type == H5T_NATIVE_USHORT) 	dataout = (void*) data2d_ushort;
	if (type == H5T_NATIVE_INT) 	dataout = (void*) data2d_int;
	if (type == H5T_INTEGER)	 	dataout = (void*) data2d_int;
	if (type == H5T_NATIVE_UINT) 	dataout = (void*) data2d_unsigned;
	if (type == H5T_NATIVE_LONG) 	dataout = (void*) data2d_long;
	if (type == H5T_NATIVE_ULONG) 	dataout = (void*) data2d_ulong;
	if (type == H5T_NATIVE_LLONG) 	dataout = (void*) data2d_llong;
	if (type == H5T_NATIVE_ULLONG) 	dataout = (void*) data2d_ullong;
	if (type == H5T_NATIVE_FLOAT) 	dataout = (void*) data2d_float;
	if (type == H5T_FLOAT) 			dataout = (void*) data2d_float;
	if (type == H5T_NATIVE_DOUBLE) 	dataout = (void*) data2d_double;
	if (type == H5T_NATIVE_LDOUBLE) dataout = (void*) data2d_ldouble;
	if (type == H5T_NATIVE_HSIZE) 	dataout = (void*) data2d_hsize_t;
	if (type == H5T_NATIVE_HSSIZE) 	dataout = (void*) data2d_hssize_t;
	if (type == H5T_NATIVE_HERR) 	dataout = (void*) data2d_herr_t;
	if (type == H5T_NATIVE_HBOOL) 	dataout = (void*) data2d_hbool_t;
	
	return dataout;
}

void* convert_3d_to_1d(void ***data_in, hid_t type, size_t levs, size_t rows, size_t cols) {

	char				*data_char;
	signed char			*data_schar;
	unsigned char		*data_uchar;
	short				*data_short;
	unsigned short		*data_ushort;
	long				*data_int;
	unsigned			*data_unsigned;
	long				*data_long;
	unsigned long		*data_ulong;
	long long			*data_llong;
	unsigned long long	*data_ullong;
	float				*data_float;
	double				*data_double;
	long double			*data_ldouble;
	hsize_t				*data_hsize_t;
	hssize_t			*data_hssize_t;
	herr_t				*data_herr_t;
	hbool_t				*data_hbool_t;
	
	char				***data3d_char;
	signed char			***data3d_schar;
	unsigned char		***data3d_uchar;
	short				***data3d_short;
	unsigned short		***data3d_ushort;
	long				***data3d_int;
	unsigned			***data3d_unsigned;
	long				***data3d_long;
	unsigned long		***data3d_ulong;
	long long			***data3d_llong;
	unsigned long long	***data3d_ullong;
	float				***data3d_float;
	double				***data3d_double;
	long double			***data3d_ldouble;
	hsize_t				***data3d_hsize_t;
	hssize_t			***data3d_hssize_t;
	herr_t				***data3d_herr_t;
	hbool_t				***data3d_hbool_t;
	
	if (type == H5T_NATIVE_CHAR)  	data_char = malloc(sizeof(char) * levs * rows * cols);
	if (type == H5T_NATIVE_SCHAR) 	data_schar = malloc(sizeof(signed char) * levs * rows * cols);
	if (type == H5T_NATIVE_UCHAR) 	data_uchar = malloc(sizeof(unsigned char) * levs * rows * cols);
	if (type == H5T_NATIVE_SHORT) 	data_short = malloc(sizeof(short) * levs * rows * cols);
	if (type == H5T_NATIVE_USHORT) 	data_ushort = malloc(sizeof(unsigned short) * levs * rows * cols);
	if (type == H5T_NATIVE_INT) 	data_int = malloc(sizeof(long) * levs * rows * cols);
	if (type == H5T_INTEGER)	 	data_int = malloc(sizeof(long) * levs * rows * cols);
	if (type == H5T_NATIVE_UINT) 	data_unsigned = malloc(sizeof(unsigned) * levs * rows * cols);
	if (type == H5T_NATIVE_LONG) 	data_long = malloc(sizeof(long) * levs * rows * cols);
	if (type == H5T_NATIVE_ULONG) 	data_ulong = malloc(sizeof(unsigned long) * levs * rows * cols);
	if (type == H5T_NATIVE_LLONG) 	data_llong = malloc(sizeof(long long) * levs * rows * cols);
	if (type == H5T_NATIVE_ULLONG) 	data_ullong = malloc(sizeof(unsigned long long) * levs * rows * cols);
	if (type == H5T_NATIVE_FLOAT) 	data_float = malloc(sizeof(float) * levs * rows * cols);
	if (type == H5T_FLOAT) 			data_float = malloc(sizeof(float) * levs * rows * cols);
	if (type == H5T_NATIVE_DOUBLE) 	data_double = malloc(sizeof(double) * levs * rows * cols);
	if (type == H5T_NATIVE_LDOUBLE) data_ldouble = malloc(sizeof(long double) * levs * rows * cols);
	if (type == H5T_NATIVE_HSIZE) 	data_hsize_t = malloc(sizeof(hsize_t) * levs * rows * cols);
	if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t = malloc(sizeof(hssize_t) * levs * rows * cols);
	if (type == H5T_NATIVE_HERR) 	data_herr_t = malloc(sizeof(herr_t) * levs * rows * cols);
	if (type == H5T_NATIVE_HBOOL) 	data_hbool_t = malloc(sizeof(hbool_t) * levs * rows * cols);
	
	if (type == H5T_NATIVE_CHAR)  	data3d_char = (char***) data_in;
	if (type == H5T_NATIVE_SCHAR) 	data3d_schar = (signed char***) data_in;
	if (type == H5T_NATIVE_UCHAR) 	data3d_uchar = (unsigned char***) data_in;
	if (type == H5T_NATIVE_SHORT) 	data3d_short = (short***) data_in;
	if (type == H5T_NATIVE_USHORT) 	data3d_ushort = (unsigned short***) data_in;
	if (type == H5T_NATIVE_INT) 	data3d_int = (long***) data_in;
	if (type == H5T_INTEGER)	 	data3d_int = (long***) data_in;
	if (type == H5T_NATIVE_UINT) 	data3d_unsigned = (unsigned***) data_in;
	if (type == H5T_NATIVE_LONG) 	data3d_long = (long***) data_in;
	if (type == H5T_NATIVE_ULONG) 	data3d_ulong = (unsigned long***) data_in;
	if (type == H5T_NATIVE_LLONG) 	data3d_llong = (long long***) data_in;
	if (type == H5T_NATIVE_ULLONG) 	data3d_ullong = (unsigned long long***) data_in;
	if (type == H5T_NATIVE_FLOAT) 	data3d_float = (float***) data_in;
	if (type == H5T_FLOAT) 			data3d_float = (float***) data_in;
	if (type == H5T_NATIVE_DOUBLE) 	data3d_double = (double***) data_in;
	if (type == H5T_NATIVE_LDOUBLE) data3d_ldouble = (long double***) data_in;
	if (type == H5T_NATIVE_HSIZE) 	data3d_hsize_t = (hsize_t***) data_in;
	if (type == H5T_NATIVE_HSSIZE) 	data3d_hssize_t = (hssize_t***) data_in;
	if (type == H5T_NATIVE_HERR) 	data3d_herr_t = (herr_t***) data_in;
	if (type == H5T_NATIVE_HBOOL) 	data3d_hbool_t = (hbool_t***) data_in;
	
	if (DEBUG_HDF5_HELPER) printf("new data shape is: r %4lu c %4lu\n", rows, cols);
	
	size_t col, row, lev;
	for (lev = 0; lev < levs; lev++) {
		for (row = 0; row < rows; row++) {
			for (col = 0; col < cols; col++) {
				size_t index = (row * cols) + col;
		
				if (type == H5T_NATIVE_CHAR)  	data_char[index] = data3d_char[lev][row][col];
				if (type == H5T_NATIVE_SCHAR) 	data_schar[index] = data3d_schar[lev][row][col];
				if (type == H5T_NATIVE_UCHAR) 	data_uchar[index] = data3d_uchar[lev][row][col];
				if (type == H5T_NATIVE_SHORT) 	data_short[index] = data3d_short[lev][row][col];
				if (type == H5T_NATIVE_USHORT) 	data_ushort[index] = data3d_ushort[lev][row][col];
				if (type == H5T_NATIVE_INT) 	data_int[index] = data3d_int[lev][row][col];
				if (type == H5T_INTEGER)	 	data_int[index] = data3d_int[lev][row][col];
				if (type == H5T_NATIVE_UINT) 	data_unsigned[index] = data3d_unsigned[lev][row][col];
				if (type == H5T_NATIVE_LONG) 	data_long[index] = data3d_long[lev][row][col];
				if (type == H5T_NATIVE_ULONG) 	data_ulong[index] = data3d_ulong[lev][row][col];
				if (type == H5T_NATIVE_LLONG) 	data_llong[index] = data3d_llong[lev][row][col];
				if (type == H5T_NATIVE_ULLONG) 	data_ullong[index] = data3d_ullong[lev][row][col];
				if (type == H5T_NATIVE_FLOAT) 	data_float[index] = data3d_float[lev][row][col];
				if (type == H5T_FLOAT) 			data_float[index] = data3d_float[lev][row][col];
				if (type == H5T_NATIVE_DOUBLE) 	data_double[index] = data3d_double[lev][row][col];
				if (type == H5T_NATIVE_LDOUBLE) data_ldouble[index] = data3d_ldouble[lev][row][col];
				if (type == H5T_NATIVE_HSIZE) 	data_hsize_t[index] = data3d_hsize_t[lev][row][col];
				if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t[index] = data3d_hssize_t[lev][row][col];
				if (type == H5T_NATIVE_HERR) 	data_herr_t[index] = data3d_herr_t[lev][row][col];
				if (type == H5T_NATIVE_HBOOL) 	data_hbool_t[index] = data3d_hbool_t[lev][row][col];
			}
		}
	}
	
	if (type == H5T_NATIVE_CHAR)  	free(data3d_char);
	if (type == H5T_NATIVE_SCHAR) 	free(data3d_schar);
	if (type == H5T_NATIVE_UCHAR) 	free(data3d_uchar);
	if (type == H5T_NATIVE_SHORT) 	free(data3d_short);
	if (type == H5T_NATIVE_USHORT) 	free(data3d_ushort);
	if (type == H5T_NATIVE_INT) 	free(data3d_int);
	if (type == H5T_INTEGER)	 	free(data3d_int);
	if (type == H5T_NATIVE_UINT) 	free(data3d_unsigned);
	if (type == H5T_NATIVE_LONG) 	free(data3d_long);
	if (type == H5T_NATIVE_ULONG) 	free(data3d_ulong);
	if (type == H5T_NATIVE_LLONG) 	free(data3d_llong);
	if (type == H5T_NATIVE_ULLONG) 	free(data3d_ullong);
	if (type == H5T_NATIVE_FLOAT) 	free(data3d_float);
	if (type == H5T_FLOAT) 			free(data3d_float);
	if (type == H5T_NATIVE_DOUBLE) 	free(data3d_double);
	if (type == H5T_NATIVE_LDOUBLE) free(data3d_ldouble);
	if (type == H5T_NATIVE_HSIZE) 	free(data3d_hsize_t);
	if (type == H5T_NATIVE_HSSIZE) 	free(data3d_hssize_t);
	if (type == H5T_NATIVE_HERR) 	free(data3d_herr_t);
	if (type == H5T_NATIVE_HBOOL) 	free(data3d_hbool_t);
	
	void *dataout;

	if (type == H5T_NATIVE_CHAR)  	dataout = (void*) data_char;
	if (type == H5T_NATIVE_SCHAR) 	dataout = (void*) data_schar;
	if (type == H5T_NATIVE_UCHAR) 	dataout = (void*) data_uchar;
	if (type == H5T_NATIVE_SHORT) 	dataout = (void*) data_short;
	if (type == H5T_NATIVE_USHORT) 	dataout = (void*) data_ushort;
	if (type == H5T_NATIVE_INT) 	dataout = (void*) data_int;
	if (type == H5T_INTEGER)	 	dataout = (void*) data_int;
	if (type == H5T_NATIVE_UINT) 	dataout = (void*) data_unsigned;
	if (type == H5T_NATIVE_LONG) 	dataout = (void*) data_long;
	if (type == H5T_NATIVE_ULONG) 	dataout = (void*) data_ulong;
	if (type == H5T_NATIVE_LLONG) 	dataout = (void*) data_llong;
	if (type == H5T_NATIVE_ULLONG) 	dataout = (void*) data_ullong;
	if (type == H5T_NATIVE_FLOAT) 	dataout = (void*) data_float;
	if (type == H5T_FLOAT) 			dataout = (void*) data_float;
	if (type == H5T_NATIVE_DOUBLE) 	dataout = (void*) data_double;
	if (type == H5T_NATIVE_LDOUBLE) dataout = (void*) data_ldouble;
	if (type == H5T_NATIVE_HSIZE) 	dataout = (void*) data_hsize_t;
	if (type == H5T_NATIVE_HSSIZE) 	dataout = (void*) data_hssize_t;
	if (type == H5T_NATIVE_HERR) 	dataout = (void*) data_herr_t;
	if (type == H5T_NATIVE_HBOOL) 	dataout = (void*) data_hbool_t;
	
	return dataout;
}

void* convert_1d_to_3d(void *data_in, hid_t type, size_t levs, size_t rows, size_t cols) {

	char				***data3d_char;
	signed char			***data3d_schar;
	unsigned char		***data3d_uchar;
	short				***data3d_short;
	unsigned short		***data3d_ushort;
	long				***data3d_int;
	unsigned			***data3d_unsigned;
	long				***data3d_long;
	unsigned long		***data3d_ulong;
	long long			***data3d_llong;
	unsigned long long	***data3d_ullong;
	float				***data3d_float;
	double				***data3d_double;
	long double			***data3d_ldouble;
	hsize_t				***data3d_hsize_t;
	hssize_t			***data3d_hssize_t;
	herr_t				***data3d_herr_t;
	hbool_t				***data3d_hbool_t;
	
	char				*data_char;
	signed char			*data_schar;
	unsigned char		*data_uchar;
	short				*data_short;
	unsigned short		*data_ushort;
	long				*data_int;
	unsigned			*data_unsigned;
	long				*data_long;
	unsigned long		*data_ulong;
	long long			*data_llong;
	unsigned long long	*data_ullong;
	float				*data_float;
	double				*data_double;
	long double			*data_ldouble;
	hsize_t				*data_hsize_t;
	hssize_t			*data_hssize_t;
	herr_t				*data_herr_t;
	hbool_t				*data_hbool_t;
	
	if (type == H5T_NATIVE_CHAR)  	data3d_char = dimalloc(sizeof(char), 3, levs, rows, cols);
	if (type == H5T_NATIVE_SCHAR) 	data3d_schar = dimalloc(sizeof(signed char), 3, levs, rows, cols);
	if (type == H5T_NATIVE_UCHAR) 	data3d_uchar = dimalloc(sizeof(unsigned char), 3, levs, rows, cols);
	if (type == H5T_NATIVE_SHORT) 	data3d_short = dimalloc(sizeof(short), 3, levs, rows, cols);
	if (type == H5T_NATIVE_USHORT) 	data3d_ushort = dimalloc(sizeof(unsigned short), 3, levs, rows, cols);
	if (type == H5T_NATIVE_INT) 	data3d_int = dimalloc(sizeof(long), 3, levs, rows, cols);
	if (type == H5T_INTEGER)	 	data3d_int = dimalloc(sizeof(long), 3, levs, rows, cols);
	if (type == H5T_NATIVE_UINT) 	data3d_unsigned = dimalloc(sizeof(unsigned), 3, levs, rows, cols);
	if (type == H5T_NATIVE_LONG) 	data3d_long = dimalloc(sizeof(long), 3, levs, rows, cols);
	if (type == H5T_NATIVE_ULONG) 	data3d_ulong = dimalloc(sizeof(unsigned long), 3, levs, rows, cols);
	if (type == H5T_NATIVE_LLONG) 	data3d_llong = dimalloc(sizeof(long long), 3, levs, rows, cols);
	if (type == H5T_NATIVE_ULLONG) 	data3d_ullong = dimalloc(sizeof(unsigned long long), 3, levs, rows, cols);
	if (type == H5T_NATIVE_FLOAT) 	data3d_float = dimalloc(sizeof(float), 3, levs, rows, cols);
	if (type == H5T_FLOAT) 			data3d_float = dimalloc(sizeof(float), 3, levs, rows, cols);
	if (type == H5T_NATIVE_DOUBLE) 	data3d_double = dimalloc(sizeof(double), 3, levs, rows, cols);
	if (type == H5T_NATIVE_LDOUBLE) data3d_ldouble = dimalloc(sizeof(long double), 3, levs, rows, cols);
	if (type == H5T_NATIVE_HSIZE) 	data3d_hsize_t = dimalloc(sizeof(hsize_t), 3, levs, rows, cols);
	if (type == H5T_NATIVE_HSSIZE) 	data3d_hssize_t = dimalloc(sizeof(hssize_t), 3, levs, rows, cols);
	if (type == H5T_NATIVE_HERR) 	data3d_herr_t = dimalloc(sizeof(herr_t), 3, levs, rows, cols);
	if (type == H5T_NATIVE_HBOOL) 	data3d_hbool_t = dimalloc(sizeof(hbool_t), 3, levs, rows, cols);
	
	if (type == H5T_NATIVE_CHAR) 	data_char = (char*) data_in;
	if (type == H5T_NATIVE_SCHAR) 	data_schar = (signed char*) data_in;
	if (type == H5T_NATIVE_UCHAR) 	data_uchar = (unsigned char*) data_in;
	if (type == H5T_NATIVE_SHORT) 	data_short = (short*) data_in;
	if (type == H5T_NATIVE_USHORT) 	data_ushort = (unsigned short*) data_in;
	if (type == H5T_NATIVE_INT) 	data_int = (long*) data_in;
	if (type == H5T_INTEGER)	 	data_int = (long*) data_in;
	if (type == H5T_NATIVE_UINT) 	data_unsigned = (unsigned*) data_in;
	if (type == H5T_NATIVE_LONG) 	data_long = (long*) data_in;
	if (type == H5T_NATIVE_ULONG) 	data_ulong = (unsigned long*) data_in;
	if (type == H5T_NATIVE_LLONG) 	data_llong = (long long*) data_in;
	if (type == H5T_NATIVE_ULLONG) 	data_ullong = (unsigned long long*) data_in;
	if (type == H5T_NATIVE_FLOAT) 	data_float = (float*) data_in;
	if (type == H5T_FLOAT) 			data_float = (float*) data_in;
	if (type == H5T_NATIVE_DOUBLE) 	data_double = (double*) data_in;
	if (type == H5T_NATIVE_LDOUBLE) data_ldouble = (long double*) data_in;
	if (type == H5T_NATIVE_HSIZE) 	data_hsize_t = (hsize_t*) data_in;
	if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t = (hssize_t*) data_in;
	if (type == H5T_NATIVE_HERR) 	data_herr_t = (herr_t*) data_in;
	if (type == H5T_NATIVE_HBOOL) 	data_hbool_t = (hbool_t*) data_in;
	
	if (DEBUG_HDF5_HELPER) printf("src data shape is: l %4lu r %4lu c %4lu\n", levs, rows, cols);
	
	size_t col, row, lev;
	for (lev = 0; lev < levs; lev++) {
		for (row = 0; row < rows; row++) {
			for (col = 0; col < cols; col++) {
				size_t index = (row * cols) + col;
		
				if (type == H5T_NATIVE_CHAR)  	data3d_char[lev][row][col] = data_char[index];
				if (type == H5T_NATIVE_SCHAR) 	data3d_schar[lev][row][col] = data_schar[index];
				if (type == H5T_NATIVE_UCHAR) 	data3d_uchar[lev][row][col] = data_uchar[index];
				if (type == H5T_NATIVE_SHORT) 	data3d_short[lev][row][col] = data_short[index];
				if (type == H5T_NATIVE_USHORT) 	data3d_ushort[lev][row][col] = data_ushort[index];
				if (type == H5T_NATIVE_INT) 	data3d_int[lev][row][col] = data_int[index];
				if (type == H5T_INTEGER)	 	data3d_int[lev][row][col] = data_int[index];
				if (type == H5T_NATIVE_UINT) 	data3d_unsigned[lev][row][col] = data_unsigned[index];
				if (type == H5T_NATIVE_LONG) 	data3d_long[lev][row][col] = data_long[index];
				if (type == H5T_NATIVE_ULONG) 	data3d_ulong[lev][row][col] = data_ulong[index];
				if (type == H5T_NATIVE_LLONG) 	data3d_llong[lev][row][col] = data_llong[index];
				if (type == H5T_NATIVE_ULLONG) 	data3d_ullong[lev][row][col] = data_ullong[index];
				if (type == H5T_NATIVE_FLOAT) 	data3d_float[lev][row][col] = data_float[index];
				if (type == H5T_FLOAT) 			data3d_float[lev][row][col] = data_float[index];
				if (type == H5T_NATIVE_DOUBLE) 	data3d_double[lev][row][col] = data_double[index];
				if (type == H5T_NATIVE_LDOUBLE) data3d_ldouble[lev][row][col] = data_ldouble[index];
				if (type == H5T_NATIVE_HSIZE) 	data3d_hsize_t[lev][row][col] = data_hsize_t[index];
				if (type == H5T_NATIVE_HSSIZE) 	data3d_hssize_t[lev][row][col] = data_hssize_t[index];
				if (type == H5T_NATIVE_HERR) 	data3d_herr_t[lev][row][col] = data_herr_t[index];
				if (type == H5T_NATIVE_HBOOL) 	data3d_hbool_t[lev][row][col] = data_hbool_t[index];
			}
		}
	}
	
	if (type == H5T_NATIVE_CHAR)  	free(data_char);
	if (type == H5T_NATIVE_SCHAR) 	free(data_schar);
	if (type == H5T_NATIVE_UCHAR) 	free(data_uchar);
	if (type == H5T_NATIVE_SHORT) 	free(data_short);
	if (type == H5T_NATIVE_USHORT) 	free(data_ushort);
	if (type == H5T_NATIVE_INT) 	free(data_int);
	if (type == H5T_INTEGER)	 	free(data_int);
	if (type == H5T_NATIVE_UINT) 	free(data_unsigned);
	if (type == H5T_NATIVE_LONG) 	free(data_long);
	if (type == H5T_NATIVE_ULONG) 	free(data_ulong);
	if (type == H5T_NATIVE_LLONG) 	free(data_llong);
	if (type == H5T_NATIVE_ULLONG) 	free(data_ullong);
	if (type == H5T_NATIVE_FLOAT) 	free(data_float);
	if (type == H5T_FLOAT) 			free(data_float);
	if (type == H5T_NATIVE_DOUBLE) 	free(data_double);
	if (type == H5T_NATIVE_LDOUBLE) free(data_ldouble);
	if (type == H5T_NATIVE_HSIZE) 	free(data_hsize_t);
	if (type == H5T_NATIVE_HSSIZE) 	free(data_hssize_t);
	if (type == H5T_NATIVE_HERR) 	free(data_herr_t);
	if (type == H5T_NATIVE_HBOOL) 	free(data_hbool_t);
	
	void *dataout;

	if (type == H5T_NATIVE_CHAR)  	dataout = (void*) data3d_char;
	if (type == H5T_NATIVE_SCHAR) 	dataout = (void*) data3d_schar;
	if (type == H5T_NATIVE_UCHAR) 	dataout = (void*) data3d_uchar;
	if (type == H5T_NATIVE_SHORT) 	dataout = (void*) data3d_short;
	if (type == H5T_NATIVE_USHORT) 	dataout = (void*) data3d_ushort;
	if (type == H5T_NATIVE_INT) 	dataout = (void*) data3d_int;
	if (type == H5T_INTEGER)	 	dataout = (void*) data3d_int;
	if (type == H5T_NATIVE_UINT) 	dataout = (void*) data3d_unsigned;
	if (type == H5T_NATIVE_LONG) 	dataout = (void*) data3d_long;
	if (type == H5T_NATIVE_ULONG) 	dataout = (void*) data3d_ulong;
	if (type == H5T_NATIVE_LLONG) 	dataout = (void*) data3d_llong;
	if (type == H5T_NATIVE_ULLONG) 	dataout = (void*) data3d_ullong;
	if (type == H5T_NATIVE_FLOAT) 	dataout = (void*) data3d_float;
	if (type == H5T_FLOAT) 			dataout = (void*) data3d_float;
	if (type == H5T_NATIVE_DOUBLE) 	dataout = (void*) data3d_double;
	if (type == H5T_NATIVE_LDOUBLE) dataout = (void*) data3d_ldouble;
	if (type == H5T_NATIVE_HSIZE) 	dataout = (void*) data3d_hsize_t;
	if (type == H5T_NATIVE_HSSIZE) 	dataout = (void*) data3d_hssize_t;
	if (type == H5T_NATIVE_HERR) 	dataout = (void*) data3d_herr_t;
	if (type == H5T_NATIVE_HBOOL) 	dataout = (void*) data3d_hbool_t;
	
	return dataout;
}

void* get_variable_ids_by_name(const char* path, const char* group, const char* name) {
	if (DEBUG_HDF5_HELPER) printf("get_variable_ids_by_name: %s %s %s \n", path, group, name);
	
	// Open the file by name
	hid_t h5id = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
	
	// Determine group hierarchy based on full variable path name
	hid_t grp_h5id = H5Gopen(h5id, group, H5P_DEFAULT);

	// Determine variable count in the group and get varids for each
	//hid_t varids[100] = {0};
	
	hsize_t num_obj = 0;
	// herr_t err_nvars = 
	H5Gget_num_objs(grp_h5id, &num_obj);

	if (DEBUG_HDF5_HELPER) {
	/*
		long NC_MAX_NAME = 100;
		printf("%s contains %d variables\n", group, (int) num_obj);
		int var_i;
		for (var_i = 0; var_i < num_obj; var_i++) {
			char name[NC_MAX_NAME + 1];
			//nc_inq_varname(grp_ncid, varids[var_i], name);
			
			ssize_t ret_size = H5Gget_objname_by_idx(grp_h5id, var_i, name, NC_MAX_NAME);
			hid_t varid = H5Dopen(grp_h5id, name, H5P_DEFAULT);
			
			varids[var_i] = varid;
			
			printf("%3d\t%3d\t%50s [%d]\n", var_i, varids[var_i], name, (int) ret_size);
		}
	*/
	}
	
	// Retrieve the varid by the variable name
	hid_t varid = H5Dopen(grp_h5id, name, H5P_DEFAULT);
	
	hid_t* ret_ids = (hid_t*) malloc(sizeof(hid_t) * 3);
	ret_ids[0] = h5id;
	ret_ids[1] = grp_h5id;
	ret_ids[2] = varid;
	
	if (DEBUG_HDF5_HELPER) printf("getting %d %d %d\n", (int) ret_ids[0], (int) ret_ids[1], (int) ret_ids[2]);
	
	return (void*) ret_ids;
}

void* get_variable_dims(hid_t grp_h5id, hid_t varid) {
	if (DEBUG_HDF5_HELPER) printf("get_variable_dims %d %d \n", (int) grp_h5id, (int) varid);
	
	// Retrieve the metadata for the variable to learn
	// ... dimensions (need all of the dimension ids)
	
	hid_t dspace = H5Dget_space(varid);
	//int ndims = H5Sget_simple_extent_ndims(dspace);
		
	hsize_t* dims = (hsize_t*) malloc(sizeof(hsize_t) * MAX_DIMS);
	int i;
	for (i = 0; i < MAX_DIMS; i++) dims[i] = 1;
	
	H5Sget_simple_extent_dims(dspace, dims, NULL);
	
	return (void*) dims;
}

void* get_variable_dims_by_name(const char* path, const char* group, const char* name) {
	if (DEBUG_HDF5_HELPER) printf("get_variable_dims_by_name: %s %s %s \n", path, group, name);
		
	hid_t* ids     = (hid_t*) get_variable_ids_by_name(path, group, name); // h5id remains open, need to close!
	hid_t h5id     = ids[0];
	hid_t grp_h5id = ids[1];
	hid_t varid    = ids[2];
	free(ids);
	
	size_t* dims = (size_t*) get_variable_dims(grp_h5id, varid);
	
	H5Fclose(h5id);
	
	return (void*) dims;
}

H5T_class_t get_variable_type(hid_t varid) {
	if (DEBUG_HDF5_HELPER) printf("get_variable_type\n");
	
	hid_t xtypep = H5Dget_type(varid);
 	H5T_class_t dataset_class = H5Tget_class(xtypep);
 	if (DEBUG_HDF5_HELPER) printf("%d\n", (int) xtypep);
 	if (DEBUG_HDF5_HELPER) printf("%d\n", dataset_class);
 	H5Tclose(xtypep);
	
	//  // needs to be closed after use
	return dataset_class;

}

H5T_class_t get_variable_type_by_name(const char* path, const char* group, const char* name) {
	if (DEBUG_HDF5_HELPER) printf("get_variable_type_by_name: %s %s %s \n", path, group, name);
	
	hid_t* ids     = (hid_t*) get_variable_ids_by_name(path, group, name); // h5id remains open, need to close!
	//hid_t h5id     = ids[0];
	//hid_t grp_h5id = ids[1];
	hid_t varid    = ids[2];
	free(ids);
	
	H5T_class_t type = get_variable_type(varid);
	
	return type;
	
}

size_t get_type_size(hid_t xtypep) {
	/*
	H5T_NATIVE_CHAR 	char
	H5T_NATIVE_SCHAR 	signed char
	H5T_NATIVE_UCHAR 	unsigned char
	H5T_NATIVE_SHORT 	short
	H5T_NATIVE_USHORT 	unsigned short
	H5T_NATIVE_INT 		int
	H5T_INTEGER 		int
	H5T_NATIVE_UINT 	unsigned
	H5T_NATIVE_LONG 	long
	H5T_NATIVE_ULONG 	unsigned long
	H5T_NATIVE_LLONG 	long long
	H5T_NATIVE_ULLONG 	unsigned long long
	H5T_NATIVE_FLOAT 	float
	H5T_FLOAT		 	float
	H5T_NATIVE_DOUBLE 	double
	H5T_NATIVE_LDOUBLE 	long double
	H5T_NATIVE_HSIZE 	hsize_t
	H5T_NATIVE_HSSIZE 	hssize_t
	H5T_NATIVE_HERR 	herr_t
	H5T_NATIVE_HBOOL 	hbool_t
	*/
	size_t type_size; // = H5Tget_size(xtypep);
	if (xtypep == H5T_NATIVE_CHAR)
		type_size = sizeof(char);
	if (xtypep == H5T_NATIVE_SCHAR)
		type_size = sizeof(signed char);
	if (xtypep == H5T_NATIVE_UCHAR)
		type_size = sizeof(unsigned char);
	if (xtypep == H5T_NATIVE_SHORT)
		type_size = sizeof(short);
	if (xtypep == H5T_NATIVE_USHORT)
		type_size = sizeof(unsigned short);
	if (xtypep == H5T_NATIVE_INT || xtypep == H5T_INTEGER)
		type_size = sizeof(long);
	if (xtypep == H5T_NATIVE_UINT)
		type_size = sizeof(unsigned int);
	if (xtypep == H5T_NATIVE_LONG)
		type_size = sizeof(long);
	if (xtypep == H5T_NATIVE_ULONG)
		type_size = sizeof(unsigned long);
	if (xtypep == H5T_NATIVE_LLONG)
		type_size = sizeof(long long);
	if (xtypep == H5T_NATIVE_ULLONG)
		type_size = sizeof(unsigned long long);
	if (xtypep == H5T_NATIVE_FLOAT || xtypep == H5T_FLOAT)
		type_size = sizeof(float);
	if (xtypep == H5T_NATIVE_DOUBLE)
		type_size = sizeof(double);
	if (xtypep == H5T_NATIVE_LDOUBLE)
		type_size = sizeof(long double);
	if (xtypep == H5T_NATIVE_HSIZE)
		type_size = sizeof(hsize_t);
	if (xtypep == H5T_NATIVE_HSSIZE)
		type_size = sizeof(hssize_t);
	if (xtypep == H5T_NATIVE_HERR)
		type_size = sizeof(herr_t);
	if (xtypep == H5T_NATIVE_HBOOL)
		type_size = sizeof(hbool_t);
		
	if (DEBUG_HDF5_HELPER) printf("type_size is %d\n", (int) type_size);
	
	return type_size;
}

void* get_variable_data_by_name(const char* path, const char* group, const char* name) {
	if (DEBUG_HDF5_HELPER) printf("get_variable_data_by_name: %s %s %s \n", path, group, name);
	
	hid_t* ids  = (hid_t*) get_variable_ids_by_name(path, group, name); // ncid remains open, need to close!
	//hid_t h5id = ids[0];
	hid_t grp_h5id = ids[1];
	hid_t varid = ids[2];
	free(ids);
	
	size_t* dims = (size_t*) get_variable_dims(grp_h5id, varid);
	
	size_t total_data_count = 1;
	int dim_i;
	for (dim_i = 0; dim_i < MAX_DIMS; dim_i++) {
		if (dims[dim_i] != 0) total_data_count *= dims[dim_i];
	}
	
	free(dims);
	
	// Get size based on the returned type
	//H5T_class_t type = get_variable_type(varid);
	//size_t type_size = get_type_size(type);

	hid_t type = H5Dget_type(varid);
	
	//size_t type_size = get_type_size(type); //H5Tget_size(type);
	size_t type_size = H5Tget_size(type);

	// .. end-edness
	// TODO
	//int int nc_inq_var_endian(int ncid, hid_t varid, int *endian)
	/*		>>>> Should already be handeled within the library.
	int endian = 0;
	int err_end = nc_inq_var_endian(ncid, varid, &endian);
	 #define NC_ENDIAN_LITTLE
	 #define NC_ENDIAN_BIG
	 #define NC_ENDIAN_NATIVE
	 */
	
	// .. fill value
	// TODO
	// Types may be an issue
	/*
	float fill_value = 1;
	int err_scale = nc_get_att(ncid, varid, _FillValue, (void*) scale_factor);
	 */
	 
	// .. scale factor
	/* TODO
	float scale_factor = 1;
	int err_scale = nc_get_att(ncid, varid, "scale_factor", (void*) scale_factor);
	 */
	 
	// .. offset
	/* TODO
	float offset_factor = 0;
	int err_offset = nc_get_att(ncid, varid, "offset", (void*) offset);
	 */
	
	if (DEBUG_HDF5_HELPER) printf("type_size: %d total_data_count: %d\n", (int) type_size, (int) total_data_count);
	
	// Retrieve the data using the varid
	//int nc_get_var(int ncid, hid_t varid, void* ip)
	void* data = (void*) malloc(type_size * total_data_count);
	memset(data, '\0', type_size * total_data_count);
	
	// get the data into a 1-d array
	hid_t dataset_type  = H5Dget_type(varid);
	hid_t dataset_space = H5Dget_space(varid);
	H5Dread(varid, dataset_type, dataset_space, H5S_ALL, H5P_DEFAULT, data);
	
	// .. do needed conversions to 'make it right'
	// TODO
		
	return (void*) data;
}

void* get_variable_data_by_name_dimalloc(const char* path, const char* group, const char* name) {
	if (DEBUG_HDF5_HELPER) printf("get_variable_data_by_name_dimalloc: %s %s %s \n", path, group, name);
	size_t *dims = (size_t*) get_variable_dims_by_name(path, group, name);		// NOTE! This is not returning int!!!
	
	size_t cols = dims[0];		// {X}
	free(dims);
	
	H5T_class_t type = get_variable_type_by_name(path, group, name);
	
	char				*data_char;
	signed char			*data_schar;
	unsigned char		*data_uchar;
	short				*data_short;
	unsigned short		*data_ushort;
	long				*data_int;
	unsigned			*data_unsigned;
	long				*data_long;
	unsigned long		*data_ulong;
	long long			*data_llong;
	unsigned long long	*data_ullong;
	float				*data_float;
	double				*data_double;
	long double			*data_ldouble;
	hsize_t				*data_hsize_t;
	hssize_t			*data_hssize_t;
	herr_t				*data_herr_t;
	hbool_t				*data_hbool_t;
	
	if (type == H5T_NATIVE_CHAR)  	data_char = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_SCHAR) 	data_schar = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_UCHAR) 	data_uchar = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_SHORT) 	data_short = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_USHORT) 	data_ushort = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_INT) 	data_int = get_variable_data_by_name(path, group, name);
	if (type == H5T_INTEGER)	 	data_int = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_UINT) 	data_unsigned = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LONG) 	data_long = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_ULONG) 	data_ulong = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LLONG) 	data_llong = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_ULLONG) 	data_ullong = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_FLOAT) 	data_float = get_variable_data_by_name(path, group, name);
	if (type == H5T_FLOAT) 			data_float = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_DOUBLE) 	data_double = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LDOUBLE) data_ldouble = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HSIZE) 	data_hsize_t = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HERR) 	data_herr_t = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HBOOL) 	data_hbool_t = get_variable_data_by_name(path, group, name);
	
	if (DEBUG_HDF5_HELPER) printf("data shape is: c %4lu\n", cols);
	
	void *dataout;

	if (type == H5T_NATIVE_CHAR)  	dataout = (void*) data_char;
	if (type == H5T_NATIVE_SCHAR) 	dataout = (void*) data_schar;
	if (type == H5T_NATIVE_UCHAR) 	dataout = (void*) data_uchar;
	if (type == H5T_NATIVE_SHORT) 	dataout = (void*) data_short;
	if (type == H5T_NATIVE_USHORT) 	dataout = (void*) data_ushort;
	if (type == H5T_NATIVE_INT) 	dataout = (void*) data_int;
	if (type == H5T_INTEGER)	 	dataout = (void*) data_int;
	if (type == H5T_NATIVE_UINT) 	dataout = (void*) data_unsigned;
	if (type == H5T_NATIVE_LONG) 	dataout = (void*) data_long;
	if (type == H5T_NATIVE_ULONG) 	dataout = (void*) data_ulong;
	if (type == H5T_NATIVE_LLONG) 	dataout = (void*) data_llong;
	if (type == H5T_NATIVE_ULLONG) 	dataout = (void*) data_ullong;
	if (type == H5T_NATIVE_FLOAT) 	dataout = (void*) data_float;
	if (type == H5T_FLOAT) 			dataout = (void*) data_float;
	if (type == H5T_NATIVE_DOUBLE) 	dataout = (void*) data_double;
	if (type == H5T_NATIVE_LDOUBLE) dataout = (void*) data_ldouble;
	if (type == H5T_NATIVE_HSIZE) 	dataout = (void*) data_hsize_t;
	if (type == H5T_NATIVE_HSSIZE) 	dataout = (void*) data_hssize_t;
	if (type == H5T_NATIVE_HERR) 	dataout = (void*) data_herr_t;
	if (type == H5T_NATIVE_HBOOL) 	dataout = (void*) data_hbool_t;
	
	return (void*) dataout;
}

void* get_variable_data_by_name_dimalloc2(const char* path, const char* group, const char* name) {
	if (DEBUG_HDF5_HELPER) printf("get_variable_data_by_name_dimalloc2: %s %s %s \n", path, group, name);
	size_t *dims = (size_t*) get_variable_dims_by_name(path, group, name);		// NOTE! This is not returning int!!!
	
	size_t rows = dims[0];		// {Y}
	size_t cols = dims[1];		// {X}
	
	free(dims);
	H5T_class_t type = get_variable_type_by_name(path, group, name);
	
	char				*data_char;
	signed char			*data_schar;
	unsigned char		*data_uchar;
	short				*data_short;
	unsigned short		*data_ushort;
	long				*data_int;
	unsigned			*data_unsigned;
	long				*data_long;
	unsigned long		*data_ulong;
	long long			*data_llong;
	unsigned long long	*data_ullong;
	float				*data_float;
	double				*data_double;
	long double			*data_ldouble;
	hsize_t				*data_hsize_t;
	hssize_t			*data_hssize_t;
	herr_t				*data_herr_t;
	hbool_t				*data_hbool_t;
	
	char				**data2d_char;
	signed char			**data2d_schar;
	unsigned char		**data2d_uchar;
	short				**data2d_short;
	unsigned short		**data2d_ushort;
	long				**data2d_int;
	unsigned			**data2d_unsigned;
	long				**data2d_long;
	unsigned long		**data2d_ulong;
	long long			**data2d_llong;
	unsigned long long	**data2d_ullong;
	float				**data2d_float;
	double				**data2d_double;
	long double			**data2d_ldouble;
	hsize_t				**data2d_hsize_t;
	hssize_t			**data2d_hssize_t;
	herr_t				**data2d_herr_t;
	hbool_t				**data2d_hbool_t;
	
	if (type == H5T_NATIVE_CHAR)  	data_char = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_SCHAR) 	data_schar = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_UCHAR) 	data_uchar = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_SHORT) 	data_short = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_USHORT) 	data_ushort = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_INT) 	data_int = get_variable_data_by_name(path, group, name);
	if (type == H5T_INTEGER)	 	data_int = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_UINT) 	data_unsigned = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LONG) 	data_long = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_ULONG) 	data_ulong = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LLONG) 	data_llong = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_ULLONG) 	data_ullong = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_FLOAT) 	data_float = get_variable_data_by_name(path, group, name);
	if (type == H5T_FLOAT) 			data_float = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_DOUBLE) 	data_double = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LDOUBLE) data_ldouble = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HSIZE) 	data_hsize_t = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HERR) 	data_herr_t = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HBOOL) 	data_hbool_t = get_variable_data_by_name(path, group, name);
	
	if (type == H5T_NATIVE_CHAR)  	data2d_char = dimalloc(sizeof(char), 2, rows, cols);
	if (type == H5T_NATIVE_SCHAR) 	data2d_schar = dimalloc(sizeof(signed char), 2, rows, cols);
	if (type == H5T_NATIVE_UCHAR) 	data2d_uchar = dimalloc(sizeof(unsigned char), 2, rows, cols);
	if (type == H5T_NATIVE_SHORT) 	data2d_short = dimalloc(sizeof(short), 2, rows, cols);
	if (type == H5T_NATIVE_USHORT) 	data2d_ushort = dimalloc(sizeof(unsigned short), 2, rows, cols);
	if (type == H5T_NATIVE_INT) 	data2d_int = dimalloc(sizeof(long), 2, rows, cols);
	if (type == H5T_INTEGER)	 	data2d_int = dimalloc(sizeof(long), 2, rows, cols);
	if (type == H5T_NATIVE_UINT) 	data2d_unsigned = dimalloc(sizeof(unsigned), 2, rows, cols);
	if (type == H5T_NATIVE_LONG) 	data2d_long = dimalloc(sizeof(long), 2, rows, cols);
	if (type == H5T_NATIVE_ULONG) 	data2d_ulong = dimalloc(sizeof(unsigned long), 2, rows, cols);
	if (type == H5T_NATIVE_LLONG) 	data2d_llong = dimalloc(sizeof(long long), 2, rows, cols);
	if (type == H5T_NATIVE_ULLONG) 	data2d_ullong = dimalloc(sizeof(unsigned long long), 2, rows, cols);
	if (type == H5T_NATIVE_FLOAT) 	data2d_float = dimalloc(sizeof(float), 2, rows, cols);
	if (type == H5T_FLOAT) 			data2d_float = dimalloc(sizeof(float), 2, rows, cols);
	if (type == H5T_NATIVE_DOUBLE) 	data2d_double = dimalloc(sizeof(double), 2, rows, cols);
	if (type == H5T_NATIVE_LDOUBLE) data2d_ldouble = dimalloc(sizeof(long double), 2, rows, cols);
	if (type == H5T_NATIVE_HSIZE) 	data2d_hsize_t = dimalloc(sizeof(hsize_t), 2, rows, cols);
	if (type == H5T_NATIVE_HSSIZE) 	data2d_hssize_t = dimalloc(sizeof(hssize_t), 2, rows, cols);
	if (type == H5T_NATIVE_HERR) 	data2d_herr_t = dimalloc(sizeof(herr_t), 2, rows, cols);
	if (type == H5T_NATIVE_HBOOL) 	data2d_hbool_t = dimalloc(sizeof(hbool_t), 2, rows, cols);
	
	if (DEBUG_HDF5_HELPER) printf("data shape is: r %4lu c %4lu\n", rows, cols);
	
	size_t col, row;
	for (row = 0; row < rows; row++) {
		for (col = 0; col < cols; col++) {
			size_t index = (row * cols) + col;
	
			if (type == H5T_NATIVE_CHAR)  	data2d_char[row][col] = data_char[index];
			if (type == H5T_NATIVE_SCHAR) 	data2d_schar[row][col] = data_schar[index];
			if (type == H5T_NATIVE_UCHAR) 	data2d_uchar[row][col] = data_uchar[index];
			if (type == H5T_NATIVE_SHORT) 	data2d_short[row][col] = data_short[index];
			if (type == H5T_NATIVE_USHORT) 	data2d_ushort[row][col] = data_ushort[index];
			if (type == H5T_NATIVE_INT) 	data2d_int[row][col] = data_int[index];
			if (type == H5T_INTEGER)	 	data2d_int[row][col] = data_int[index];
			if (type == H5T_NATIVE_UINT) 	data2d_unsigned[row][col] = data_unsigned[index];
			if (type == H5T_NATIVE_LONG) 	data2d_long[row][col] = data_long[index];
			if (type == H5T_NATIVE_ULONG) 	data2d_ulong[row][col] = data_ulong[index];
			if (type == H5T_NATIVE_LLONG) 	data2d_llong[row][col] = data_llong[index];
			if (type == H5T_NATIVE_ULLONG) 	data2d_ullong[row][col] = data_ullong[index];
			if (type == H5T_NATIVE_FLOAT) 	data2d_float[row][col] = data_float[index];
			if (type == H5T_FLOAT) 			data2d_float[row][col] = data_float[index];
			if (type == H5T_NATIVE_DOUBLE) 	data2d_double[row][col] = data_double[index];
			if (type == H5T_NATIVE_LDOUBLE) data2d_ldouble[row][col] = data_ldouble[index];
			if (type == H5T_NATIVE_HSIZE) 	data2d_hsize_t[row][col] = data_hsize_t[index];
			if (type == H5T_NATIVE_HSSIZE) 	data2d_hssize_t[row][col] = data_hssize_t[index];
			if (type == H5T_NATIVE_HERR) 	data2d_herr_t[row][col] = data_herr_t[index];
			if (type == H5T_NATIVE_HBOOL) 	data2d_hbool_t[row][col] = data_hbool_t[index];
		}
	}
	
	if (type == H5T_NATIVE_CHAR)  	free(data_char);
	if (type == H5T_NATIVE_SCHAR) 	free(data_schar);
	if (type == H5T_NATIVE_UCHAR) 	free(data_uchar);
	if (type == H5T_NATIVE_SHORT) 	free(data_short);
	if (type == H5T_NATIVE_USHORT) 	free(data_ushort);
	if (type == H5T_NATIVE_INT) 	free(data_int);
	if (type == H5T_INTEGER)	 	free(data_int);
	if (type == H5T_NATIVE_UINT) 	free(data_unsigned);
	if (type == H5T_NATIVE_LONG) 	free(data_long);
	if (type == H5T_NATIVE_ULONG) 	free(data_ulong);
	if (type == H5T_NATIVE_LLONG) 	free(data_llong);
	if (type == H5T_NATIVE_ULLONG) 	free(data_ullong);
	if (type == H5T_NATIVE_FLOAT) 	free(data_float);
	if (type == H5T_FLOAT) 			free(data_float);
	if (type == H5T_NATIVE_DOUBLE) 	free(data_double);
	if (type == H5T_NATIVE_LDOUBLE) free(data_ldouble);
	if (type == H5T_NATIVE_HSIZE) 	free(data_hsize_t);
	if (type == H5T_NATIVE_HSSIZE) 	free(data_hssize_t);
	if (type == H5T_NATIVE_HERR) 	free(data_herr_t);
	if (type == H5T_NATIVE_HBOOL) 	free(data_hbool_t);
	
	void *dataout;

	if (type == H5T_NATIVE_CHAR)  	dataout = (void*) data2d_char;
	if (type == H5T_NATIVE_SCHAR) 	dataout = (void*) data2d_schar;
	if (type == H5T_NATIVE_UCHAR) 	dataout = (void*) data2d_uchar;
	if (type == H5T_NATIVE_SHORT) 	dataout = (void*) data2d_short;
	if (type == H5T_NATIVE_USHORT) 	dataout = (void*) data2d_ushort;
	if (type == H5T_NATIVE_INT) 	dataout = (void*) data2d_int;
	if (type == H5T_INTEGER)	 	dataout = (void*) data2d_int;
	if (type == H5T_NATIVE_UINT) 	dataout = (void*) data2d_unsigned;
	if (type == H5T_NATIVE_LONG) 	dataout = (void*) data2d_long;
	if (type == H5T_NATIVE_ULONG) 	dataout = (void*) data2d_ulong;
	if (type == H5T_NATIVE_LLONG) 	dataout = (void*) data2d_llong;
	if (type == H5T_NATIVE_ULLONG) 	dataout = (void*) data2d_ullong;
	if (type == H5T_NATIVE_FLOAT) 	dataout = (void*) data2d_float;
	if (type == H5T_FLOAT) 			dataout = (void*) data2d_float;
	if (type == H5T_NATIVE_DOUBLE) 	dataout = (void*) data2d_double;
	if (type == H5T_NATIVE_LDOUBLE) dataout = (void*) data2d_ldouble;
	if (type == H5T_NATIVE_HSIZE) 	dataout = (void*) data2d_hsize_t;
	if (type == H5T_NATIVE_HSSIZE) 	dataout = (void*) data2d_hssize_t;
	if (type == H5T_NATIVE_HERR) 	dataout = (void*) data2d_herr_t;
	if (type == H5T_NATIVE_HBOOL) 	dataout = (void*) data2d_hbool_t;
	
	return (void*) dataout;
}

void* get_variable_data_by_name_dimalloc3(const char* path, const char* group, const char* name) {
	
	char				*data_char;
	signed char			*data_schar;
	unsigned char		*data_uchar;
	short				*data_short;
	unsigned short		*data_ushort;
	long				*data_int;
	unsigned			*data_unsigned;
	long				*data_long;
	unsigned long		*data_ulong;
	long long			*data_llong;
	unsigned long long	*data_ullong;
	float				*data_float;
	double				*data_double;
	long double			*data_ldouble;
	hsize_t				*data_hsize_t;
	hssize_t			*data_hssize_t;
	herr_t				*data_herr_t;
	hbool_t				*data_hbool_t;
	
	size_t *dims = (size_t*) get_variable_dims_by_name(path, group, name);		// NOTE! This is not returning int!!!
	
	size_t levs = dims[0];		// {Z}
	size_t rows = dims[1];		// {Y}
	size_t cols = dims[2];		// {X}
	
	free(dims);
	hid_t type = get_variable_type_by_name(path, group, name);
	
	if (type == H5T_NATIVE_CHAR)  	data_char     = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_SCHAR) 	data_schar    = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_UCHAR) 	data_uchar    = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_SHORT) 	data_short    = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_USHORT) 	data_ushort   = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_INT) 	data_int      = get_variable_data_by_name(path, group, name);
	if (type == H5T_INTEGER)	 	data_int      = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_UINT) 	data_unsigned = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LONG) 	data_long     = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_ULONG) 	data_ulong    = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LLONG) 	data_llong    = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_ULLONG) 	data_ullong   = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_FLOAT) 	data_float    = get_variable_data_by_name(path, group, name);
	if (type == H5T_FLOAT) 			data_float    = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_DOUBLE) 	data_double   = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_LDOUBLE) data_ldouble  = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HSIZE) 	data_hsize_t  = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HSSIZE) 	data_hssize_t = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HERR) 	data_herr_t   = get_variable_data_by_name(path, group, name);
	if (type == H5T_NATIVE_HBOOL) 	data_hbool_t  = get_variable_data_by_name(path, group, name);

	if (type == H5T_NATIVE_CHAR)  	return (void*) convert_1d_to_3d(data_char, type, levs, rows, cols);
	if (type == H5T_NATIVE_SCHAR) 	return (void*) convert_1d_to_3d(data_schar, type, levs, rows, cols);
	if (type == H5T_NATIVE_UCHAR) 	return (void*) convert_1d_to_3d(data_uchar, type, levs, rows, cols);
	if (type == H5T_NATIVE_SHORT) 	return (void*) convert_1d_to_3d(data_short, type, levs, rows, cols);
	if (type == H5T_NATIVE_USHORT) 	return (void*) convert_1d_to_3d(data_ushort, type, levs, rows, cols);
	if (type == H5T_NATIVE_INT) 	return (void*) convert_1d_to_3d(data_int, type, levs, rows, cols);
	if (type == H5T_INTEGER)	 	return (void*) convert_1d_to_3d(data_int, type, levs, rows, cols);
	if (type == H5T_NATIVE_UINT) 	return (void*) convert_1d_to_3d(data_unsigned, type, levs, rows, cols);
	if (type == H5T_NATIVE_LONG) 	return (void*) convert_1d_to_3d(data_long, type, levs, rows, cols);
	if (type == H5T_NATIVE_ULONG) 	return (void*) convert_1d_to_3d(data_ulong, type, levs, rows, cols);
	if (type == H5T_NATIVE_LLONG) 	return (void*) convert_1d_to_3d(data_llong, type, levs, rows, cols);
	if (type == H5T_NATIVE_ULLONG) 	return (void*) convert_1d_to_3d(data_ullong, type, levs, rows, cols);
	if (type == H5T_NATIVE_FLOAT) 	return (void*) convert_1d_to_3d(data_float, type, levs, rows, cols);
	if (type == H5T_FLOAT) 			return (void*) convert_1d_to_3d(data_float, type, levs, rows, cols);
	if (type == H5T_NATIVE_DOUBLE) 	return (void*) convert_1d_to_3d(data_double, type, levs, rows, cols);
	if (type == H5T_NATIVE_LDOUBLE) return (void*) convert_1d_to_3d(data_ldouble, type, levs, rows, cols);
	if (type == H5T_NATIVE_HSIZE) 	return (void*) convert_1d_to_3d(data_hsize_t, type, levs, rows, cols);
	if (type == H5T_NATIVE_HSSIZE) 	return (void*) convert_1d_to_3d(data_hssize_t, type, levs, rows, cols);
	if (type == H5T_NATIVE_HERR) 	return (void*) convert_1d_to_3d(data_herr_t, type, levs, rows, cols);
	if (type == H5T_NATIVE_HBOOL) 	return (void*) convert_1d_to_3d(data_hbool_t, type, levs, rows, cols);
	
	return (void*) NULL;
}

/*
int main() {

	size_t blocks = 2;
	size_t rows = 3;
	size_t cols = 4;

	float ***data3d_float = dimalloc(sizeof(float), 3, blocks, rows, cols);
	float   *data1d_float = NULL;
	
	int i, j, k;
	for (i = 0; i < blocks; i++) {
		for (j = 0; j < rows; j++) {
			for (k = 0; k < cols; k++) {
				data3d_float[i][j][k] = i * 10. + j + k / 10.;
			}
		}
	}
	
	
	for (i = 0; i < blocks; i++) {
		for (j = 0; j < rows; j++) {
			for (k = 0; k < cols; k++) {
				printf("[%d, %d, %d] = %f\n", i, j, k, data3d_float[i][j][k]);
			}
		}
	}
	
	data1d_float = (float   *) convert_3d_to_1d((void ***) data3d_float, NC_FLOAT, blocks, rows, cols);
	data3d_float = (float ***) convert_1d_to_3d((void *  ) data1d_float, NC_FLOAT, blocks, rows, cols);
	
	for (i = 0; i < blocks; i++) {
		for (j = 0; j < rows; j++) {
			for (k = 0; k < cols; k++) {
				printf("[%d, %d, %d] = %f\n", i, j, k, data3d_float[i][j][k]);
			}
		}
	}
	
	free(data3d_float);
}
*/

/*
int main() {
	//if (DEBUG_HDF5_HELPER) printf("main\n");
	
	char path[]  = "/data/MISR/l5ftl01.larc.nasa.gov/misrl2l3/MISR/MIL2ASAE.003/2017.01.01/MISR_AM1_AS_AEROSOL_P008_O090641_F13_0023.nc";
	char group[] = "/4.4_KM_PRODUCTS/";
	char name1[] = "Latitude";
	char name2[] = "Longitude";
	char name3[] = "Aerosol_Optical_Depth";
	char name4[] = "Aerosol_Optical_Depth_Uncertainty";
*/

/*
int main() {
	//if (DEBUG_HDF5_HELPER) printf("main\n");
	
	char path[]  = "/home/shawn.jaker/VIIRS_data/night/GDNBO/GDNBO_npp_d20180101_t0842208_e0848012_b32021_c20180101144801690315_noac_ops.h5";
	char group[] = "/All_Data/VIIRS-DNB-GEO_All/";
	char name1[] = "Latitude";
	char name2[] = "Longitude";
	char name3[] = "SatelliteZenithAngle";
	
	//double target_lat = 32.121
	//double target_lon = -110.912
	
	float** lon = NULL;
	float** lat = NULL;
	float** dat = NULL;
	//float** aou;
	
	lat = get_variable_data_by_name_dimalloc2(path, group, name1); 
	lon = get_variable_data_by_name_dimalloc2(path, group, name2);
	dat = get_variable_data_by_name_dimalloc2(path, group, name3);
	//aou = get_variable_data_by_name_dimalloc2(path, group, name4);
	
	size_t* dims = (size_t*) get_variable_dims_by_name(path, group, name1);
	
	size_t cols = (dims[0] != 0 ? dims[0] : 1); // {X}
	size_t rows = (dims[1] != 0 ? dims[1] : 1); // {Y}
	size_t levs = (dims[2] != 0 ? dims[2] : 1); // {Z}
	
	printf("Dims cols: %lu rows: %lu levels: %lu \n", cols, rows, levs);
	
	free(dims);
	
	if (TEST) {
		//size_t lev;
		size_t col, row;
		//for (lev = 0; lev < levs; lev++) {
			for (row = 0; row < rows; row++) {
				for (col = 0; col < cols; col++) {
					size_t index = (row * cols) + col;
					if (dat[row][col] > -9999) printf("%8lu, %8lu, %8lu, %4f, %4f, %4f\n", //, %4f\n",
						index,
						col,
						row,
						lon[row][col],
						lat[row][col],
						dat[row][col]
						//aou[row][col]
						);
				}
			}
		//}
	}
	
	free(lat);
	free(lon);
	free(dat);
	//free(aou);
	
	return 0;
}
*/
