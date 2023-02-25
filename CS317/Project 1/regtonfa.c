/***********************************************************************
name: 
	project 1
	
description:	
	Take regular expression in postfix, convert to NFA, then sort and print regex, start and accept states, and all transitions
***********************************************************************/

/* Includes and definitions */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "regtonfa.h"
#include "stack.h"
static int snum; 		


int main(int argc, char *argv[]){
	FILE * fp;
	char buffer[1024];
	memset(buffer, '\0', 1024);
	snum = 1; //number that tracks how many states we have generated so far
	if (argc < 2){
		fprintf(stderr, "No target file specified\n");
		exit(1);
	}
	
	fp = fopen(argv[1], "r");
	if (fp == NULL){ 	
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
	Stack * myStack;
	myStack = initStack();
	
	while (fgets(buffer, 1024, fp) != NULL){
		if (buffer[strlen(buffer)-1] == '\n'){
			buffer[strlen(buffer)-1] = '\0';
		}
		char c;
		for (int i = 0; i < strlen(buffer); i++){
			c = buffer[i];
			switch(c){
				case '&':
					catNFA(myStack);
					break;
				case '|':
					orNFA(myStack);
					break;
				case '*':
					kleeneNFA(myStack);
					break;
				case 'a':
					symbolNFA(myStack, 'a');
					break;
				case 'b':
					symbolNFA(myStack, 'b');
					break;
				case 'c':
					symbolNFA(myStack, 'c');
					break;
				case 'd':
					symbolNFA(myStack, 'd');
					break;
				case 'e':
					symbolNFA(myStack, 'e');
					break;
				case 'E':
					symbolNFA(myStack, 'E');
					break;
				default:
					fprintf(stderr, "Invalid symbol in string\n");
					exit(1);
			}
	
		}
		//here we will assume the NFA is complete and pop it out, then sort and print accordingly
		NFA *n = (NFA*)popStack(myStack);
		NFA *temp;
		if ((temp = popStack(myStack)) != NULL){ 	//stack isn't empty, something went wrong, have to free everything in stack to avoid memory leaks despite exit(1)
			fprintf(stderr, "Error, malformed input\n");
			destroyNFA(temp);
			while((temp = popStack(myStack)) != NULL){
				destroyNFA(temp);
			}
			destroyStack(myStack);
			fclose(fp);
			exit(1);
		}
		printNFA(n, buffer);
		memset(buffer, '\0', 1024); 			//clear buffer for next line of file
		snum = 1; 					//reset state number count for next NFA
		destroyNFA(n);
	}
	//all regular expressions read, time to exit
	destroyStack(myStack);
	fclose(fp);

}

NFA * nfaInit(){
	NFA *n;
	n = (NFA*) malloc(sizeof(NFA));

	n->nextState = NULL;
	return n;
}
Transition * tInit(){
	Transition *t;
	t = (Transition*)malloc(sizeof(Transition));
	t->nextState = NULL;
	return t;
}
void symbolNFA(Stack* myStack, char letter){
	NFA *n;
	Transition *t;
	n = nfaInit();
	t = tInit();
	n->start = snum;
	t->s1 = snum;
	snum++;
	n->accept = snum;
	t->s2 = snum;
	snum++; 	//incremented afterwards so our snum will already be on what the next state should be at
	
	t->symbol = letter;
	n->nextState = t;
	pushStack(myStack, n); //change to just push() with new implementation
}

void catNFA(Stack* myStack){ //never increments number of states because snum never used
	NFA *n1;
	NFA *n2;
	Transition* t;
	Transition* cursor;
	n2 = (NFA*)popStack(myStack);
	n1 = (NFA*)popStack(myStack);
	cursor = n1->nextState;
	while (cursor->nextState != NULL){
		cursor = cursor->nextState;
	}
	t = tInit();		//generate epsilon transition connecting NFA1 and NFA2
	t->s1 = n1->accept;
	t->s2 = n2->start;
	t->symbol = 'E';
	cursor->nextState = t; //adds epsilon transition to the end of NFA1
	cursor = n2->nextState;
	t->nextState = cursor; //adds all transitions from NFA2 to NFA1
	n1->accept = n2->accept;
	free(n2);
	pushStack(myStack,n1);
}
void orNFA(Stack* myStack){
	NFA *n1;
	NFA *n2;
	Transition* t1;
	Transition* t2;
	Transition* cursor;
	
	n2 = (NFA*)popStack(myStack);
	n1 = (NFA*)popStack(myStack);
	if (n1 == NULL || n2 == NULL){ //not enough elements in stack for operation
		fprintf(stderr, "Error, malformed input\n");
		exit(1);
	}
	t1 = tInit(); 		//first two transitions will be for a new start state to connect to the original NFA start states
	t2 = tInit();
	t1->s1 = snum;
	t1->s2 = n1->start;
	t1->symbol = 'E';
	t2->s1 = snum;
	t2->s2 = n2->start;
	t2->symbol = 'E';
	cursor = n1->nextState;
	t2->nextState = n2->nextState;
	t1->nextState = t2;
	while (cursor->nextState != NULL){
		cursor = cursor->nextState;
	}
	cursor->nextState = t1;
	n1->start = snum;

	snum++; 		//next two transitions will be for an accept state with epsilon transition
	t1 = tInit();
	t2 = tInit();
	t1->s1 = n1->accept;
	t1->s2 = snum;
	t1->symbol = 'E';
	t1->nextState = n1->nextState; 		//very ugly looking swapping to combine the two n1 linked lists as well as t1 and t2 but works fine
	n1->nextState = t1;
	t2->s1 = n2->accept;
	t2->s2 = snum;
	t2->symbol = 'E';
	t2->nextState = n1->nextState;
	n1->nextState = t2;
	n1->accept = snum;
	free(n2);
	snum++;
	pushStack(myStack,n1);
}
void kleeneNFA(Stack* myStack){
	NFA *n;
	Transition* t;
	Transition* cursor;
	
	n = (NFA*)popStack(myStack);
	if (n == NULL){ 	//not enough elements in stack for operation
		fprintf(stderr, "Error, malformed input\n");
		exit(1);
	}
	t = tInit(); 		//new state that will be added to start of NFA
	t->s1 = snum;
	t->s2 = n->start;
	t->symbol = 'E';
	t->nextState = n->nextState;
	n->nextState = t;
	
	t = tInit(); 		//epsilon transition that will go from original accept state to new state at the start of the NFA
	t->s1 = n->accept;
	t->s2 = snum;
	t->symbol = 'E';
	cursor = n->nextState; 	
	while (cursor->nextState != NULL){
		cursor = cursor->nextState;
	}
	cursor->nextState = t;
	n->start = snum; 	//update new start and and accept states for kleene
	n->accept = snum;
	snum++;
	pushStack(myStack, n);
}

void destroyNFA(NFA* n){
	Transition* t = n->nextState;
	destroyNFARecursive(t);
	free(n);
}

void destroyNFARecursive(Transition * t){
	if (t->nextState !=NULL){
	destroyNFARecursive(t->nextState);
	}
	free(t);
	return;

}

void printNFA(NFA* n, char * buffer){
	int count = 1;
	int index = 0;
	Transition* cursor;
	Transition** sortingArray;
	cursor = n->nextState;
	while (cursor->nextState != NULL){ 			//get number of transitions to generate an aray for sorting
		cursor = cursor->nextState;
		count++;
	}
	sortingArray = (Transition **)calloc((count), sizeof(Transition*));
	cursor = n->nextState;
	while (cursor != NULL && index < count){ 		//populate new array with transition structs
		sortingArray[index] = cursor;
		cursor = cursor->nextState;
		index++;
	}
	printf("RE: %s\n", buffer);
	printf("Start: q%d\n", n->start);
	printf("Accept: q%d\n", n->accept);
	qsort(sortingArray, count, sizeof(Transition*), tCompare);
	for (index = 0; index < count; index++){
		cursor = sortingArray[index];
		printf("(q%d, %c) -> q%d\n", cursor->s1, cursor->symbol, cursor->s2);
	}
	printf("\n");
	free(sortingArray);
	
}
int tCompare(const void * a, const void * b){
	int c = (*(Transition**)a)->s1; 	//have to cast a then dereference it to get the state numbers of both transitions
	int d = (*(Transition**)b)->s1;
	if (c > d){
		return 1;
	}
	if (c < d){
		return -1;
	}
					//both c and d have the same state1 so we must compare state2
	c = (*(Transition**)a)->s2;
	d = (*(Transition**)b)->s2;
	if (c > d){
		return 1;
	}
	if (c < d){
		return -1;
	}
	return (0); //we should never have a case where there is the same start and end state for both c and d so if this returns, we should be concerned
}

