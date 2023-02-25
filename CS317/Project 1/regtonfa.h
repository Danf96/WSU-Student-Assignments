#ifndef REGTONFA_H
#define REGTONFA_H

#include "stack.h"

typedef struct _transition {
	int s1; 	//state1 and state2 respectively
	int s2;
	char symbol;
	struct _transition *nextState;
	
} Transition;

typedef struct _nfa { //linked list that will contain key/value pairs and the number of occurences of each value

	int start;
	int accept;
	struct _transition *nextState;
} NFA;

void catNFA(Stack* myStack); 			//combines two NFAs and pops the combination onto the stack
void orNFA(Stack* myStack); 			//also combines two NFAs and adds new states and transitions
void kleeneNFA(Stack* myStack); 		//turns NFA into a kleene star with the start and accept states being a new state
void symbolNFA(Stack* mystack, char letter); 	//generates new NFA with two states and pops it onto the stack
void destroyNFA(NFA* n); 			//frees memory used in NFA
void destroyNFARecursive(Transition * t);
NFA * nfaInit();
Transition * tInit();
void printNFA(NFA* n, char * buffer); 		//moves all transitions into an array, sorts them, then prints according to specified output
int tCompare(const void * a, const void * b);


#endif
