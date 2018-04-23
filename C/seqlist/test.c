#include <stdio.h>

#include "seqlist.h"
#include "error.h"

typedef struct
{
	int id;
	char *name;
} teacher_t;

int print_teacher(void *myseqlist)
{
	if(seqlist_isempty(myseqlist))
	{
		fprintf(stdout, "no teacher data int the list\n");
		return -1;
	}
	int length = 0;
	teacher_t *myteacher;
	length = seqlist_length(myseqlist);
	fprintf(stdout, "id of teacher int the list are ");

	for (int i = 0; i < length; i++)
	{
		myteacher = (teacher_t *)seqlist_get(myseqlist, i);
		fprintf(stdout, " %d", myteacher->id);
	}
	fprintf(stdout, "\n");
	return 0;
}

int main(int argc, char const *argv[])
{
	teacher_t myteacher1, myteacher2, myteacher3;
	myteacher1.id = 1;
	myteacher2.id = 2;
	myteacher3.id = 3;

	SeqList *myseqlist;
	myseqlist = seqlist_create(10);
	if (myseqlist == NULL)
		err_quit("seqlist_create err");

	seqlist_insert(myseqlist, &myteacher1, 0);
	seqlist_insert(myseqlist, &myteacher2, 0);
	seqlist_insert(myseqlist, &myteacher3, 0);
	
	print_teacher(myseqlist);

	seqlist_delete(myseqlist, 0);
	print_teacher(myseqlist);

	seqlist_delete(myseqlist, 1);
	print_teacher(myseqlist);
	
	seqlist_delete(myseqlist, 0);
	print_teacher(myseqlist);

	seqlist_insert(myseqlist, &myteacher2, 0);
	print_teacher(myseqlist);

	seqlist_clear(myseqlist);
	print_teacher(myseqlist);

	seqlist_destroy(myseqlist);

	return 0;
}