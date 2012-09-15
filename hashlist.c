/* author: Andrew Walsh (alwalsh) */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hashnode.h"
#include "hashlist.h"

struct hashlist
{
   hashnode_ref first;
   hashnode_ref last;
};

hashlist_ref hl_new()
{
   hashlist_ref list = malloc(sizeof(struct hashlist));
   list->first = NULL;
   list->last = NULL;
   return list;
}

void hl_delete(hashlist_ref list)
{
   hashnode_ref node;
   while (list->first != NULL) {
      node = list->first;
      list->first = hn_next(node);
      hn_delete(node);
   }      
   memset(list, 0, sizeof(struct hashlist));
   free(list);
   list = NULL;
}

int hl_contains(hashlist_ref list, const hashcode_t value) 
{
   if (list->first == NULL) {
      return 0;
   } else {
      hashnode_ref node = list->first;
      while (node != NULL) {
         if (value == hn_hashcode(node)) return 1;
         node = hn_next(node);
      }
      return 0;
   }
}

hashnode_ref hl_first(hashlist_ref list) 
{ 
   return list->first; 
}

hashnode_ref hl_last(hashlist_ref list) 
{ 
   return list->last; 
}

hashnode_ref hl_node(hashlist_ref list, const hashcode_t hashcode)
{
   if (list->first == NULL) {
      return 0;
   } else {
      hashnode_ref node = list->first;   
      while (node != NULL) {
         if (hn_hashcode(node) == hashcode) return node;
         node = hn_next(node);
      }
      return NULL;
   }
}

hashnode_ref hl_insert(hashlist_ref list, hashcode_t hashcode,
                       void * value)
{
   hashnode_ref second = list->first;
   list->first = hn_new(hashcode, value, list->first);
   if (second == NULL) {
      list->last = list->first;
   } else {
      hn_setprev(second, list->first); 
   }
   return list->first;
}

void hl_insertnode(hashlist_ref list, hashnode_ref node)
{
   hn_setnext(node, list->first);
   list->first = node;
}

hashnode_ref hl_dequeue(hashlist_ref list)
{
   assert(list->first != NULL);
   hashnode_ref node = list->first;
   list->first = hn_next(node);
   hn_setnext(node, NULL);
   if (list->first == NULL) {
      list->last = NULL;
   } else {
      hn_setprev(list->first, NULL);
   }
   return node;
}

