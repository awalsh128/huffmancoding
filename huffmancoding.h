#ifndef __HUFFMANCODING_H__
#define __HUFFMANCODING_H__

int hc_compress(const char* readfilename, const char* writefilename);

int hc_uncompress(const char* readfilename, const char* writefilename);

#endif

