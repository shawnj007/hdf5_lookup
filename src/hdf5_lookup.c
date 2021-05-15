/*
 *	Program: HDF5 Lookup v0.1
 *
 *	Programmer: Shawn L. Jaker
 *
 *	Usage: This code is free to use and modify, so long
 *		as this header remains intact, except for
 *		"Modifications" below.
 *
 *	Purpose: Extract chosen variable data using native
 *		coordinates of file in a brute force approach
 *		to geo-location with a great circle calculation.
 *
 *	USER CONSTANT TO SET (below):
 *		MAX_GOOD_DIS_KM - determines the maximum distance that will 'match'
 *
 *	COMPILE:
 *		$ cd src
 *		$ make
 *		$ cd ..
 *
 *	Run:
 *		$ ./hdf5_lookup /FILE/path /var1/path {/var2/path {...}} /lat/path /lon/path target_lat target_lon
 *
 *	Parameters:
 *	 	/FILE/path			File system path of file to open
 *	 	/var1/path			variable path inside the file
 *	 	{/var2/path {...}}	optional variables (added output columns)
 *	 	/lat/path			latitude table path insid the file
 *	 	/lon/path			longitude table path inside the file
 *	 	target_lat			latitude value for search
 *	 	target_lon			longitude value for search
 *
 *	Returns: To standard out...
 *		target_latitude			(from command line)
 *		target_longitude		(from command line)
 *		distance_in_km			(calculated, great circle distance in kilometers)
 *		observation_latitude	(found in nc file)
 *		observation_longitude	(found in nc file)
 *		variable_1_value		(found in nc file)
 *		...
 *		variable_n_value
 *
 *	Modifications:
 *		2021 04 29 - Initial Version
 *		2021 04 29 - Modified to open hdf5 files instead of misr
 *		2021 04 30 - Added 'autoscale' to nearest for undersized datasets
 
 Command:
 
  $ ./hdf5_lookup File VarTable1 {VarTable2} LatTable LonTable target_lat target_lon
 
 Example:
 
  $ ./hdf5_lookup ~/VIIRS_data/night/GDNBO/GDNBO_npp_d20180101_t0859256_e0905060_b32021_c20180101150505919989_noac_ops.h5 /All_Data/VIIRS-DNB-GEO_All/SatelliteZenithAngle /All_Data/VIIRS-DNB-GEO_All/SatelliteZenithAngle /All_Data/VIIRS-DNB-GEO_All/Latitude /All_Data/VIIRS-DNB-GEO_All/Longitude 32.13 -111.09
 
 Parameters:
  File					file name to open
  
  VarTable{1..n}		variable(s) to decode (using full internal path)
  LatTable				Latitude table (using full internal path)
  LonTable				Longitude table (using full internal path)

  target_lat			latitude to find
  target_lon			longitude to find
 
 *
 *
 *
 */

#include "hdf5_lookup.h"

#define DEBUG_HDF5_LOOKUP 1

#define MAX_GOOD_DIS_KM 15

double gc_distance(double lat_0, double lon_0, double lat_1, double lon_1) {
	double pi = M_PI;
	double lat_s = lat_0 * pi / 180.;
	double lon_s = lon_0 * pi / 180.;
	double lat_e = lat_1 * pi / 180.;
	double lon_e = lon_1 * pi / 180.;
	double dLon = lon_e - lon_s;
	double dLat = lat_e - lat_s;
	double a = pow(sin(dLat/2.),2.) + cos(lat_s) * cos(lat_e) * pow(sin(dLon/2.),2.);
	double c = 2. * asin(sqrt(a));
	return c * 6367.; // km
}
 
void* get_indices_from_lat_long(float** data_lat, float** data_lon, double target_lat, double target_lon, int rows, int cols) {
	
	double closest = 99999;
	
	// Start in the center of the data
	int closest_col = -9999;
	int closest_row = -9999;
	
	int found = 0;
	
	do {
			
		int tmp_col = closest_col;
		int tmp_row = closest_row;
		
		int col, row;
		
		for(row = 0; row < rows; row++) {
			for(col = 0; col < cols; col++) {
				
				float lat = data_lat[row][col];
				float lon = data_lon[row][col];
				
				if ((lat > -9999) && (lon > -9999)) {
				
					double distance = gc_distance(lat, lon, target_lat, target_lon);
					
					// if any of those are closer to target then move there and repeat
					if (closest > distance) {
					
						if (DEBUG_HDF5_LOOKUP) printf("%10d ", row * cols + col);
						if (DEBUG_HDF5_LOOKUP) printf("%11.4f %11.4f ", lat, lon);
						if (DEBUG_HDF5_LOOKUP) printf("%4d %4d ", row, col);
						if (DEBUG_HDF5_LOOKUP) printf("%8.3e ", distance);
						if (DEBUG_HDF5_LOOKUP) printf("\n");

						tmp_col = col;
						tmp_row = row;
						closest = distance;
					}
				}
			}
		}

		// if no neighbors are closer, then closest one has been found
		if ((closest_col == tmp_col) && (closest_row == tmp_row)) {
			found = 1;
		} else {
			closest_col = tmp_col;
			closest_row = tmp_row;
		}

	// Repeat until the walk ends
	} while (found == 0);
	
	int* ret_vals = malloc(sizeof(int) * 3);
	
	ret_vals[0] = closest_row;
	ret_vals[1] = closest_col;
	ret_vals[2] = closest;
	
	return (void*) ret_vals;
}

