#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "minqueue.h"

struct minqueue
{
	node_ptr head;
};

node_ptr mq_dequeue(minqueue_ptr queue)
{
	node_ptr node = queue->head;
	queue->head = node_link(node);
	node_setlink(node, NULL);
	return node;
}

void mq_delete(minqueue_ptr queue)
{
	memset(queue, sizeof(struct minqueue), 0);
	free(queue);
}

void mq_insert(minqueue_ptr queue, node_ptr node)
{
	node_ptr prev_link = NULL;
	node_ptr	link = NULL; 
	
	// empty queue
	if (queue->head == NULL) {
		queue->head = node;
	// populated queue
	} else {
		link = queue->head;
		while ((link != NULL) && (node_count(link) <= node_count(node))) {
			prev_link = link;
			link = node_link(link);
		}
		// head of the queue
		if (prev_link == NULL) {
			node_setlink(node, link);
			queue->head = node;
		// tail of the queue
		} else if (link == NULL) {
			node_setlink(prev_link, node);
		// middle of the queue
		} else {
			node_setlink(prev_link, node);
			node_setlink(node, link);
		}
	}	
}

int mq_isempty(minqueue_ptr queue)
{
	return (queue->head == NULL);
}

minqueue_ptr mq_new(void)
{
	minqueue_ptr queue = malloc(sizeof(struct minqueue));
	memset(queue, sizeof(struct minqueue), 0);
	queue->head = NULL;
	return queue;
}

