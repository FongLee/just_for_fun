#ifndef LINKLIST_H
#define LINKLIST_H


typedef struct linklist_node
{
	struct link_list_node *next;
} linklist_node_t;

/**
 * create a link list 
 * @return handler pointer
 */
void * linklist_create();

/**
 * clear the link list
 * @param  list handler pointer
 * @return      0: success; -1: err
 */
int   linklist_clear(void *list);

/**
 * destroy linklist
 * @param list handler pointer
 */
void linklist_destroy(void *list);

/**
 * insert data into link list
 * @param  list handler pointer
 * @param  node address of node
 * @param  pos  position in link list (0-len)
 * @return      0: success; -1: err
 */
int linklist_insert(void * list, linklist_node_t *node, int pos);

/**
 * delete a node in link list
 * @param  list handler pointer
 * @param  pos  position in link list (0 - len-1)
 * @return      >0: success; NULL: err
 */
linklist_node_t * linklist_delete(void * list, int pos);

/**
 * get the length of link list 
 * @param  list handler pointer
 * @return      0-len
 */
int linklist_getlen(void *list);

/**
 * get node in position from link list
 * @param  list handler pointer
 * @param  pos  position in link list (0-len-1)
 * @return      >0: success; NULL: err
 */
linklist_node_t * linklist_get(void * list, int pos);

#endif