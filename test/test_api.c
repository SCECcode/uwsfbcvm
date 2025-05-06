/**
 * @file test_api.c
 * @brief Bootstraps the test framework for the UWSFBCVM library.
 * @author - SCEC
 * @version 1.0
 *
 * Tests the UWSFBCVM library by loading it and executing the code as
 * UCVM would do it.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "uwsfbcvm.h"

/**
 * Initializes and runs the test program. Tests link against the
 * static version of the library to prevent any dynamic loading
 * issues.
 *
 * @param argc The number of arguments.
 * @param argv The argument strings.
 * @return A zero value indicating success.
 */
int main(int argc, const char* argv[]) {

	// Declare the structures.
	uwsfbcvm_point_t pt;
	uwsfbcvm_properties_t ret;

	// Initialize the model.
        char *envstr=getenv("UCVM_INSTALL_PATH");
        if(envstr != NULL) {
            if (uwsfbcvm_init(envstr, "uwsfbcvm") != 0) {
                assert(1);
            }
            } else if (uwsfbcvm_init("..", "uwsfbcvm") != 0) {
                assert(1);
        }

	printf("Loaded the model successfully.\n");

	// Query a point.
	pt.longitude = -121.5514;
	pt.latitude = 37.2484;
	pt.depth = 0;

	uwsfbcvm_query(&pt, &ret, 1);

        //fprintf(stderr,"%f, %f, %f\n", ret.vs, ret.vp, ret.rho);
	assert(ret.vs > 0);
	assert(ret.vp > 0);
	assert(ret.rho > 0);

	printf("Query was successful.\n");

	// Close the model.
	assert(uwsfbcvm_finalize() == 0);

	printf("Model closed successfully.\n");

	printf("\nALL UWSFBCVM TESTS PASSED\n");

	return 0;
}
