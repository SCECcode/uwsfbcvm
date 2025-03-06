/**
 * @file uwsfbcvm.c
 * @brief Main file for UWSFBCVM library.
 * @author - SCEC 
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Delivers UWSFBCVM Velocity Model
 *
 */

#include "uwsfbcvm.h"

int uwsfbcvm_debug = 0;
FILE *stderrfp;

/** The config of the model */
char *uwsfbcvm_config_string=NULL;
int uwsfbcvm_config_sz=0;

/**
 * Initializes the UWSFBCVM plugin model within the UCVM framework. In order to initialize
 * the model, we must provide the UCVM install path and optionally a place in memory
 * where the model already exists.
 *
 * @param dir The directory in which UCVM has been installed.
 * @param label A unique identifier for the velocity model.
 * @return Success or failure, if initialization was successful.
 */
int uwsfbcvm_init(const char *dir, const char *label) {
    int tempVal = 0;
    char configbuf[512];

    if(uwsfbcvm_debug) {
        stderrfp = fopen("uwsfbcvm_debug.log", "w+");
        fprintf(stderrfp," ===== START ===== \n");
    }

    uwsfbcvm_config_string = calloc(UWSFBCVM_CONFIG_MAX, sizeof(char));
    uwsfbcvm_config_string[0]='\0';
    uwsfbcvm_config_sz=0;

    // Initialize variables.
    uwsfbcvm_configuration = calloc(1, sizeof(uwsfbcvm_configuration_t));
    uwsfbcvm_velocity_model = calloc(1, sizeof(uwsfbcvm_model_t));

    // Configuration file location.
    sprintf(configbuf, "%s/model/%s/data/config", dir, label);

    // Read the configuration file.
    if (uwsfbcvm_read_configuration(configbuf, uwsfbcvm_configuration) != SUCCESS) {
                print_error("No configuration file was found to read from.");
                return FAIL;
        }

    // Set up the data directory.
    sprintf(uwsfbcvm_data_directory, "%s/model/%s/data/%s", dir, label, uwsfbcvm_configuration->model_dir);

    // Can we allocate the model, or parts of it, to memory. If so, we do.
    tempVal = uwsfbcvm_try_reading_model(uwsfbcvm_velocity_model);

    if (tempVal == SUCCESS) {
    	fprintf(stderr, "WARNING: Could not load model into memory. Reading the model from the\n");
    	fprintf(stderr, "hard disk may result in slow performance.");
    } else if (tempVal == FAIL) {
    	print_error("No model file was found to read from.");
    	return FAIL;
    }

    // In order to simplify our calculations in the query, we want to rotate the box so that the bottom-left
    // corner is at (0m,0m). Our box's height is total_height_m and total_width_m. We then rotate the
    // point so that is is somewhere between (0,0) and (total_width_m, total_height_m). How far along
    // the X and Y axis determines which grid points we use for the interpolation routine.

    uwsfbcvm_total_height_m = sqrt(pow(uwsfbcvm_configuration->top_left_corner_lat - uwsfbcvm_configuration->bottom_left_corner_lat, 2.0f) +
    					  pow(uwsfbcvm_configuration->top_left_corner_lon - uwsfbcvm_configuration->bottom_left_corner_lon, 2.0f));
    uwsfbcvm_total_width_m  = sqrt(pow(uwsfbcvm_configuration->top_right_corner_lat - uwsfbcvm_configuration->top_left_corner_lat, 2.0f) +
    					  pow(uwsfbcvm_configuration->top_right_corner_lon - uwsfbcvm_configuration->top_left_corner_lon, 2.0f));

        /* setup config_string */
        sprintf(uwsfbcvm_config_string,"config = %s\n",configbuf);
        uwsfbcvm_config_sz=1;

    // Let everyone know that we are initialized and ready for business.
    uwsfbcvm_is_initialized = 1;

    return SUCCESS;
}

/**
 * Queries UWSFBCVM at the given points and returns the data that it finds.
 *
 * @param points The points at which the queries will be made.
 * @param data The data that will be returned (Vp, Vs, density, Qs, and/or Qp).
 * @param numpoints The total number of points to query.
 * @return SUCCESS or FAIL.
 */
