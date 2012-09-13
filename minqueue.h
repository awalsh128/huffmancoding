#ifndef __MINQUEUE_H__
#define __MINQUEUE_H__

#include "node.h"

typedef struct minqueue* minqueue_ptr;

void mq_free(minqueue_ptr queue);

minqueue_ptr mq_malloc(void);

void mq_insert(minqueue_ptr queue, node_ptr node);

node_ptr mq_dequeue(minqueue_ptr queue);

int mq_isempty(minqueue_ptr queue);

#endif
