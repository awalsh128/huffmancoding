#include <stdlib.h>
#include <stdio.h>

#include "bitio.h"

int main(int argc, char** argv)
{
	int count = 5;
	int lens[] = {11, 3, 8, 14, 5};
	unsigned char* b[count]; 
	breader_ptr reader = bio_reader_malloc("test.in");
	breader_ptr writer = bio_writer_malloc("test.out");

	for (int i = 0; i < count; i++) {
		b[i] = bio_read(reader, lens[i]);
	}

	for (int i = 0; i < count; i++) {
		bio_write(writer, b[i], lens[i]);
	}

	for (int i = 0; i < count; i++) {
		free(b[i]);
	}

	bio_reader_free(reader);
	bio_writer_free(writer);

	return 0;
}

