#ifndef QUEUELIST_H
#define QUEUELIST_H

#include <stdbool.h>

#include "linklist.h"

typedef  linklist_node_t queue_node_t ;

/**
 * create queue of link list
 * @return handler pointer
 */
void * queue_create();

/**
 * destory queue
 * @param queue handler pointer
 */
void queue_destroy(void *queue);

/**
 * insert node into queue
 * @param  queue handler pointer
 * @param  node  data node 
 * @return       0: success; -1: err
 */
int queue_insert(void *queue, void *node);

/**
 * delete front data of queue
 * @param  queue handler pointer
 * @return       >0: data node; NULL: err
 */
queue_node_t * queue_delete(void *queue);

/**
 * return front data node from queue
 * @param  queue handler pointer
 * @return       >0: data node; NULL: err
 */
queue_node_t * queue_head(void *queue);

/**
 * return length of queue
 * @param  queue handler pointer
 * @return       0 or len
 */
int queue_length(void *queue);

/**
 * verify whether queue is empty
 * @param  queue handler pointer
 * @return       true: is empty; false: is not empty
 */
bool queue_empty(void *queue);

#endif