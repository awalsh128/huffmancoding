/* author: Andrew Walsh (alwalsh) */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hashnode.h"
#include "hashlist.h"
#include "hashtable.h"

static const float load_factor = 0.75f;

struct hashtable
{
   int count;
   int capacity;   
   hashlist_ref * lists;
   void * object;
};

static hashlist_ref * _ht_new_lists(int capacity)
{
   hashlist_ref * lists = calloc(capacity, sizeof(hashlist_ref));
   for (int i=0; i < capacity; i++) {
      lists[i] = hl_new();
   }
   return lists;
}

static void _ht_resize(hashtable_ref table) 
{
   int key;
   int capacity = table->capacity * 2;
   hashlist_ref * newlists = _ht_new_lists(capacity);
   hashnode_ref node;

   for (int i=0; i < table->capacity; i++) {
      node = hl_first(table->lists[i]);
      if (node != NULL) {
         /* chain nodes into the new table */
         while (hl_first(table->lists[i]) != NULL) {
            node = hl_dequeue(table->lists[i]);
            key = hn_hashcode(node) % capacity;
            hl_insertnode(newlists[key], node);
         }
      }
      hl_delete(table->lists[i]); /* delete old list */
   }
   free(table->lists);
   table->lists = newlists; /* reconnect new table */
   table->capacity = capacity;
}

hashtable_ref ht_new(const int capacity) 
{
   hashtable_ref table = malloc(sizeof(struct hashtable));      
   table->count = 0;
   table->capacity = capacity;
   table->lists = _ht_new_lists(capacity);
   table->object = NULL;
   return table;
}

void ht_delete(hashtable_ref table) 
{
   for (int i=0; i < table->capacity; i++) {
      hl_delete(table->lists[i]);
   }
   free(table->lists);
   table->object = NULL;
   memset(table, 0, sizeof(struct hashtable));
   free(table);
   table = NULL;
}

int ht_capacity(hashtable_ref table) 
{ 
   return table->capacity; 
}

void * ht_object(hashtable_ref table)
{
   return table->object;
}

hashnode_ref ht_insert(hashtable_ref table, hashcode_t hashcode,
                       void * value) 
{
   int key;
   double load = (double)(table->count + 1) / (double)table->capacity;
   if (load >= load_factor) _ht_resize(table);
   key = hashcode % table->capacity;
   assert(!hl_contains(table->lists[key], hashcode)); 
   table->count++;
   return hl_insert(table->lists[key], hashcode, value);
}

hashlist_ref ht_list(hashtable_ref table, int key)
{
   return table->lists[key];
}

hashnode_ref ht_peek(hashtable_ref table, hashcode_t hashcode)
{
   int key = hashcode % table->capacity;
   if (hl_contains(table->lists[key], hashcode)) { 
      return hl_node(table->lists[key], hashcode);
   } else {
      return NULL;
   }
}

void ht_setobject(hashtable_ref table, void * object)
{
   table->object = object;
}
