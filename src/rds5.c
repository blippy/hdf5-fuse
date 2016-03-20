/* read string dataset  and dump it out
 *
 *
 * */

//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
//#include <math.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <error.h>

// TOD this is a common op used in many files
#define GOOF(args...) {error(0, -1, args); goto cleanup;}
int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	int status = EXIT_FAILURE;
	assert(argc ==2);
	char *fname = argv[1];
	FILE *fp = fopen(fname, "rb");
	assert(fp);

	char attrbuf[100]; // an attribute buffer
	size_t attrbuflen = sizeof(attrbuf), size;

	// check that the file is a string type
	size = getxattr(fname, "@T", attrbuf, attrbuflen);
	attrbuf[size] = 0;
	if(size == 0) GOOF("Couldn't get type for file");
	if(attrbuf[0] != 'S') GOOF("Not a string dataset");

	// find the string width. We don't need to worry about number of elements
	size = getxattr(fname, "@W", attrbuf, attrbuflen);
	attrbuf[size] = 0;
	int width = atoi(attrbuf);

	// dump out the contents
	char *buf = alloca(width+1);
	if(buf == NULL) GOOF("Couldn't allocate a buffer to hold data");
	while(fread(buf, width, 1, fp)) puts(buf);

	status = EXIT_SUCCESS;
cleanup:
	fclose(fp);
	return status;
}
