#ifndef SEQLIST_H
#define SEQLIST_H
#include <stdbool.h>

typedef void SeqList;

void *seqlist_create(int capacity);

void seqlist_destroy(void * list);

void seqlist_clear(void * list);

int seqlist_length(void * list);

int seqlist_capacity(void * list);

int seqlist_insert(void * list, void *node, int pos);

void * seqlist_get(void * list, int pos);

int  seqlist_delete(void *list, int pos);

bool seqlist_isempty(void *list);

bool seqlist_isfull(void *list);


#endif