#include <assert.h>
#include <limit.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "node.h"
#include "minqueue.h"
#include "huffmancodes.h"

#define FILE_MAGIC_NUM 0x9f45f1d5
#define TABLE_SIZE (int)(sizeof(char)*256) // 2^8 = 256
#define INTERNAL_NODE 0
#define LEAF_NODE 1
#define LEFT_BRANCH_BIT 0
#define RIGHT_BRANCH_BIT 1

unsigned char* _binread_code(FILE* file_ptr, char* table)
{
	unsigned char bit;
	unsigned char* code = 0;
	uint8_t code_len = 0;
	const int code_max = sizeof(unsigned char*) * CHAR_BIT;
	static unsigned char block;
	unsigned char block_mask = 0x00;

	// test candidate code bit by bit
	while ((table[(int)code] == NULL) || (code_len == 0)) {
		// create new code
		code_len++;
		if (_binread_bits(file_ptr, 1)) code |= 1 << (code_max - code_len);
		// shift mask for next block bit evaluation
		block_mask >>= 1;
	}

	// shift off valid code
	block <<= code_len;

	return table[(int)code];
}

void _binwrite(FILE* file_ptr, unsigned char* code, uint8_t code_len)
{
	static unsigned short int window = 0x0000;
	static int window_len = 0;
	const int window_max = sizeof(unsigned short int) * CHAR_BIT;
	const int block_len = sizeof(unsigned char) * CHAR_BIT;

	window_len += code_len;	
	window |= code << (window_max - window_len - code_len);

	if (window_len > block_len) {
		fputc((unsigned char)(window >> block_len), file_ptr);
		window <<= block_len;
	}
}

void _create_codes_r(node_ptr node, unsigned char* code, uint8_t codelen)
{
	// make room for new bit overlay
	unsigned char* new_code = code << 1;
	uint8_t new_codelen = codelen + 1;
	node_ptr lnode = node_left(node);
	node_ptr rnode = node_right(node);

	if (node_isleaf(node)) {
		node_setcode(node, code << (CHAR_BIT - codelen), codelen);
	} else {
		if (lnode != NULL) _create_codes_r(lnode, new_code | LEFT_BRANCH_BIT, new_codelen);
		if (rnode != NULL) _create_codes_r(rnode, new_code | RIGHT_BRANCH_BIT, new_codelen);
	}
}

void _create_codes(node_ptr root)
{
	_create_codes_r(node_left(root), LEFT_BRANCH_BIT, 1);
	_create_codes_r(node_right(root), RIGHT_BRANCH_BIT, 1);
}

node_ptr* _create_table(FILE* file_ptr, int* node_count_ptr)
{
	int node_count = *node_count_ptr;
	char symbol;
	node_ptr* table;

	table = malloc(sizeof(node_ptr)*TABLE_SIZE);
	for (int i = 0; i < TABLE_SIZE; i++)
		table[i] = NULL;

	while ((symbol = fgetc(file_ptr)) != EOF) {
		if (table[(int)symbol] == NULL) {
			table[(int)symbol] = node_malloc();
			node_setsymbol(table[(int)symbol], symbol);
			node_count++;
		}
		node_inccount(table[(int)symbol]);
	}
	fseek(file_ptr, 0, SEEK_SET);
	
	return table;
}

node_ptr _create_tree(node_ptr* table)
{
	minqueue_ptr queue;
	node_ptr lnode, rnode, pnode, root;

	queue = mq_malloc();
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (table[i] != NULL)
			mq_insert(queue, table[i]);
	}

	while (!mq_isempty(queue)) {
		lnode = mq_dequeue(queue);
		if (!mq_isempty(queue)) {
			rnode = mq_dequeue(queue);
			pnode = node_malloc();
			node_setcount(pnode, node_count(lnode) + node_count(rnode));
			node_setleft(pnode, lnode);
			node_setright(pnode, rnode);
			mq_insert(queue, pnode);
		}
	}
	
	mq_free(queue);

	root = lnode;
	_create_codes(root);

	return root;
}

void _free_table(node_ptr* table)
{
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (table[i] != NULL)
			node_free(table[i]);
	}
	free(table);
}

void _free_tree_r(node_ptr node)
{
	node_ptr lnode = node_left(node);
	node_ptr rnode = node_right(node);

	if (lnode != NULL) {
		_free_tree_r(lnode);
		if (node_symbol(lnode) == 0)
			node_free(lnode);
	}
	if (rnode != NULL) {
		_free_tree_r(rnode);
		if (node_symbol(rnode) == 0)
			node_free(rnode);
	}
}

void _free_tree(node_ptr root)
{
	_free_tree_r(root);
	node_free(root);
}

