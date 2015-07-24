
#include "queuelist.h"

/**
 * create queue of link list
 * @return handler pointer
 */
void * queue_create()
{
	return linklist_create();
}

/**
 * insert node into queue
 * @param  queue handler pointer
 * @param  node  data node 
 * @return       0: success; -1: err
 */
int queue_insert(void *queue, void *node)
{
	int pos = linklist_getlen(queue);
	return linklist_insert(queue, node, pos);
}

/**
 * delete front data of queue
 * @param  queue handler pointer
 * @return       >0: data node; NULL: err
 */
queue_node_t * queue_delete(void *queue)
{
	return linklist_delete(queue, 0);
}

/**
 * return front data node from queue
 * @param  queue handler pointer
 * @return       >0: data node; NULL: err
 */
queue_node_t * queue_head(void *queue)
{
	return linklist_get(queue, 0);
}

/**
 * verify whether queue is empty
 * @param  queue handler pointer
 * @return       true: is empty; false: is not empty
 */
bool queue_empty(void *queue)
{
	if (linklist_getlen(queue) == 0)
		return true;
	
	return false;
}

/**
 * return length of queue
 * @param  queue handler pointer
 * @return       0 or len
 */
int queue_length(void *queue)
{
	return linklist_getlen(queue);
}

/**
 * destory queue
 * @param queue handler pointer
 */
void queue_destroy(void *queue)
{
	return linklist_destroy(queue);
}

