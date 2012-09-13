#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffmancoding.h"

int main(int argc, char** argv)
{
	const char* input_filename = "foo.txt";
	const char* comp_filename = "foo.comp";
	const char* uncomp_filename = "foo.uncomp";

	printf("compressed %s to %s...\n", input_filename, comp_filename);
	hc_compress(input_filename, comp_filename);
	printf("uncompressed %s to %s...\n", comp_filename, uncomp_filename);
	hc_uncompress(comp_filename, uncomp_filename);

	return 0;
}

