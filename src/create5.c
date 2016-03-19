/** create an enntity of HDF5 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <hdf5.h>
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <error.h>
#include <ctype.h>
#include <hdf5.h>

#define GOOF(args...) { error(0, -1, args); goto cleanup; }

int decode_dset_name(char *dset_name, char *type, int *nels, int *strlen)
{
	syslog(LOG_DEBUG, "create_dataset");
	int status = -1; // assume failure
	char  *saveptr;
	/*
	for(j = 1, str1 = dset_name; ; j++ str1 = NULL){
		token = strtok_r(str1, ":", &saveptr);
		if(token == NULL) break:
		*/
	char *varname = strtok_r(dset_name, ":", &saveptr);
	if(varname == NULL || dset_name[0] == ':'){
		syslog(LOG_ERR, "ERR: varname invalid: <%s>", dset_name);
		goto cleanup;
	}

	char *type_spec = strtok_r(NULL, "\0", &saveptr);
	if(type_spec==0){
		error(0, -1,  "type spec is NULL");
		goto cleanup;
	}

	*type = type_spec[0];
	type_spec++;
	//int nels, strlen;
	switch(*type) {
		case 'd':
		case 'D' : 
			sscanf(type_spec, "%d", nels); break;
		case 's':
		case 'S' :
			sscanf(type_spec, "%dx%d", nels, strlen); break;
		default:
			  error(0, -1, "Dataset type unsupported: <%c>", type_spec[0]);
			  goto cleanup;
	}

	if(*nels == 0) GOOF("Cannot request 0-length array");
	*type = tolower(*type);
	if(*type == 's' && *strlen ==0) GOOF("Cannot request 0-width string");

	//printf("type_spec: <%s>\n", type_spec);
	
	status = 0; //success
cleanup:
	return status;
}
int create_dataset(hid_t loc_id, char *dset_name)
{
	int status = -1; //assume failure
	char type;
	int nels, strlen;
	status = decode_dset_name(dset_name, &type, &nels, &strlen);
	if(status !=0) GOOF("create_dataset had problems decoding dataset spec");

	//puts("TODO only handles double just now");

	hsize_t dims[1] = {nels};
	//dims[0] = ds.size();
	hid_t dspace = H5Screate_simple(1, dims, NULL);


	// TODO fix goofy creation for : create5 sharelock.h5 go7:S100/10
	hid_t dtype;
	switch(type){
		case 'd': dtype = H5Tcopy(H5T_NATIVE_DOUBLE); break;
		case 's': 
			  dtype = H5Tcopy(H5T_C_S1);
			  H5Tset_size(dtype, strlen);
			  break;
		default:
			  error(0, -1, "TODO unhandled type");
	}


	hid_t dset = H5Dcreate2(loc_id, dset_name, dtype, dspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);


	//hid_t dset = H5Dcreate2(loc_id, dset_name, H5T_NATIVE_DOUBLE, dspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	//H5Dwrite(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ds[0]);

	H5Dclose(dset);
	H5Sclose(dspace);


	printf("varname: <%s>, type <%c>, nels %d, strlen %d\n", dset_name, type, nels, strlen);
	status = 0;
cleanup:
	return status;
}

int main(int argc, char *argv[])
{
	openlog("hdf5-fuse", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_DEBUG, "Programm create5 starting");
	int status = EXIT_SUCCESS;
	static struct option longopts[] = {
		//{ "buffy",	     no_argument,	     NULL,	     'b' },
		//{ "fluoride",   required_argument,	     NULL,	     'f' },
		//{ "daggerset",  no_argument,	     &daggerset,     1 },
		{"help", no_argument, NULL, 'h'},
		{ NULL,	     0,			     NULL,	     0 }
	};

	int ch;
	while ((ch = getopt_long(argc, argv, "h", longopts, NULL)) != -1) { 
	switch(ch) {
		case 'h': printf("create5 FILE DATASET\n"); goto cleanup;
		case '?' : break; // getopt_long already printed an error message
		default: 
			  fprintf(stderr, "Didn't understand option '%c'. Exiting\n", ch); 
			  status= EXIT_FAILURE;
			  goto cleanup;
	}}

	if(optind+2 != argc ){
		fprintf(stderr, "Incorrect number of arguments. Existing\n");
		status = EXIT_FAILURE;
		goto cleanup;
	}

	char *h5fname = argv[optind];
	char *dataset_name = argv[optind+1];

	hid_t rid = H5Fopen(h5fname, H5F_ACC_RDWR, H5P_DEFAULT);
	status = create_dataset(rid, dataset_name);
	// TODO check status
	status = H5Fclose(rid);

	syslog(LOG_DEBUG, "File: %s, dataset %s\n", h5fname, dataset_name);

cleanup:
	//if(dset) free(dset);
	syslog(LOG_DEBUG, "Programme exiting with status %d", status);
	closelog();
	return status;
}
