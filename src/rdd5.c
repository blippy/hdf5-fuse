/* read input as doubles 
 *
 *
 * */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	freopen(NULL, "rb", stdin); // stdin need to be in binary mode, not text mode
	//assert(argc==2);
	//FILE *fp = fopen(argv[1], "rb");
	FILE *fp = stdin;
	//assert(fp);
	double d;
	while(fread(&d, 8, 1, fp)){ printf("%lf\n", d);}

	//printf("%lf\n", NAN);
	//fclose(fp);
	return EXIT_SUCCESS;
}
