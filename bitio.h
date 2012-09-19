#ifndef __BITIO_H__
#define __BITIO_H__

typedef struct bitfile* breader_ptr;
typedef struct bitfile* bwriter_ptr;

int bio_byte_size(int n);

int bio_eof(breader_ptr reader);

FILE* bio_file_ptr(breader_ptr reader);

void bio_print(unsigned char* bits, int bit_len);

unsigned char* bio_read(breader_ptr reader, int read_len);

unsigned char bio_read_byte(breader_ptr reader, int read_len);

void bio_reader_delete(breader_ptr reader);

breader_ptr bio_reader_new(const char* filename);

int bio_writer_delete(bwriter_ptr writer);

bwriter_ptr bio_writer_new(const char* filename);

void bio_write(bwriter_ptr writer, unsigned char* bits, int write_len);

void bio_write_byte(bwriter_ptr writer, unsigned char bits, int write_len);

#endif
