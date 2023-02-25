#ifndef STACK_H
#define STACK_H
typedef struct _node
{
	void *data;
	struct _node* next;
}Node;

typedef struct _stack
{
	struct _node* head;
}Stack;

Stack * initStack();
void destroyStack(Stack* s);
void pushStack(Stack* s,void *data);
void* popStack(Stack* s);

#endif