int uwsfbcvm_query(uwsfbcvm_point_t *points, uwsfbcvm_properties_t *data, int numpoints) {
    int i = 0;
    int load_x_coord = 0, load_y_coord = 0, load_z_coord = 0;
    double x_percent = 0, y_percent = 0, z_percent = 0;
    uwsfbcvm_properties_t surrounding_points[8];
        double lon_e, lat_n;

    int zone = 11;
    int longlat2utm = 0;

        double delta_lon = (uwsfbcvm_configuration->top_right_corner_lon - uwsfbcvm_configuration->bottom_left_corner_lon)/(uwsfbcvm_configuration->nx - 1);
        double delta_lat = (uwsfbcvm_configuration->top_right_corner_lat - uwsfbcvm_configuration->bottom_left_corner_lat)/(uwsfbcvm_configuration->ny - 1);

    for (i = 0; i < numpoints; i++) {
    	lon_e = points[i].longitude; 
    	lat_n = points[i].latitude; 
//fprintf(stderr,">>>>>>>>>working on i >> %d <<<<<<<<< %lf %lf\n",i, lon_e, lat_n);

    	// Which point base point does that correspond to?
    	load_y_coord = (int)(round((lat_n - uwsfbcvm_configuration->bottom_left_corner_lat) / delta_lat));
    	load_x_coord = (int)(round((lon_e - uwsfbcvm_configuration->bottom_left_corner_lon) / delta_lon));
    	load_z_coord = (int)((points[i].depth)/1000);

//fprintf(stderr,"coord %d %d %d\n", load_x_coord, load_y_coord, load_z_coord);

    	// Are we outside the model's X and Y and Z boundaries?
    	if (points[i].depth > uwsfbcvm_configuration->depth || load_x_coord > uwsfbcvm_configuration->nx -1  || load_y_coord > uwsfbcvm_configuration->ny -1 || load_x_coord < 0 || load_y_coord < 0 || load_z_coord < 0) {
    		data[i].vp = -1;
    		data[i].vs = -1;
    		data[i].rho = -1;
    		continue;
    	}

    	// Get the X, Y, and Z percentages for the bilinear or trilinear interpolation below.
    	x_percent =fmod((lon_e - uwsfbcvm_configuration->bottom_left_corner_lon), delta_lon) /delta_lon;
    	y_percent = fmod((lat_n - uwsfbcvm_configuration->bottom_left_corner_lat), delta_lat)/
delta_lat;
    	z_percent = fmod(points[i].depth, uwsfbcvm_configuration->depth_interval) / uwsfbcvm_configuration->depth_interval;

//fprintf(stderr,"percent %lf %lf %lf\n", x_percent, y_percent, z_percent);
    	if (load_z_coord == 0 && z_percent == 0) {
    		// We're below the model boundaries. Bilinearly interpolate the bottom plane and use that value.
    		load_z_coord = 0;
                   if(uwsfbcvm_configuration->interpolation) {

    		// Get the four properties.
    		uwsfbcvm_read_properties(load_x_coord,     load_y_coord,     load_z_coord,     &(surrounding_points[0]));	// Orgin.
    		uwsfbcvm_read_properties(load_x_coord + 1, load_y_coord,     load_z_coord,     &(surrounding_points[1]));	// Orgin + 1x
    		uwsfbcvm_read_properties(load_x_coord,     load_y_coord + 1, load_z_coord,     &(surrounding_points[2]));	// Orgin + 1y
    		uwsfbcvm_read_properties(load_x_coord + 1, load_y_coord + 1, load_z_coord,     &(surrounding_points[3]));	// Orgin + x + y, forms top plane.

    		uwsfbcvm_bilinear_interpolation(x_percent, y_percent, surrounding_points, &(data[i]));
                  } else {
    		uwsfbcvm_read_properties(load_x_coord,     load_y_coord,     load_z_coord,     &(data[i]));	// Orgin.
                  }

    	} else {
    	  if( uwsfbcvm_configuration->interpolation) {
    		// Read all the surrounding point properties.
    		uwsfbcvm_read_properties(load_x_coord,     load_y_coord,     load_z_coord,     &(surrounding_points[0]));	// Orgin.
    		uwsfbcvm_read_properties(load_x_coord + 1, load_y_coord,     load_z_coord,     &(surrounding_points[1]));	// Orgin + 1x
    		uwsfbcvm_read_properties(load_x_coord,     load_y_coord + 1, load_z_coord,     &(surrounding_points[2]));	// Orgin + 1y
    		uwsfbcvm_read_properties(load_x_coord + 1, load_y_coord + 1, load_z_coord,     &(surrounding_points[3]));	// Orgin + x + y, forms top plane.
    		uwsfbcvm_read_properties(load_x_coord,     load_y_coord,     load_z_coord - 1, &(surrounding_points[4]));	// Bottom plane origin
    		uwsfbcvm_read_properties(load_x_coord + 1, load_y_coord,     load_z_coord - 1, &(surrounding_points[5]));	// +1x
    		uwsfbcvm_read_properties(load_x_coord,     load_y_coord + 1, load_z_coord - 1, &(surrounding_points[6]));	// +1y
    		uwsfbcvm_read_properties(load_x_coord + 1, load_y_coord + 1, load_z_coord - 1, &(surrounding_points[7]));	// +x +y, forms bottom plane.

    		uwsfbcvm_trilinear_interpolation(x_percent, y_percent, z_percent, surrounding_points, &(data[i]));
                    } else {
                        // no interpolation, data as it is
    		uwsfbcvm_read_properties(load_x_coord,     load_y_coord,     load_z_coord,     &(data[i]));	// Orgin.
                    }
    	}

           data[i].rho = uwsfbcvm_calculate_density(data[i].vp);
           data[i].vs = uwsfbcvm_calculate_vs(data[i].vp);
    }

    return SUCCESS;
}

