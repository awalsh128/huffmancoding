/* author: Andrew Walsh (alwalsh) */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hashnode.h"

struct hashnode
{
   void * value;
   hashcode_t hashcode;
   hashnode_ref prev;
   hashnode_ref next;
};

hashnode_ref hn_new(hashcode_t hashcode, void * value,
                    hashnode_ref next)
{
   hashnode_ref node = malloc(sizeof(struct hashnode));
   assert(node != NULL);
   assert(value != NULL);
   node->value = value;
   node->hashcode = hashcode;
   node->prev = NULL;
   node->next = next;
   return node;
}

void hn_delete(hashnode_ref node)
{
   assert(node != NULL);
   node->value = NULL;
   node->prev = NULL;
   node->next = NULL;
   memset(node, 0, sizeof(struct hashnode));
   free(node);
   node = NULL;
}

hashcode_t hn_hashcode(hashnode_ref node)
{
   return node->hashcode;
}

hashnode_ref hn_next(hashnode_ref node)
{
   return node->next;
}

hashnode_ref hn_prev(hashnode_ref node)
{
   return node->prev;
}

void hn_setnext(hashnode_ref node, hashnode_ref next)
{
   node->next = next;
}

void hn_setprev(hashnode_ref node, hashnode_ref prev)
{
   node->prev = prev;
}

void * hn_value(hashnode_ref node)
{
   return node->value;
}

