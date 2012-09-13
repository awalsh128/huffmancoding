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

static int _byte_size(int n)
{
	int m = n / CHAR_BIT;
	if ((n % CHAR_BIT) > 0) m++;
	return m;
}

static void _free(struct bitfile* file)
{
	fclose(file->file_ptr);
	free(file);
}

static struct bitfile* _malloc(const char* filename, const char* file_mode)
{
	struct bitfile* file = malloc(sizeof(struct bitfile));
	file->file_ptr = fopen(filename, file_mode);
	file->buffer = 0x00;
	file->buffer_len = 0;

	return file;
}

static void _printbin(unsigned char* bits, int bit_len)
{
   int byte_len = bit_len / 8;
   int bit_rem = bit_len % 8;
   unsigned char mask = 0x80;

   int j, k;
   for (int i = 0; i < bit_len; i++) {
      j = i / 8;
      k = i % 8;
		//printf("\ni = %d, j = %d, k = %d\n", i, j, k);
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
	unsigned char block_head, block_tail;

	unsigned char* out;
	unsigned char* fout;

	int rem_len;
	
	out_len = read_len;
	out_size = _byte_size(out_len);

	fout_len = out_len - reader->buffer_len;
	fout_size = _byte_size(fout_len);
	fout_len = fout_size * CHAR_BIT;

	total_len = reader->buffer_len + fout_len;
	total_size = _byte_size(total_len);

	rem_len = out_len % CHAR_BIT;

	out = malloc(sizeof(unsigned char) * out_size);

	if (read_len < reader->buffer_len) {

		// case where no bits need to be read
		out[0] = reader->buffer_len & HI_MASK(out_len);
		reader->buffer <<= out_len;
		reader->buffer_len -= out_len;

	} else if (reader->buffer_len == 0) {

		// case where buffer is empty; read bits will already be byte aligned to output
		assert(fread(out, 1, out_size, reader->file_ptr) == out_size);	// check against underflow
		reader->buffer = out[out_size - 1] << rem_len;
		reader->buffer_len = fout_len - out_len;

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

	printf("out_len = %d, out_size = %d, fout_len = %d, fout_size = %d, total_len = %d, total_size = %d, out = ", out_len, out_size, fout_len, fout_size, total_len, total_size);
	_printbin(out, read_len);
	printf(", buffer = ");
	_printbin(&reader->buffer, reader->buffer_len);
	printf(", buffer_len = %d\n", reader->buffer_len);

	return out;
}

void bio_reader_free(breader_ptr reader)
{
	_free((struct bitfile*)reader);
}

breader_ptr bio_reader_malloc(const char* filename)
{
	return (breader_ptr)_malloc(filename, "r");
}

void bio_writer_free(bwriter_ptr writer)
{
	// flush buffer to disk if there are any bits left
	if (writer->buffer_len > 0) fwrite(&writer->buffer, 1, 1, writer->file_ptr);
	_free((struct bitfile*)writer);
}

bwriter_ptr bio_writer_malloc(const char* filename)
{
	return (bwriter_ptr)_malloc(filename, "w");
}

void bio_write(bwriter_ptr writer, unsigned char* bits, int write_len)
{
	int write_size;					// function output length
	int fwrite_len, fwrite_size;	// file bit and byte length
	int block_head_len, block_tail_len;			// block segment lengths
	unsigned char block_head;				// block block_head
	unsigned char* out;				// file output blocks

	write_size = write_len / CHAR_BIT;
	if ((write_len % CHAR_BIT) > 0) write_size++;

	fwrite_len = write_len + writer->buffer_len;
	fwrite_size = fwrite_len / CHAR_BIT;

	printf("write_len = %d, write_size = %d, fwrite_size = %d, writer->buffer_len = %d, writer->buffer = %#x\n", write_len, write_size, fwrite_size, writer->buffer_len, writer->buffer);

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
			printf("   out[%d] = %#x, block_head = %#x\n", i, bits[i], block_head);
		}

		fwrite(out, 1, fwrite_size, writer->file_ptr);

		if (write_size == fwrite_size) {
			writer->buffer = block_head;
		} else {
			writer->buffer = out[write_size - 1];
		}
		writer->buffer_len = fwrite_len % CHAR_BIT;
	
		free(out);
	}
}