/**
 * Retrieves the material properties (whatever is available) for the given data point, expressed
 * in x, y, and z co-ordinates.
 *
 * @param x The x coordinate of the data point.
 * @param y The y coordinate of the data point.
 * @param z The z coordinate of the data point.
 * @param data The properties struct to which the material properties will be written.
 */
void uwsfbcvm_read_properties(int x, int y, int z, uwsfbcvm_properties_t *data) {

    // Set everything to -1 to indicate not found.
    data->vp = -1;
    data->vs = -1;
    data->rho = -1;

    float *ptr = NULL;
    FILE *fp = NULL;

    int location = z * (uwsfbcvm_configuration->nx * uwsfbcvm_configuration->ny) + (y * uwsfbcvm_configuration->nx) + x;

    // Check our loaded components of the model.
    if (uwsfbcvm_velocity_model->vp_status == 2) {
    	// Read from memory.
    	ptr = (float *)uwsfbcvm_velocity_model->vp;
    	data->vp = ptr[location];
//fprintf(stderr,"XX read from location memory %d, %lf\n", location, data->vp);
    } else if (uwsfbcvm_velocity_model->vp_status == 1) {
    	// Read from file.
    	fseek(fp, location * sizeof(float), SEEK_SET);
    	fread(&(data->vp), sizeof(float), 1, fp);
//fprintf(stderr,"XX read from location file %d, %lf\n", location, data->vp);
    }
}

/**
 * Trilinearly interpolates given a x percentage, y percentage, z percentage and a cube of
 * data properties in top origin format (top plane first, bottom plane second).
 *
 * @param x_percent X percentage
 * @param y_percent Y percentage
 * @param z_percent Z percentage
 * @param eight_points Eight surrounding data properties
 * @param ret_properties Returned data properties
 */
void uwsfbcvm_trilinear_interpolation(double x_percent, double y_percent, double z_percent,
    						 uwsfbcvm_properties_t *eight_points, uwsfbcvm_properties_t *ret_properties) {
    uwsfbcvm_properties_t *temp_array = calloc(2, sizeof(uwsfbcvm_properties_t));
    uwsfbcvm_properties_t *four_points = eight_points;

    uwsfbcvm_bilinear_interpolation(x_percent, y_percent, four_points, &temp_array[0]);

    // Now advance the pointer four "uwsfbcvm_properties_t" spaces.
    four_points += 4;

    // Another interpolation.
    uwsfbcvm_bilinear_interpolation(x_percent, y_percent, four_points, &temp_array[1]);

    // Now linearly interpolate between the two.
    uwsfbcvm_linear_interpolation(z_percent, &temp_array[0], &temp_array[1], ret_properties);

    free(temp_array);
}