void usage(int argc, char** argv) {

	printf("\n%s File/Path Group1/VarTable1 {Group2/VarTable2 {...}} LatGroup/LatTable LonGroup/LonTable target_lat target_lon\n\n", argv[0]);
	
	printf("EXAMPLE:\n");
	printf("./viirs_lookup ~/VIIRS_data/night/GDNBO/GDNBO_npp_d20180101_t0859256_e0905060_b32021_c20180101150505919989_noac_ops.h5 /All_Data/VIIRS-DNB-GEO_All/SatelliteZenithAngle /All_Data/VIIRS-DNB-GEO_All/SatelliteZenithAngle /All_Data/VIIRS-DNB-GEO_All/Latitude /All_Data/VIIRS-DNB-GEO_All/Longitude 32.13 -111.09\n");

	// ~/VIIRS_data/night/GDNBO/GDNBO_npp_d20180101_t0842208_e0848012_b32021_c20180101144801690315_noac_ops.h5
	// /All_Data/VIIRS-DNB-GEO_All/Latitude
	// /All_Data/VIIRS-DNB-GEO_All/Longitude
	// /All_Data/VIIRS-DNB-GEO_All/MidTime
}

int main (int argc, char** argv) {
	// Parse input line and verify file exists
	
	// ./hdf5_lookup File VarTable1 {VarTable2} LatTable LonTable target_lat target_lon
	// TODO ./hdf5_lookup VarFile1:VarTable1 {VarFile2:VarTable2} LatFile:LatTable LonFile:LonTable target_lat target_lon
	
	if (argc < 7) {
		printf("Too few arguments\n");
		usage(argc, argv);
		return 1;
	} // else {
		//printf("have %d args\n", argc);
	// }
	
	char* path = argv[1];

	char** variables = (char**) malloc(sizeof(char*) * (argc - 6));
	int i;
	for (i = 0; i < argc - 6; i++) {
		variables[i] = argv[i + 2];
	}

	char* lat_table = argv[argc - 4];
	char* lon_table = argv[argc - 3];
	float lat = atof(argv[argc - 2]);
	float lon = atof(argv[argc - 1]);
	
	/*********/

	char* group_lat_end = strrchr(lat_table, (int) '/');
	int group_lat_len = group_lat_end - lat_table + 1;
	char* group_lat = malloc(sizeof(char) * (group_lat_len + 1));
	strncpy(group_lat, lat_table, group_lat_len);
	group_lat[group_lat_len] = 0;
	
	long name_lat_len = strlen(lat_table) - group_lat_len;
	char* name_lat = malloc(sizeof(char) * (name_lat_len + 1));
	strncpy(name_lat, lat_table + group_lat_len, name_lat_len);
	name_lat[name_lat_len] = 0;
	
	/*
	*/
	if (DEBUG_HDF5_LOOKUP) printf("lat path:  %s\n", path);
	if (DEBUG_HDF5_LOOKUP) printf("lat group: %s\n", group_lat);
	if (DEBUG_HDF5_LOOKUP) printf("lat name:  %s\n", name_lat);
	
	float** data_lat = get_variable_data_by_name_dimalloc2(path, group_lat, name_lat);

	/*********/	

	char* group_lon_end = strrchr(lon_table, (int) '/');
	int group_lon_len = group_lon_end - lon_table + 1;
	char* group_lon = malloc(sizeof(char) * (group_lon_len + 1));
	strncpy(group_lon, lon_table, group_lon_len);
	group_lon[group_lon_len] = 0;
	
	long name_lon_len = strlen(lon_table) - group_lon_len;
	char* name_lon = malloc(sizeof(char) * (name_lon_len + 1));
	strncpy(name_lon, lon_table + group_lon_len, name_lon_len);
	name_lon[name_lon_len] = 0;

	/*
	*/
	if (DEBUG_HDF5_LOOKUP) printf("lon path:  %s\n", path);
	if (DEBUG_HDF5_LOOKUP) printf("lon group: %s\n", group_lon);
	if (DEBUG_HDF5_LOOKUP) printf("lon name:  %s\n", name_lon);
	
	float** data_lon = get_variable_data_by_name_dimalloc2(path, group_lon, name_lon);
	
	size_t *ll_dims = (size_t*) get_variable_dims_by_name(path, group_lon, name_lon);

	/*********/	
	
	if (DEBUG_HDF5_LOOKUP) printf("Finding target\n");

	int rows = ll_dims[0];
	int cols = ll_dims[1];

	int* indices = (int*) get_indices_from_lat_long(data_lat, data_lon, lat, lon, rows, cols);

	int row = indices[0];
	int col = indices[1];

	/*********/

	if ((row >= 0 && row < rows) && (col >= 0 && col < cols)) {
		float latitude = data_lat[row][col];
		float longitude = data_lon[row][col];
		double distance = gc_distance(latitude, longitude, lat, lon);
		
		if (DEBUG_HDF5_LOOKUP) printf("%f %f %f\n", latitude, longitude, distance);
		
		if (distance < MAX_GOOD_DIS_KM) {

			printf("%10.6f %10.6f %6.4f %10.6f %10.6f",
				lat,
				lon,
				distance,
				data_lat[row][col],
				data_lon[row][col]);
			
			for (i = 0; i < argc - 6; i++) {
			
				char* dat_table = variables[i];
				
				char* group_dat_end = strrchr(dat_table, (int) '/');
				int group_dat_len = group_dat_end - dat_table + 1;
				char* group_dat = malloc(sizeof(char) * (group_dat_len + 1));
				strncpy(group_dat, dat_table, group_dat_len);
				group_dat[group_dat_len] = 0;
				
				long name_dat_len = strlen(dat_table) - group_dat_len;
				char* name_dat = malloc(sizeof(char) * (name_dat_len + 1));
				strncpy(name_dat, dat_table + group_dat_len, name_dat_len);
				name_dat[name_dat_len] = 0;
				
				/*
				*/
				if (DEBUG_HDF5_LOOKUP) printf("dat path:  %s\n", path);
				if (DEBUG_HDF5_LOOKUP) printf("dat group: %s\n", group_dat);
				if (DEBUG_HDF5_LOOKUP) printf("dat name:  %s\n", name_dat);
				
				H5T_class_t data_type = get_variable_type_by_name(path, group_dat, name_dat);
				size_t *dims = (size_t*) get_variable_dims_by_name(path, group_dat, name_dat);
				
				unsigned long* data_lng;
				float** data_flt;
				
				if (DEBUG_HDF5_LOOKUP) printf("data_type is: %d %d %d\n", data_type, H5T_INTEGER, H5T_FLOAT);
				
				if (data_type == H5T_INTEGER) {				
					data_lng = get_variable_data_by_name_dimalloc(path, group_dat, name_dat);
				} else if (data_type == H5T_FLOAT) {
					data_flt = get_variable_data_by_name_dimalloc2(path, group_dat, name_dat);
				}
				
				if (DEBUG_HDF5_LOOKUP) printf(" %d %d %d\n", (int) dims[0], (int) dims[1], (int) dims[2]);
				
				/*
				int x, y;
				
				for(y = 0; y < rows; y++) { 			// rows
					for(x = 0; x < cols; x++) {			// columns
					
						float lat = data_lat[x][y];
						float lon = data_lon[x][y];
						float dat = data_dat[x][y];
						
						if ((lat > -9999) && (lon > -9999) & (dat > -9999)) {
							
							printf("%4.10f %4.10f %4.10f\n", lat, lon, dat);
						}
					}
				}
				*/
				
				int ll_row = row;
				int ll_col = col;
				
				int row_count = dims[0];
				int col_count = dims[1];
				if (DEBUG_HDF5_LOOKUP) printf(" r c = %d %d\n", row_count, col_count);
				
				int ll_row_count = ll_dims[0];
				int ll_col_count = ll_dims[1];
				if (DEBUG_HDF5_LOOKUP) printf(" r c = %d %d\n", ll_row_count, ll_col_count);
				
				free(dims);
				
				int new_row = (int) ((float) ll_row * ((float) row_count / (float) ll_row_count));
				int new_col = (int) ((float) ll_col * ((float) col_count / (float) ll_col_count));

				if (DEBUG_HDF5_LOOKUP) printf(" r c = %d %d\n", new_row, new_col);

				if (data_type == H5T_INTEGER) {				
					//unsigned long data_out = data_lng[new_row][new_col];
					unsigned long data_out = data_lng[new_row];
					printf(" %lu", data_out);
				} else if (data_type == H5T_FLOAT) {
					float data_out = data_flt[new_row][new_col];
					printf(" %f", data_out);
				}
				//printf("target lat,lon[%10.6f, %10.6f] is [%6.4f] km to x,y[%4d, %4d] = lat,lon[%10.6f, %10.6f] => val %e \n",

				// write target data to stdout
				//printf(" %15.13e\n", (double) data_out);
				
				free(group_dat);
				free(name_dat);
				
				if (data_type == H5T_INTEGER) {				
					free(data_lng);
				} else if (data_type == H5T_FLOAT) {
					free(data_flt);
				}
				
				//free(data_dat);
			}
			//for each variable
			// cycle through data out values as columns on stdout
			printf("\n");
		}
	}
	// Return the requested variables as series of columns
	
	free(indices);

	free(group_lat);
	free(name_lat);
	free(data_lat);
	
	free(group_lon);
	free(name_lon);
	free(data_lon);
	
	free(variables);
	
	return 0;
}
