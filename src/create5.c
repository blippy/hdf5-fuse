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
	syslog(LOG_DEBUG, "Programm create5 starting");
	int status = EXIT_SUCCESS;
	static struct option longopts[] = {
		//{ "buffy",	     no_argument,	     NULL,	     'b' },
		//{ "fluoride",   required_argument,	     NULL,	     'f' },
		//{ "daggerset",  no_argument,	     &daggerset,     1 },
		{"dset", required_argument, NULL, 'd'},
		{ NULL,	     0,			     NULL,	     0 }
	};

	char *dset = NULL; // dataset name
	int ch;
	while ((ch = getopt_long(argc, argv, "d:", longopts, NULL)) != -1) { 
	switch(ch) {
		case 'd': dset=strdup(optarg); break;
	}}

	//argc -= optind;
	//argv += optind;

	char *h5fname = argv[optind];
	if(h5fname == NULL) {
		fprintf(stderr, "No HDF5 file specified");
		status = EXIT_FAILURE;
		goto cleanup;
	}

	if(dset) {
		status = create_dataset(dset);
	} else	{
		status = create_new_file(h5fname);
	}

	//fprintf(stdout, "Dataset: %s, file %s\n", dset, h5fname);

cleanup:
	if(dset) free(dset);
	syslog(LOG_DEBUG, "Programme exiting with status %d", status);
	closelog();
	return status;
}
