/* author: Andrew Walsh (awalsh128@gmail.com) */

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bitio.h"
#include "node.h"
#include "minqueue.h"
#include "huffmancoding.h"

#define FILE_MAGIC_NUM 0x9f45f1d5
#define TABLE_SIZE (int)(sizeof(char)*256) // 2^8 = 256
#define INTERNAL_NODE 0x00
#define LEAF_NODE 0x80
#define LEFT_BRANCH_BIT 0x80
#define RIGHT_BRANCH_BIT 0x00

static unsigned char* _catbit(unsigned char bit, unsigned char* bits, int bits_len)
{
	unsigned char* newbits;
	int bits_size, newbits_size; 

	bits_size = bio_byte_size(bits_len);
	newbits_size = bio_byte_size(bits_len + 1);
	newbits = malloc(sizeof(unsigned char) * newbits_size);
	memset(newbits, 0, newbits_size);
	memcpy(newbits, bits, bits_size);

	for (int i = (newbits_size - 1); i > 0; i--) {
		newbits[i] = (newbits[i] >> 1) | newbits[i - 1];
	}
	newbits[0] = (newbits[0] >> 1) | bit;

	return newbits;
}

static void _create_codes_r(node_ptr node, unsigned char bit, unsigned char* code, int code_len)
{
	int newcode_len;
	unsigned char* newcode;

	node_ptr lnode = node_left(node);
	node_ptr rnode = node_right(node);

	newcode = _catbit(bit, code, code_len);
	newcode_len = code_len + 1;

	if (node_isleaf(node)) {
		node_setcode(node, newcode, newcode_len);
	} else {
		if (lnode != NULL) _create_codes_r(lnode, LEFT_BRANCH_BIT, newcode, newcode_len);
		if (rnode != NULL) _create_codes_r(rnode, RIGHT_BRANCH_BIT, newcode, newcode_len);
		free(newcode);
	}
}

static void _create_codes(node_ptr root)
{
	unsigned char* code = malloc(sizeof(char));

	_create_codes_r(node_left(root), LEFT_BRANCH_BIT, code, 0);
	_create_codes_r(node_right(root), RIGHT_BRANCH_BIT, code, 0);

	free(code);
}

static node_ptr* _create_table(FILE* file_ptr, int* node_count_ptr)
{
	int node_count = *node_count_ptr;
	char symbol;
	node_ptr* table;

	table = malloc(sizeof(node_ptr) * TABLE_SIZE);
	for (int i = 0; i < TABLE_SIZE; i++)
		table[i] = NULL;

	while ((symbol = fgetc(file_ptr)) != EOF) {
		if (table[(int)symbol] == NULL) {
			table[(int)symbol] = node_new();
			node_setsymbol(table[(int)symbol], symbol);
			node_count++;
		}
		node_inccount(table[(int)symbol]);
	}
	fseek(file_ptr, 0, SEEK_SET);
	
	return table;
}

static node_ptr _create_tree(node_ptr* table)
{
	minqueue_ptr queue;
	node_ptr lnode, rnode, pnode, root;

	queue = mq_new();
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (table[i] != NULL)
			mq_insert(queue, table[i]);
	}

	while (!mq_isempty(queue)) {
		lnode = mq_dequeue(queue);
		if (!mq_isempty(queue)) {
			rnode = mq_dequeue(queue);
			pnode = node_new();
			node_setcount(pnode, node_count(lnode) + node_count(rnode));
			node_setleft(pnode, lnode);
			node_setright(pnode, rnode);
			mq_insert(queue, pnode);
		}
	}
	
	mq_delete(queue);

	root = lnode;
	_create_codes(root);

	return root;
}

static void _delete_table(node_ptr* table)
{
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (table[i] != NULL)
			node_delete(table[i]);
	}
	free(table);
}

static void _delete_tree_r(node_ptr node)
{
	node_ptr lnode = node_left(node);
	node_ptr rnode = node_right(node);

	if (lnode != NULL) {
		_delete_tree_r(lnode);
		if (node_symbol(lnode) == 0)
			node_delete(lnode);
	}
	if (rnode != NULL) {
		_delete_tree_r(rnode);
		if (node_symbol(rnode) == 0)
			node_delete(rnode);
	}
}

static void _delete_tree(node_ptr root)
{
	_delete_tree_r(root);
	node_delete(root);
}

