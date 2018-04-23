#include <stdio.h>

#include "queuelist.h"

typedef struct teacher
{
	linklist_node_t node;
	int age;
} teacher_t;

int main(int argc, char const *argv[])
{
	void * queue;
	queue = queue_create();
	
	teacher_t t[50];

	for (int i = 0 ; i < 50; i++)
	{
		t[i].age = i;
		queue_insert(queue, &t[i]);
	}
	
	teacher_t * p;
	int k = queue_length(queue);
	for (int i = 0; i < k-1; i++)
	{
		p = (teacher_t *)queue_delete(queue);
		fprintf(stdout, "%d ", p->age);
	} 

	fprintf(stdout, "\n");

	p = (teacher_t *)queue_head(queue);
	fprintf(stdout, "%d ", p->age);
	fprintf(stdout, "\n");
	queue_delete(queue);

	for (int i = 0 ; i < 50; i++)
	{
		t[i].age = i + 100;
		queue_insert(queue, &t[i]);
	}
	if (!queue_empty(queue))
		fprintf(stdout, "queue is not empty\n");
	k = queue_length(queue);
	for (int i = 0; i < k; i++)
	{
		p = (teacher_t *)queue_delete(queue);
		fprintf(stdout, "%d ", p->age);
	} 
	fprintf(stdout, "\n");
	if (queue_empty(queue))
		fprintf(stdout, "queue not empty\n");

	queue_destroy(queue);
	return 0;
}