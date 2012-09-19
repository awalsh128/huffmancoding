/* author: Andrew Walsh (awalsh128@gmail.com) */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitio.h"

#define LOW_MASK(n) ((1 << n) - 1)
#define HI_MASK(n) (LOW_MASK(n) << (CHAR_BIT - n))

struct bitfile
{
	FILE* file_ptr;
	unsigned char buffer;
	int buffer_len;
};

static void _delete(struct bitfile* file)
{
	fclose(file->file_ptr);
	free(file);
}

static struct bitfile* _new(const char* filename, const char* file_mode)
{
	struct bitfile* file = malloc(sizeof(struct bitfile));
	file->file_ptr = fopen(filename, file_mode);
	file->buffer = 0x00;
	file->buffer_len = 0;

	return file;
}

int bio_byte_size(int n)
{
	int m = n / CHAR_BIT;
	if ((n % CHAR_BIT) > 0) m++;
	return m;
}

int bio_eof(breader_ptr reader)
{
	char c = fgetc(reader->file_ptr);
	ungetc(c, reader->file_ptr);

	return (c == EOF);
}

FILE* bio_file_ptr(breader_ptr reader)
{
	return reader->file_ptr;
}

void bio_print(unsigned char* bits, int bit_len)
{
   int j, k;
   for (int i = (bit_len - 1); i >= 0; i--) {
      j = i / 8;
      k = i % 8;
      if ((i != 0) && (k == 0)) printf(" ");
		printf("%d", (bits[j] & (0x80 >> k)) > 0 ? 1 : 0);
   }
}

unsigned char* bio_read(breader_ptr reader, int read_len)
{
	int out_len, out_size;
	int fout_len, fout_size;
	int total_len, total_size;

	int block_head_len, block_tail_len;
	unsigned char block_head;

	unsigned char* out;
	unsigned char* fout;

	int rem_len;
	
	out_len = read_len;
	out_size = bio_byte_size(out_len);

	fout_len = out_len - reader->buffer_len;
	fout_size = bio_byte_size(fout_len);
	fout_len = fout_size * CHAR_BIT;

	total_len = reader->buffer_len + fout_len;
	total_size = bio_byte_size(total_len);

	rem_len = out_len % CHAR_BIT;

	out = malloc(sizeof(unsigned char) * out_size);

	if (read_len < reader->buffer_len) {

		// case where no bits need to be read
		out[0] = reader->buffer & HI_MASK(out_len);
		reader->buffer <<= out_len;
		reader->buffer_len -= out_len;

	} else if (reader->buffer_len == 0) {

		// case where buffer is empty; read bits will already be byte aligned to output
		assert(fread(out, 1, out_size, reader->file_ptr) == out_size);	// check against underflow
		reader->buffer = out[out_size - 1] << rem_len;
		reader->buffer_len = fout_len - out_len;
		if (rem_len > 0) out[out_size - 1] &= HI_MASK(rem_len);

	} else {

		// case where buffer is populated; read bites will need to be shifted
		fout = malloc(sizeof(unsigned char) * fout_size);
		assert(fread(fout, 1, fout_size, reader->file_ptr) == fout_size);

		block_head = reader->buffer;
		block_head_len = reader->buffer_len;
		block_tail_len = CHAR_BIT - block_head_len;
		for (int i = 0; i < fout_size; i++) {
			out[i] = block_head | fout[i] >> block_head_len;
			block_head = fout[i] << block_tail_len;
		}

		if (total_size > out_size) {

			// case where file tail bits are on a different block		
			if (rem_len > 0) {
				out[out_size - 1] &= HI_MASK(rem_len);		// trim last output block
				rem_len = rem_len - block_head_len;
			} else {
				rem_len = CHAR_BIT - block_head_len;
			}
			reader->buffer = fout[fout_size - 1] << rem_len;
			reader->buffer_len = CHAR_BIT - rem_len;

		} else {

			// case where file tail bits are on the same block
			out[out_size - 1] = block_head & HI_MASK(rem_len);
			reader->buffer = out[out_size - 1] << rem_len;
			reader->buffer_len = total_len - out_len;

		}

		free(fout);
	}

	printf("buffer = ");
	bio_print(&reader->buffer, reader->buffer_len);
	printf(", out = ");
	bio_print(out, out_len);
	printf("\n");
	return out;
}

unsigned char bio_read_byte(breader_ptr reader, int read_len)
{
	unsigned char* bits;
	unsigned char byte;

	assert(read_len <= CHAR_BIT);
	bits = bio_read(reader, read_len);
	byte = bits[0];	
	free(bits);

	return byte;
}

void bio_reader_delete(breader_ptr reader)
{
	_delete((struct bitfile*)reader);
}

breader_ptr bio_reader_new(const char* filename)
{
	return (breader_ptr)_new(filename, "r");
}

int bio_writer_delete(bwriter_ptr writer)
{
	int rem_len;	

	// flush buffer to disk if there are any bits left
	if (writer->buffer_len > 0) {
		fwrite(&(writer->buffer), 1, 1, writer->file_ptr);
	}
	rem_len = writer->buffer_len;
	_delete((struct bitfile*)writer);

	return rem_len;
}

bwriter_ptr bio_writer_new(const char* filename)
{
	return (bwriter_ptr)_new(filename, "w");
}

void bio_write(bwriter_ptr writer, unsigned char* bits, int write_len)
{
	int write_size;							// function output length
	int fwrite_len, fwrite_size;			// file bit and byte length
	int block_head_len, block_tail_len;	// block segment lengths
	unsigned char block_head;				// block block_head
	unsigned char* out;						// file output blocks

	write_size = write_len / CHAR_BIT;
	if ((write_len % CHAR_BIT) > 0) write_size++;

	fwrite_len = write_len + writer->buffer_len;
	fwrite_size = fwrite_len / CHAR_BIT;

	printf("write = ");
	bio_print(bits, write_len);

	if (fwrite_size == 0) {

		// case where write buffer not full yet
		writer->buffer |= bits[0] >> writer->buffer_len;
		writer->buffer_len = fwrite_len;

	} else {

		// case where blocks need to be written
		out = malloc(sizeof(unsigned char) * fwrite_size);
		memset(out, 0, sizeof(unsigned char) * fwrite_size);

		block_head = writer->buffer;
		block_head_len = writer->buffer_len;
		block_tail_len = CHAR_BIT - block_head_len;
		for (int i = 0; i < write_size; i++) {
			// right shift out block_tail block segment and inlay block_head segment
			out[i] = block_head | (bits[i] >> block_head_len);
			// create new block_head from shifted out block_tail block segment
			block_head = bits[i] << block_tail_len;
		}

		printf(", write out = ");
		bio_print(out, 8);
		fwrite(out, 1, fwrite_size, writer->file_ptr);

		if (write_size == fwrite_size) {
			writer->buffer = block_head;
		} else {
			writer->buffer = out[write_size - 1];
		}
		writer->buffer_len = fwrite_len % CHAR_BIT;
	
		free(out);
	}

	printf(", buffer = ");
	bio_print(&(writer->buffer), writer->buffer_len);
	printf("\n");
}

void bio_write_byte(bwriter_ptr writer, unsigned char bits, int write_len)
{
	assert(write_len <= CHAR_BIT);
	bio_write(writer, &bits, write_len);
}

