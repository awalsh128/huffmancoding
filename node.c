/* author: Andrew Walsh (awalsh128@gmail.com) */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "node.h"

struct node
{
	unsigned char symbol;
	int count;
	unsigned char* code;
	int codelen;
	struct node* link;
	struct node* left;
	struct node* right;
};

unsigned char* node_code(node_ptr node)
{
	return node->code;
}

int node_codelen(node_ptr node)
{
	return node->codelen;
}

int node_count(node_ptr node)
{
	return node->count;
}

void node_delete(node_ptr node)
{
	assert(node != NULL);
	node->link = NULL;	
	node->left = NULL;	
	node->right = NULL;	
	free(node->code);
	memset(node, sizeof(struct node), 0);
	free(node);
	node = NULL;
}

void node_inccount(node_ptr node)
{
	node->count++;
}

int node_isleaf(node_ptr node)
{
	return (node->left == NULL) && (node->right == NULL);
}

node_ptr node_new(void)
{
	node_ptr node = malloc(sizeof(struct node));
	memset(node, sizeof(struct node), 0);
	node->symbol = '\0';
	node->count = 0;
	node->code = 0;
	node->codelen = 0;
	node->link = NULL;
	node->left = NULL;
	node->right = NULL;
	return node;
}

node_ptr node_left(node_ptr node)
{
	return node->left;
}

node_ptr node_link(node_ptr node)
{
	return node->link;
}

node_ptr node_right(node_ptr node)
{
	return node->right;
}

void node_listsort(node_ptr* nodes, int i, int j)
{
	node_ptr swap;
	node_ptr pivot;
	int k, l;

	if (i < j) 
	{
		k = i;
		l = j;
		pivot = nodes[j];

		do {
		  while ((k < l) && (nodes[k]->count <= pivot->count))
			  k++;
		  while ((l > k) && (nodes[l]->count >= pivot->count))
			  l--;
		  if (k < l) {
			  swap = nodes[k];
			  nodes[k] = nodes[l];
			  nodes[l] = swap;
		  }
		} while (k < l);

		nodes[j] = nodes[k];
		nodes[k] = pivot;

		node_listsort(nodes, i, --k);
		node_listsort(nodes, ++k, j);
	}
}

void node_setcode(node_ptr node, unsigned char* code, int codelen)
{
	node->code = code;
	node->codelen = codelen;
}

void node_setcount(node_ptr node, int count)
{
	node->count = count;
}

void node_setleft(node_ptr node, node_ptr left)
{
	node->left = left;
}

void node_setlink(node_ptr node, node_ptr link)
{
	node->link = link;
}

void node_setright(node_ptr node, node_ptr right)
{
	node->right = right;
}

void node_setsymbol(node_ptr node, unsigned char symbol)
{
	node->symbol = symbol;
}

char node_symbol(node_ptr node)
{
	return node->symbol;
}
