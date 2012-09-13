#ifndef __HUFFMANCODING_H__
#define __HUFFMANCODING_H__

#include "node.h"

void hc_compress(const char* readfilename, const char* writefilename);

void hc_uncompress(const char* readfilename, const char* writefilename);

#endif