/**
 * Bilinearly interpolates given a x percentage, y percentage, and a plane of data properties in
 * origin, bottom-right, top-left, top-right format.
 *
 * @param x_percent X percentage.
 * @param y_percent Y percentage.
 * @param four_points Data property plane.
 * @param ret_properties Returned data properties.
 */
void uwsfbcvm_bilinear_interpolation(double x_percent, double y_percent, uwsfbcvm_properties_t *four_points, uwsfbcvm_properties_t *ret_properties) {

    uwsfbcvm_properties_t *temp_array = calloc(2, sizeof(uwsfbcvm_properties_t));

    uwsfbcvm_linear_interpolation(x_percent, &four_points[0], &four_points[1], &temp_array[0]);
    uwsfbcvm_linear_interpolation(x_percent, &four_points[2], &four_points[3], &temp_array[1]);
    uwsfbcvm_linear_interpolation(y_percent, &temp_array[0], &temp_array[1], ret_properties);

    free(temp_array);
}

/**
 * Linearly interpolates given a percentage from x0 to x1, a data point at x0, and a data point at x1.
 *
 * @param percent Percent of the way from x0 to x1 (from 0 to 1 interval).
 * @param x0 Data point at x0.
 * @param x1 Data point at x1.
 * @param ret_properties Resulting data properties.
 */
void uwsfbcvm_linear_interpolation(double percent, uwsfbcvm_properties_t *x0, uwsfbcvm_properties_t *x1, uwsfbcvm_properties_t *ret_properties) {

    ret_properties->vp  = (1 - percent) * x0->vp  + percent * x1->vp;
    ret_properties->vs  = (1 - percent) * x0->vs  + percent * x1->vs;
    ret_properties->rho = (1 - percent) * x0->rho + percent * x1->rho;
}

/**
 * Called when the model is being discarded. Free all variables.
 *
 * @return SUCCESS
 */
int uwsfbcvm_finalize() {
    if (uwsfbcvm_velocity_model->vp) free(uwsfbcvm_velocity_model->vp);

    free(uwsfbcvm_configuration);

    if(uwsfbcvm_debug) {
     fclose(stderrfp);
    }

    return SUCCESS;
}

/**
 * Returns the version information.
 *
 * @param ver Version string to return.
 * @param len Maximum length of buffer.
 * @return Zero
 */
int uwsfbcvm_version(char *ver, int len)
{
  int verlen;
  verlen = strlen(uwsfbcvm_version_string);
  if (verlen > len - 1) {
    verlen = len - 1;
  }
  memset(ver, 0, len);
  strncpy(ver, uwsfbcvm_version_string, verlen);
  return 0;
}

/**
 * Returns the model config information.
 *
 * @param key Config key string to return.
 * @param sz Number of config term to return.
 * @return Zero
 */
int uwsfbcvm_config(char **config, int *sz)
{
  int len=strlen(uwsfbcvm_config_string);
  if(len > 0) {
    *config=uwsfbcvm_config_string;
    *sz=uwsfbcvm_config_sz;
    return SUCCESS;
  }
  return FAIL;
}

/**
 * Reads the configuration file describing the various properties of CVM-S5 and populates
 * the configuration struct. This assumes configuration has been "calloc'ed" and validates
 * that each value is not zero at the end.
 *
 * @param file The configuration file location on disk to read.
 * @param config The configuration struct to which the data should be written.
 * @return Success or failure, depending on if file was read successfully.
 */
