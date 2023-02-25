#include<stdlib.h>
#include "stack.h"

//standard approach to stack, nothing particularly interesting

Stack * initStack()
{
	Stack * s;
	s = (Stack*) malloc(sizeof(Stack));
	s->head = NULL;
	return s;
}

void destroyStack(Stack* s)
{
	//while(popStack(s) != NULL); 	//will run until nothing left in the stack
	free(s); 			//frees head of stack
}

void pushStack(Stack* s,void *data)
{
	Node * new = (Node*)malloc(sizeof(Node));
	new->data = data;
	new->next = s->head;
	s->head = new;
}

void* popStack(Stack* s)
{
	if(s->head == NULL) return NULL;
	
	void* data = s->head->data;
	Node* next = s->head->next;
	free(s->head);
	s->head = next;
	return data;
}
