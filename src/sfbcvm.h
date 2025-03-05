/**
 * @file sfbcvm.h
 * @brief Main header file for SFBCVM library.
 * @author - SCEC 
 * @version 1.0
 *
 * Delivers SF Bay Velocity Model
 *
 */

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// Constants
#ifndef M_PI
	/** Defines pi */
	#define M_PI 3.14159265358979323846
#endif

/** Defines a return value of success */
#define SUCCESS 0
/** Defines a return value of failure */
#define FAIL 1
/** Defines a return value of NA from model */
#define NA -1 

/* config string */
#define SFBCVM_CONFIG_MAX 1000

// Structures
/** Defines a point (latitude, longitude, and depth) in WGS84 format */
typedef struct sfbcvm_point_t {
	/** Longitude member of the point */
	double longitude;
	/** Latitude member of the point */
	double latitude;
	/** Depth member of the point */
	double depth;
} sfbcvm_point_t;

/** Defines the material properties this model will retrieve. */
typedef struct sfbcvm_properties_t {
	/** P-wave velocity in meters per second */
	double vp;
	/** S-wave velocity in meters per second */
	double vs;
	/** Density in g/m^3 */
	double rho;
        /** NOT USED from basic_property_t */
        double qp;
        /** NOT USED from basic_property_t */
        double qs;
} sfbcvm_properties_t;

/** The SFBCVM configuration structure. */
typedef struct sfbcvm_configuration_t {
	/** The zone of UTM projection */
	int utm_zone;
	/** The model directory */
	char model_dir[128];
	/** Number of x points */
	int nx;
	/** Number of y points */
	int ny;
	/** Number of z points */
	int nz;
	/** Depth in meters */
	double depth;
	/** Top left corner easting */
	double top_left_corner_lon;
	/** Top left corner northing */
	double top_left_corner_lat;
	/** Top right corner easting */
	double top_right_corner_lon;
	/** Top right corner northing */
	double top_right_corner_lat;
	/** Bottom left corner easting */
	double bottom_left_corner_lon;
	/** Bottom left corner northing */
	double bottom_left_corner_lat;
	/** Bottom right corner easting */
	double bottom_right_corner_lat;
	/** Bottom right corner northing */
	double bottom_right_corner_lon;
	/** Z interval for the data */
	double depth_interval;
        /** Bilinear or Trilinear Interpolation on or off (1 or 0) */
        int interpolation;

} sfbcvm_configuration_t;

/** The model structure which points to available portions of the model. */
typedef struct sfbcvm_model_t {
	/** A pointer to the Vp data either in memory or disk. Null if does not exist. */
	void *vp;
	/** Vp status: 0 = not found, 1 = found and not in memory, 2 = found and in memory */
	int vp_status;
} sfbcvm_model_t;

// Constants
/** The version of the model. */
const char *sfbcvm_version_string = "SFBCVM";

// Variables
/** Set to 1 when the model is ready for query. */
int sfbcvm_is_initialized = 0;

/** Location of the binary data files. */
char sfbcvm_data_directory[128];

/** Configuration parameters. */
sfbcvm_configuration_t *sfbcvm_configuration;
/** Holds pointers to the velocity model data OR indicates it can be read from file. */
sfbcvm_model_t *sfbcvm_velocity_model;

/** The height of this model's region, in meters. */
double sfbcvm_total_height_m = 0;
/** The width of this model's region, in meters. */
double sfbcvm_total_width_m = 0;

// UCVM API Required Functions

#ifdef DYNAMIC_LIBRARY

/** Initializes the model */
int model_init(const char *dir, const char *label);
/** Cleans up the model (frees memory, etc.) */
int model_finalize();
/** Returns version information */
int model_version(char *ver, int len);
/** Queries the model */
int model_query(sfbcvm_point_t *points, sfbcvm_properties_t *data, int numpts);

#endif

// SFBCVM Related Functions

/** Initializes the model */
int sfbcvm_init(const char *dir, const char *label);
/** Cleans up the model (frees memory, etc.) */
int sfbcvm_finalize();
/** Returns version information */
int sfbcvm_version(char *ver, int len);
/** Queries the model */
int sfbcvm_query(sfbcvm_point_t *points, sfbcvm_properties_t *data, int numpts);

// Non-UCVM Helper Functions
/** Reads the configuration file. */
int sfbcvm_read_configuration(char *file, sfbcvm_configuration_t *config);
void print_error(char *err);
/** Retrieves the value at a specified grid point in the model. */
void sfbcvm_read_properties(int x, int y, int z, sfbcvm_properties_t *data);
/** Attempts to malloc the model size in memory and read it in. */
int sfbcvm_try_reading_model(sfbcvm_model_t *model);
/** Calculates density from Vs. */
double sfbcvm_calculate_density(double vp);
/** Calculates Vs from Vp. */
double sfbcvm_calculate_vs(double vp);

// Interpolation Functions
/** Linearly interpolates two sfbcvm_properties_t structures */
void sfbcvm_linear_interpolation(double percent, sfbcvm_properties_t *x0, sfbcvm_properties_t *x1, sfbcvm_properties_t *ret_properties);
/** Bilinearly interpolates the properties. */
void sfbcvm_bilinear_interpolation(double x_percent, double y_percent, sfbcvm_properties_t *four_points, sfbcvm_properties_t *ret_properties);
/** Trilinearly interpolates the properties. */
void sfbcvm_trilinear_interpolation(double x_percent, double y_percent, double z_percent, sfbcvm_properties_t *eight_points,
							 sfbcvm_properties_t *ret_properties);
