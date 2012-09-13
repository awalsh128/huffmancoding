#ifndef __BITIO_H__
#define __BITIO_H__

typedef struct bitfile* breader_ptr;
typedef struct bitfile* bwriter_ptr;

unsigned char* bio_read(breader_ptr reader, int read_len);

void bio_reader_free(breader_ptr reader);

breader_ptr bio_reader_malloc(const char* filename);

void bio_writer_free(bwriter_ptr writer);

bwriter_ptr bio_writer_malloc(const char* filename);

void bio_write(bwriter_ptr writer, unsigned char* bits, int write_len);

#endif
