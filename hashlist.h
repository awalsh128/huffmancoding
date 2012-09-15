/* author: Andrew Walsh (alwalsh) */

#ifndef __HASHLIST_H__
#define __HASHLIST_H__

#include "hashnode.h"

typedef struct hashlist * hashlist_ref;

/**
 * Get a new instance of a hashlist.
 * @precondition   none
 * @postcondition  Allocated hashlist to memory.
 * @return         Pointer to allocated node.
 */
hashlist_ref hl_new();

/**
 * Delete an instance of a hashlist.
 * @precondition   none
 * @postcondition  Deallocated hashlist from memory.
 * @param list     Hashlist to delete.
 */
void hl_delete(hashlist_ref list);

/**
 * See if a hashnode is contained in the list.
 * @precondition   none
 * @postcondition  none
 * @param list     Hash list to search.
 * @param value    Hashcode value to test for in list.
 * @return         True (1) if value found in list (else 0).
 */
int hl_contains(hashlist_ref list, hashcode_t value);   

/**
 * Get the first node in the list.
 * @precondition   none
 * @postcondition  none
 * @param list     Hash list containing first node.
 * @return         The first node in the list or NULL.
 */
hashnode_ref hl_first(hashlist_ref list);

/**
 * Get the last node in the list.
 * @precondition   Size of list must be non-zero.
 * @postcondition  none
 * @param list     Hash list containing last node.
 * @return         The last node in the list or NULL.
 */
hashnode_ref hl_last(hashlist_ref list);

/**
 * Get a node in the list matching the hashcode.
 * @precondition   Node must exist in list.
 * @postcondition  none
 * @param list     Hash list to extract node.
 * @param hashcode Hashcode to search for.
 * @return         The node in the list.
 */
hashnode_ref hl_node(hashlist_ref list, const hashcode_t hashcode); 

/**
 * Insert a node into the list.
 * @precondition    none  
 * @postcondition   List will contain an additional node.   
 * @param list      Hash list to insert node.
 * @param hashcode  Hash code to to insert with node.
 * @param value     Value to attach to hashnode.
 * @return           Hashnode inserted into list.
 */
hashnode_ref hl_insert(hashlist_ref list, hashcode_t hashcode, 
                       void * value);

/**
 * Insert a node into the list.
 * @precondition    none  
 * @postcondition   List will contain an additional node.   
 * @param list      Hash list to insert node.
 * @param node      Hashnode to insert.
 */
void hl_insertnode(hashlist_ref list, hashnode_ref node);

/**
 * Dequeue a node from the list.
 * @precondition    Size of list must be non-zero.
 * @postcondition   List will contain one less node.   
 * @param list      Hash list to perform dequeue on.
 * @return          The node dequeued from the list.   
 */
hashnode_ref hl_dequeue(hashlist_ref list);

#endif

