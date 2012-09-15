/* author: Andrew Walsh (alwalsh) */

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdio.h>

#include "hashnode.h"
#include "hashlist.h"

typedef struct hashtable * hashtable_ref;

/**
 * Get a new instance of a hashlist.
 * @precondition     none
 * @postcondition    Allocated hashtable to memory.
 * @param capacity   Capacity of string table.
 * @return           Pointer to allocated table.
 */
hashtable_ref ht_new(const int capacity);

/**
 * Delete an instance of a hashtable.
 * @precondition   none
 * @postcondition  Deallocated hashtable from memory.
 * @param table    Hash table to delete.
 */
void ht_delete(hashtable_ref table);

/**
 * Get the capacity of the hashtable.
 * @precondition   none
 * @postcondition  none      
 * @param table    Hashtable to extract capacity.
 * @return         The capacity of the hashtable.
 */
int ht_capacity(hashtable_ref table); 

/**
 * Get the associated object to the hashtable.
 * @precondition   none
 * @postcondition  none
 * @param table    Hashtable to extract object.
 * @return         The object of the hashtable.
 */
void * ht_object(hashtable_ref table);

/**
 * Insert a node into the hashtable.
 * @precondition   Hashcode must not exist in table.
 * @postcondition  An entry will be added to the table. 
 * @param table    Hashtable to intern node to.
 * @param hashcode Hash code to search by.
 * @param value    Hash value pointer.
 * @return         Node entry in the table.
 */
hashnode_ref ht_insert(hashtable_ref table, hashcode_t hashcode, \
                       void * value);

/**
 * Get hashlist from table at key.
 * @precondition   none      
 * @postcondition none
 * @param table   Hashtable to get list from.
 * @param key      Index key into hashtable.
 * @return         Hashlist from table by key.
 */
hashlist_ref ht_list(hashtable_ref table, int key);

/**
 * Peek a node from the hashtable.
 * @precondition   Hashcode must not exist in table.
 * @postcondition  none
 * @param table    Hashtable to peek into.
 * @param hashcode Hash code to search by.
 * @return         Node entry in the hashtable.
 */
hashnode_ref ht_peek(hashtable_ref table, hashcode_t hashcode);

/**
 * Set the associated object to the hashtable.
 * @precondition   none
 * @postcondition  none
 * @param table    Hashtable to set object.
 * @param object   The object of the hashtable. 
 */
void ht_setobject(hashtable_ref table, void * object);

#endif