int uwsfbcvm_read_configuration(char *file, uwsfbcvm_configuration_t *config) {
    FILE *fp = fopen(file, "r");
    char key[40];
    char value[80];
    char line_holder[128];

    // If our file pointer is null, an error has occurred. Return fail.
    if (fp == NULL) {
    	print_error("Could not open the configuration file.");
    	return FAIL;
    }

    // Read the lines in the configuration file.
    while (fgets(line_holder, sizeof(line_holder), fp) != NULL) {
    	if (line_holder[0] != '#' && line_holder[0] != ' ' && line_holder[0] != '\n') {
    		sscanf(line_holder, "%s = %s", key, value);

    		// Which variable are we editing?
    		if (strcmp(key, "utm_zone") == 0)
      			config->utm_zone = atoi(value);
    		if (strcmp(key, "model_dir") == 0)
    			sprintf(config->model_dir, "%s", value);
    		if (strcmp(key, "nx") == 0)
      			config->nx = atoi(value);
    		if (strcmp(key, "ny") == 0)
      		 	config->ny = atoi(value);
    		if (strcmp(key, "nz") == 0)
      		 	config->nz = atoi(value);
    		if (strcmp(key, "depth") == 0)
      		 	config->depth = atof(value);
    		if (strcmp(key, "top_left_corner_lon") == 0)
    			config->top_left_corner_lon = atof(value);
    		if (strcmp(key, "top_left_corner_lat") == 0)
    	 		config->top_left_corner_lat = atof(value);
    		if (strcmp(key, "top_right_corner_lon") == 0)
    			config->top_right_corner_lon = atof(value);
    		if (strcmp(key, "top_right_corner_lat") == 0)
    			config->top_right_corner_lat = atof(value);
    		if (strcmp(key, "bottom_left_corner_lon") == 0)
    			config->bottom_left_corner_lon = atof(value);
    		if (strcmp(key, "bottom_left_corner_lat") == 0)
    			config->bottom_left_corner_lat = atof(value);
    		if (strcmp(key, "bottom_right_corner_lon") == 0)
    			config->bottom_right_corner_lon = atof(value);
    		if (strcmp(key, "bottom_right_corner_lat") == 0)
    			config->bottom_right_corner_lat = atof(value);
    		if (strcmp(key, "depth_interval") == 0)
    			config->depth_interval = atof(value);
    		if (strcmp(key, "interpolation") == 0) {
                                if (strcmp(value, "on") == 0) {
                                     config->interpolation = 1;
                                     } else {
                                          config->interpolation = 0;
                                }
                        };

    	}
    }

    // Have we set up all configuration parameters?
    if (config->utm_zone == 0 || config->nx == 0 || config->ny == 0 || config->nz == 0 || config->model_dir[0] == '\0' ||
    	config->top_left_corner_lon == 0 || config->top_left_corner_lat == 0 || config->top_right_corner_lon == 0 ||
    	config->top_right_corner_lat == 0 || config->bottom_left_corner_lon == 0 || config->bottom_left_corner_lat == 0 ||
    	config->bottom_right_corner_lon == 0 || config->bottom_right_corner_lat == 0 || config->depth == 0 ||
    	config->depth_interval == 0) {
    	print_error("One configuration parameter not specified. Please check your configuration file.");
    	return FAIL;
    }

    fclose(fp);

    return SUCCESS;
}

/**
 * Calculates the density based off of Vp. Base on Brocher's formulae
 *
 * @param vp 
 * @return Density, in g/m^3.
 * [eqn. 6] r (g/cm3) = 1.6612Vp – 0.4721Vp2 + 0.0671Vp3 – 0.0043Vp4 + 0.000106Vp5.
 * Equation 6 is the “Nafe-Drake curve” (Ludwig et al., 1970).
 * start with vp in km 
 */
double uwsfbcvm_calculate_density(double vp) {
     double retVal ;

     vp = vp * 0.001;
     double t1 = (vp * 1.6612);
     double t2 = ((vp * vp ) * 0.4721);
     double t3 = ((vp * vp * vp) * 0.0671);
     double t4 = ((vp * vp * vp * vp) * 0.0043);
     double t5 = ((vp * vp * vp * vp * vp) * 0.000106);
     retVal = t1 - t2 + t3 - t4 + t5;
     if (retVal < 1.0) {
       retVal = 1.0;
     }
     retVal = retVal * 1000.0;
     return retVal;
}

/**
 * Calculates the vs based off of Vp. Base on Brocher's formulae
 *
 * https://pubs.usgs.gov/of/2005/1317/of2005-1317.pdf
 *
 * @param vp
 * @return Vs, in km.
 * Vs derived from Vp, Brocher (2005) eqn 1.
 * [eqn. 1] Vs (km/s) = 0.7858 – 1.2344Vp + 0.7949Vp2 – 0.1238Vp3 + 0.0064Vp4.
 * Equation 1 is valid for 1.5 < Vp < 8 km/s.
 */
double uwsfbcvm_calculate_vs(double vp) {
     double retVal ;

     vp = vp * 0.001;
     double t1= (vp * 1.2344);
     double t2= ((vp * vp)* 0.7949); 
     double t3= ((vp * vp * vp) * 0.1238);
     double t4= ((vp * vp * vp * vp) * 0.0064);
     retVal = 0.7858 - t1 + t2 - t3 + t4;
     retVal = retVal * 1000.0;

     return retVal;
}


