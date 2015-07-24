#include <stdlib.h>

#include "linklist.h"

typedef struct linklist_handle
{
	linklist_node_t head;
	int length;
} linklist_handle_t ;

/**
 * create a link list 
 * @return handler pointer
 */
void * linklist_create()
{
	linklist_handle_t *plist = (linklist_handle_t *)malloc(sizeof(linklist_handle_t));
	if (plist == NULL)
		return NULL;

	plist->length = 0;
	plist->head.next = NULL;
	return plist;
}

/**
 * clear the link list
 * @param  list handler pointer
 * @return      0: success; -1: err
 */
int  linklist_clear(void *list)
{
	if (list == NULL)
		return -1;
	linklist_handle_t *plist = list;
	plist->head.next = NULL;
	plist->length = 0;
	return 0;
}

/**
 * destroy linklist
 * @param list handler pointer
 */
void linklist_destroy(void *list)
{
	if (list == NULL)
		return ;
	free(list);

	return;
}

/**
 * insert data into link list
 * @param  list handler pointer
 * @param  node address of node
 * @param  pos  position in link list (0-len)
 * @return      0: success; -1: err
 */
int linklist_insert(void * list, linklist_node_t *node, int pos)
{
	linklist_handle_t *plist = list;

	if (pos > plist->length || list == NULL || node == NULL)
	{
		return -1;
	}

	linklist_node_t *current = &(plist->head);
	int i = 0;
	while(i < pos)
	{
		current = current->next;
		i++;
	}

	node->next = current->next;
	current->next = node;
	plist->length++;
	return 0;

}

/**
 * delete a node in link list
 * @param  list handler pointer
 * @param  pos  position in link list (0 - len-1)
 * @return      >0: success; NULL: err
 */
linklist_node_t * linklist_delete(void * list, int pos)
{
	linklist_handle_t *plist = (linklist_handle_t *) list;

	if (list == NULL || pos >= plist->length)
		return NULL;

	linklist_node_t * current = &(plist->head);

	int i = 0; 
	while(i < pos)
	{
		current = current->next;
		i++;
	}

	linklist_node_t *tmp = current->next;
	current->next = tmp->next;
	plist->length--;
	return tmp;
}

/**
 * get the length of link list 
 * @param  list handler pointer
 * @return      0-len
 */
int linklist_getlen(void *list)
{
	if (list == NULL)
		return 0;
	linklist_handle_t *plist = (linklist_handle_t *) list;

	return plist->length;
}

/**
 * get node in position from link list
 * @param  list handler pointer
 * @param  pos  position in link list (0-len-1)
 * @return      >0: success; NULL: err
 */
linklist_node_t * linklist_get(void * list, int pos)
{
	linklist_handle_t *plist = (linklist_handle_t *) list;

	if (list == NULL || pos >= plist->length)
		return NULL;

	linklist_node_t * current = &(plist->head);

	int i = 0; 
	while(i < pos)
	{
		current = current->next;
		i++;
	}
	return current->next;

}

