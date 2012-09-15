/* author: Andrew Walsh (alwalsh) */

#ifndef __HASHNODE_H__
#define __HASHNODE_H__

#include <stdint.h>

typedef uint32_t hashcode_t;
typedef struct hashnode * hashnode_ref;

/**
 * Get a new instance of a hashnode.
 * @precondition     none
 * @postcondition    Allocated hashnode to memory.
 * @param hashcode   Hashcode for value.
 * @param value      Value information for node.
 * @param next       Next node chained to this.
 * @return           Pointer to allocated node.
 */
hashnode_ref hn_new(hashcode_t hashcode, void * value, \
                    hashnode_ref next);

/**
 * Delete an instance of a hashnode.
 * @precondition   none
 * @postcondition  Deallocated hashnode from memory.
 * @param node     Hashnode to delete.
 */
void hn_delete(hashnode_ref node);

/**
 * Get the hashcode for the node.
 * @precondition   none
 * @postcondition  none
 * @param node     Hashnode to extract hashcode.
 * @return         The hashcode for the node.
 */
hashcode_t hn_hashcode(hashnode_ref node);

/**
 * Get the next node chained to this node.
 * @precondition   none
 * @postcondition  none
 * @param node     Hashnode sibling of next.
 * @return         The next node chained to this node.   
 */
hashnode_ref hn_next(hashnode_ref node);

/**
 * Get the previous node chained to this node.
 * @precondition   none
 * @postcondition  none
 * @param node     Hashnode sibling of previous.
 * @return         The previous node chained to this node.   
 */
hashnode_ref hn_prev(hashnode_ref node);

/**
 * Set the next node pointer.
 * @precondition   none
 * @postcondition  Node will point next to given.
 * @param node     Hashnode sibling of next.
 * @param next     Hashnode sibling of node.
 */
void hn_setnext(hashnode_ref node, hashnode_ref next);

/**
 * Set the previous node pointer.
 * @precondition   none
 * @postcondition  Node will point previous to given.
 * @param node     Hashnode sibling of previous.
 * @param next     Hashnode sibling of node.
 */
void hn_setprev(hashnode_ref node, hashnode_ref prev);

/**
 * Get the value info. 
 * @precondition   none
 * @postcondition  none
 * @param node     Hashnode with value.
 * @return         The external node.   
 */
void * hn_value(hashnode_ref node);

#endif