/**
 * Prints the error string provided.
 *
 * @param err The error string to print out to stderr.
 */
void print_error(char *err) {
    fprintf(stderr, "An error has occurred while executing UWSFBCVM. The error was:\n\n");
    fprintf(stderr, "%s", err);
    fprintf(stderr, "\n\nPlease contact software@scec.org and describe both the error and a bit\n");
    fprintf(stderr, "about the computer you are running UWSFBCVM on (Linux, Mac, etc.).\n");
}

/**
 * Tries to read the model into memory.
 *
 * @param model The model parameter struct which will hold the pointers to the data either on disk or in memory.
 * @return 2 if all files are read to memory, SUCCESS if file is found but at least 1
 * is not in memory, FAIL if no file found.
 */
int uwsfbcvm_try_reading_model(uwsfbcvm_model_t *model) {
    double base_malloc = uwsfbcvm_configuration->nx * uwsfbcvm_configuration->ny * uwsfbcvm_configuration->nz * sizeof(float);
    int file_count = 0;
    int all_read_to_memory = 1;
    char current_file[128];
    FILE *fp;

    // Let's see what data we actually have.
    sprintf(current_file, "%s/vp.dat", uwsfbcvm_data_directory);
    if (access(current_file, R_OK) == 0) {
    	model->vp = malloc(base_malloc);
    	if (model->vp != NULL) {
    		// Read the model in.
    		fp = fopen(current_file, "rb");
    		fread(model->vp, 1, base_malloc, fp);
    		fclose(fp);
    		model->vp_status = 2;
    	} else {
    		all_read_to_memory = 0;
    		model->vp = fopen(current_file, "rb");
    		model->vp_status = 1;
    	}
    	file_count++;
    }

    if (file_count == 0)
    	return FAIL;
        else if (all_read_to_memory == 0)
                return SUCCESS;
        else
                return 2;
}

// The following functions are for dynamic library mode. If we are compiling
// a static library, these functions must be disabled to avoid conflicts.
#ifdef DYNAMIC_LIBRARY

/**
 * Init function loaded and called by the UCVM library. Calls uwsfbcvm_init.
 *
 * @param dir The directory in which UCVM is installed.
 * @return Success or failure.
 */
int model_init(const char *dir, const char *label) {
    return uwsfbcvm_init(dir, label);
}

/**
 * Query function loaded and called by the UCVM library. Calls uwsfbcvm_query.
 *
 * @param points The basic_point_t array containing the points.
 * @param data The basic_properties_t array containing the material properties returned.
 * @param numpoints The number of points in the array.
 * @return Success or fail.
 */
int model_query(uwsfbcvm_point_t *points, uwsfbcvm_properties_t *data, int numpoints) {
    return uwsfbcvm_query(points, data, numpoints);
}

/**
 * Finalize function loaded and called by the UCVM library. Calls uwsfbcvm_finalize.
 *
 * @return Success
 */
int model_finalize() {
    return uwsfbcvm_finalize();
}

/**
 * Version function loaded and called by the UCVM library. Calls uwsfbcvm_version.
 *
 * @param ver Version string to return.
 * @param len Maximum length of buffer.
 * @return Zero
 */
int model_version(char *ver, int len) {
    return uwsfbcvm_version(ver, len);
}

/** 
 * Version function loaded and called by the UCVM library. Calls uwsfbcvm_config.
 *                  
 * @param ver Config string to return.
 * @param sz sz of configs.
 * @return Zero
 */
int model_config(char **config, int *sz) {
        return uwsfbcvm_config(config, sz);
}


int (*get_model_init())(const char *, const char *) {
        return &uwsfbcvm_init;
}
int (*get_model_query())(uwsfbcvm_point_t *, uwsfbcvm_properties_t *, int) {
         return &uwsfbcvm_query;
}
int (*get_model_finalize())() {
         return &uwsfbcvm_finalize;
}
int (*get_model_version())(char *, int) {
         return &uwsfbcvm_version;
}
int (*get_model_config())(char **, int*) {
         return &uwsfbcvm_config;
}



#endif