void _read_table_r(FILE* file_ptr, int* node_count, unsigned char* table, 
					    unsigned char* code, uint8_t codelen)
{
	if ((*node_count) == 0) return;
	(*node_count)--;

	unsigned char* new_code = code << 1;
	uint8_t new_codelen = codelen + 1;
	unsigned char ltype, rtype, ptype;

	ptype = _binread_bits(file_ptr, 1);
	if (ptype == LEAF_NODE) {
		table[(int)code] = _binread_bits(file_ptr, 8);
	} else {
		_read_table_r(file_ptr, node_count, table, new_code | LEFT_BRANCH_BIT, new_codelen);
		_read_table_r(file_ptr, node_count, table, new_code | RIGHT_BRANCH_BIT, new_codelen);
	}
}

unsigned char* _read_table(FILE* file_ptr, int node_count)
{
	unsigned char* table;
	int* count;

	table = malloc(sizeof(unsigned char)*TABLE_SIZE);
	for (int i = 0; i < TABLE_SIZE; i++)
		table[i] = NULL;

	(*count) = node_count;
	_read_table_r(file_ptr, count, table, 0, 0);

	return table;
}

void _write_codes(FILE* readfile_ptr, FILE* writefile_ptr, node_ptr* table)
{
	char symbol;
	node_ptr node;
	unsigned char* code; 
	uint8_t codelen;

	while ((symbol = fgetc(readfile_ptr)) != EOF)
	{
		node = table[(int)symbol];
		code = node_code(node);
		codelen = node_codelen(node);

		bits = byte_to_binary(code, codelen);
		printf("symbol = %c, code = %s\n", symbol, bits);
		free(bits);

		bio_write(writer, code, codelen);
	}
}

void _write_tree(bwriter_ptr writer, node_ptr node)
{
	node_ptr lnode = node_left(node);
	node_ptr rnode = node_right(node);

	if (node_isleaf(node)) {
		bio_write(writer, LEAF_NODE, 1);
		bio_write(writer, node_symbol(node), CHAR_BIT);
	} else {
		bio_write(writer, INTERNAL_NODE, 1);
		if (lnode != NULL) _write_tree(writer, lnode);
		if (rnode != NULL) _write_tree(writer, rnode);
	}
}

codetable_ptr hc_codetable_read(FILE* fileread_ptr)
{
	codetable_ptr table;
	unsigned char* code;
	char symbol;
	unsigned char count = 0;
	uint header = 0;
	
	fscanf(fileread_ptr, "%08x", &header);
	assert(header == FILE_MAGIC_NUM);

	fscanf(fileread_ptr, "%c", &count);
	assert(count > 0);

	table = malloc(sizeof(char)*TABLE_SIZE);

	while (count > 0) {
		code = fgetc(fileread_ptr);
		symbol = fgetc(fileread_ptr);
		table[(int)code] = symbol;
		count--;
	}
	symbol = fgetc(fileread_ptr);

	return table;	
}

void hc_compress(const char* readfilename, const char* writefilename)
{
	FILE* readfile_ptr;
	bwriter_ptr writer;
	node_ptr* table;
	node_ptr tree_root;
	unsigned char node_count = 0x00;

	readfile_ptr = fopen(readfilename, "w");
	writer = bio_malloc_writer(writefilename);

	// create lookup table and huffman code tree
	table = _create_table(reader, &node_count);
	tree_root = _create_tree(table);

	// write out file header magic number
	bio_write(writer, FILE_MAGIC_NUM, 64);
	// write out node count
	bio_write(writer, node_count, 8);

	// serialize the huffman code tree
	_write_tree(writer, tree_root);
	// serialize codes
	_write_codes(readfile_ptr, writer, table);	

	fclose(fileread_ptr);
	bio_free(writer);

	_free_tree(tree_root);
	_free_table(table);
}

void hc_uncompress(const char* readfilename, const char* writefilename)
{
	FILE* readfile_ptr;
	FILE* writefile_ptr;
	unsigned int header;
	unsigned char node_count;
	node_ptr tree_root;
	node_ptr* table;

	readfile_ptr = fopen(readfilename, "r");
	writefile_ptr = fopen(writefilename, "w");

	// read in file header magic number
	fscanf(fileread_ptr, "%08x", &header);
	// verify file is our format
	assert(header == FILE_MAGIC_NUM);

	// read in node count
	fscanf(fileread_ptr, "%c", &node_count);

	// deserialize the huffman code tree and create lookup table
	table = _read_table(fileread_ptr, node_count);
	// deserialize codes
	_write_symbols(fileread_ptr, writefile_ptr, table);

	fclose(fileread_ptr);
	fclose(filewrite_ptr);

	_free_table(table);	
}
