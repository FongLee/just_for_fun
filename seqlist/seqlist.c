#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "seqlist.h"


typedef struct seqlist
{
	int capacity;
	int length;
	unsigned int *node;
} seqlist_t;

/**
 * create a sequential list
 * @param  capacity [description]
 * @return          [description]
 */
void *seqlist_create(int capacity)
{
	if (capacity < 0)
		return NULL;

	seqlist_t *myseqlist;
	myseqlist = malloc(sizeof(seqlist_t) + sizeof(unsigned int) * capacity);
	myseqlist->node = (unsigned int *)(myseqlist + 1);
	myseqlist->length = 0;
	myseqlist->capacity = capacity;

	return myseqlist;
}

/**
 * destroy sequentail list
 * @param list [description]
 */
void seqlist_destroy(void * list)
{
	if (list == NULL)
		return ;

	free(list);
}

/**
 * clear sequentail list
 * @param list [description]
 */
void seqlist_clear(void * list)
{
	if (list == NULL)
		return ;

	seqlist_t *myseqlist = (seqlist_t *)list;
	myseqlist->length = 0;
}

/**
 * get the lenght of sequentail list
 * @param  list [description]
 * @return      [description]
 */
int seqlist_length(void * list)
{
	if (list == NULL)
		return -1;

	seqlist_t *myseqlist = (seqlist_t *)list;

	return myseqlist->length;
}

/**
 * get capacity of sequentail list
 * @param  list [description]
 * @return      [description]
 */
int seqlist_capacity(void * list)
{
	if (list == NULL)
		return -1;

	seqlist_t *myseqlist = (seqlist_t *)list;

	return myseqlist->capacity;
}

/**
 * insert node into sequentail list
 * @param  list [description]
 * @param  node [description]
 * @param  pos  [description]
 * @return      [description]
 */
int seqlist_insert(void * list, void *node, int pos)
{
	seqlist_t *myseqlist = (seqlist_t *)list;
	
	if (list == NULL || pos < 0 || 
		 	node == NULL || pos >= myseqlist->capacity ||
		 		seqlist_isfull(myseqlist))
		return -1;

	if(pos > myseqlist->length)
		pos = myseqlist->length;
	int i = 0;
	for(i = myseqlist->length; i > pos ; i--)
		myseqlist->node[i] = myseqlist->node[i-1];

	myseqlist->node[pos] = (unsigned int )node;
	myseqlist->length++;

	return 0; 

}

/**
 * get node int the positon of sequentail list
 * @param  list [description]
 * @param  pos  [description]
 * @return      [description]
 */
void * seqlist_get(void * list, int pos)
{
	seqlist_t *myseqlist = (seqlist_t *)list;
	if (list == NULL || pos < 0 || pos >= myseqlist->length)
		return NULL;

	return (void *)myseqlist->node[pos];
}

/**
 * delete node in the position
 * @param  list [description]
 * @param  pos  [description]
 * @return      [description]
 */
int seqlist_delete(void *list, int pos)
{
	seqlist_t *myseqlist = (seqlist_t *)list;
	if (list == NULL || pos < 0 || pos >= myseqlist->length
		 	|| seqlist_isempty(list))
		return -1;

	for(int i = pos; i < myseqlist->length; i++)
		myseqlist->node[i] = myseqlist->node[i+1];

	myseqlist->node[myseqlist->length -1] = 0;
	myseqlist->length--;
	return 0;

}

/**
 * verify whether sequentail list is empty 
 * @param  list [description]
 * @return      [description]
 */
bool seqlist_isempty(void *list)
{
	seqlist_t *myseqlist = (seqlist_t *)list;
	if (myseqlist->length == 0)
		return true;

	return false;
}

/**
 * verify whether sequentail list is full 
 * @param  list [description]
 * @return      bool; true
 */
bool seqlist_isfull(void *list)
{
	seqlist_t *myseqlist = (seqlist_t *)list;
	if (myseqlist->length == myseqlist->capacity)
		return true;

	return false;

}