static node_ptr _read_tree_r(breader_ptr reader, unsigned char bit, unsigned char* code, int code_len)
{
	unsigned char ptype;
	unsigned char symbol;
	node_ptr pnode, lnode, rnode;
	unsigned char* newcode;
	int newcode_len;

	newcode = _catbit(bit, code, code_len);
	newcode_len = code_len + 1;

	pnode = node_new();
	ptype = bio_read_byte(reader, 1);

	if (ptype == LEAF_NODE) {
		symbol = bio_read_byte(reader, CHAR_BIT);
		node_setcode(pnode, newcode, newcode_len);
		node_setsymbol(pnode, symbol);
	} else if (ptype == INTERNAL_NODE) {
		lnode = _read_tree_r(reader, LEFT_BRANCH_BIT, newcode, newcode_len);
		rnode = _read_tree_r(reader, RIGHT_BRANCH_BIT, newcode, newcode_len);
		node_setleft(pnode, lnode);
		node_setright(pnode, rnode);
		free(newcode);
	} else {
		printf("error: ptype = %#x\n", ptype);
	}

	return pnode;
}
static node_ptr _read_tree(breader_ptr reader)
{
	unsigned char rtype;
	unsigned char symbol;
	node_ptr root, lnode, rnode;
	unsigned char* code;
	int code_len;

	code = malloc(sizeof(unsigned char));
	code_len = 0;

	root = node_new();
	rtype = bio_read_byte(reader, 1);

	if (rtype == LEAF_NODE) {
		symbol = bio_read_byte(reader, CHAR_BIT);
		node_setcode(root, code, code_len);
		node_setsymbol(root, symbol);
	} else if (rtype == INTERNAL_NODE) {
		lnode = _read_tree_r(reader, LEFT_BRANCH_BIT, code, code_len);
		rnode = _read_tree_r(reader, RIGHT_BRANCH_BIT, code, code_len);
		node_setleft(root, lnode);
		node_setright(root, rnode);
		free(code);
	} else {
		printf("error: rtype = %#x\n", rtype);
	}

	return root;
}

void _write_codes(FILE* readfile_ptr, bwriter_ptr writer, node_ptr* table)
{
	char symbol;
	node_ptr node;

	while ((symbol = fgetc(readfile_ptr)) != EOF)
	{
		node = table[(int)symbol];
		bio_write(writer, node_code(node), node_codelen(node));
	}
}

void _write_symbols(breader_ptr reader, FILE* writefile_ptr, node_ptr tree_root)
{
	node_ptr node = tree_root;
	unsigned char bit;
	unsigned char symbol;

	while (!bio_eof(reader)) {
		bit = bio_read_byte(reader, 1);
		if (bit == LEFT_BRANCH_BIT) {
			node = node_left(node);
		} else {
			node = node_right(node);
		}
		if (node_isleaf(node)) {
			symbol = node_symbol(node);
			fwrite(&symbol, 1, 1, writefile_ptr);
			node = tree_root;
		}
	}
}

void _write_tree(bwriter_ptr writer, node_ptr node)
{
	node_ptr lnode = node_left(node);
	node_ptr rnode = node_right(node);

	if (node_isleaf(node)) {
		bio_write_byte(writer, LEAF_NODE, 1);
		bio_write_byte(writer, node_symbol(node), CHAR_BIT);
	} else {
		bio_write_byte(writer, INTERNAL_NODE, 1);
		if (lnode != NULL) _write_tree(writer, lnode);
		if (rnode != NULL) _write_tree(writer, rnode);
	}
}

int hc_compress(const char* readfilename, const char* writefilename)
{
	FILE* readfile_ptr;
	size_t readfile_size;
	bwriter_ptr writer;
	node_ptr* table;
	node_ptr tree_root;
	int node_count = 0;

	// verify file is compressable
	readfile_ptr = fopen(readfilename, "r");
	fseek(readfile_ptr, 0L, SEEK_END);
	readfile_size = ftell(readfile_ptr);
	fseek(readfile_ptr, 0L, SEEK_SET);	
	if (readfile_size == 0) {
		fprintf(stderr, "Nothing to compress.\n");
		return 0;
	}

	// create lookup table and huffman code tree
	table = _create_table(readfile_ptr, &node_count);
	tree_root = _create_tree(table);

	writer = bio_writer_new(writefilename);

	// write out file header magic number
	fprintf(bio_file_ptr(writer), "%08x", FILE_MAGIC_NUM);

	// serialize the huffman code tree
	_write_tree(writer, tree_root);
	// serialize codes
	_write_codes(readfile_ptr, writer, table);	

	fclose(readfile_ptr);
	bio_writer_delete(writer);

	_delete_tree(tree_root);
	_delete_table(table);

	return 1;
}

int hc_uncompress(const char* readfilename, const char* writefilename)
{
	FILE* writefile_ptr;
	breader_ptr reader;
	unsigned int header;
	node_ptr tree_root;

	reader = bio_reader_new(readfilename);
	writefile_ptr = fopen(writefilename, "w");

	// read in file header magic number
	fscanf(bio_file_ptr(reader), "%08x", &header);
	// verify file is our format
	assert(header == FILE_MAGIC_NUM);

	// deserialize the huffman coding tree
	tree_root = _read_tree(reader);
	// deserialize codes
	_write_symbols(reader, writefile_ptr, tree_root);

	_delete_tree(tree_root);
	bio_reader_delete(reader);
	fclose(writefile_ptr);

	return 1;
}
