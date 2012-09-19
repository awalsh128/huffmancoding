#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffmancoding.h"

int main(int argc, char** argv)
{
	const char* input_filename = "foo.txt";
	const char* comp_filename = "foo.comp";
	const char* uncomp_filename = "foo.uncomp";

	printf("compressing %s to %s...\n", input_filename, comp_filename);
	if (hc_compress(input_filename, comp_filename)) {
		printf("uncompressing %s to %s...\n", comp_filename, uncomp_filename);
		if (!hc_uncompress(comp_filename, uncomp_filename)) return -2;
	} else {
		return -1;
	}

	return 0;
}

