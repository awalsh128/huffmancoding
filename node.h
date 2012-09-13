#ifndef __NODE_H__
#define __NODE_H__

typedef struct node* node_ptr;

unsigned char* node_code(node_ptr node);

uint8_t node_codelen(node_ptr node);

int node_count(node_ptr node);

void node_free(node_ptr node);

void node_inccount(node_ptr node);

int node_isleaf(node_ptr node);

node_ptr node_malloc(void);

node_ptr node_left(node_ptr node);

node_ptr node_link(node_ptr node);

node_ptr node_right(node_ptr node);

void node_listsort(node_ptr* nodes, int i, int j);

void node_setcode(node_ptr node, unsigned char* code, uint8_t codelen);

void node_setcount(node_ptr node, int count);

void node_setleft(node_ptr node, node_ptr left);

void node_setlink(node_ptr node, node_ptr link);

void node_setright(node_ptr node, node_ptr right);

void node_setsymbol(node_ptr node, char symbol);

char node_symbol(node_ptr node);

#endif
