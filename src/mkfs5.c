/** create an enntity of HDF5 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <hdf5.h>
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>

int main(int argc, char *argv[])
{
	openlog("hdf5-fuse", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_DEBUG, "Programm mkfs5 starting");
	int status = EXIT_SUCCESS;

	if(argc != 2){
		fprintf(stderr, "Usage: mkfs5 FILE\n");
		status = EXIT_FAILURE;
		goto cleanup;
	}

	// I'm trying to turn off the error stack, but it doesn't seem to work
	hid_t error_stack = H5Eget_current_stack();
	H5Eset_auto2(error_stack, NULL, NULL); // turn the error stack off

	hid_t fid = H5Fcreate(argv[1], H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
	if(fid<0) {
		fprintf(stderr, "Could not create file %s\n", argv[1]);
		status = EXIT_FAILURE;
		goto cleanup;
	}



	status = H5Fclose(fid);
cleanup:
	syslog(LOG_DEBUG, "Programme exiting with status %d", status);
	closelog();
	return status;
}
