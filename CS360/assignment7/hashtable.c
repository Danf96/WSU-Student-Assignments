#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hashtable.h"
#include "crc64.h"

HashTable * hashTableInit (int maxSize, int elements){
	HashTable *h;
	h = (HashTable*) malloc(sizeof(HashTable));
	assert(h != NULL);
	h->maxSize = maxSize; 									//maxSize will be used to keep track of # of buckets and elements will be used in sorting array
	h->elements = elements;
	h->bucketArray = (Node**)calloc(maxSize, sizeof(Node*)); //mallocs array of nodes with number of maxsize elements
	assert(h->bucketArray != NULL);
	return h;
}

void hashTablePut (HashTable *h, void* value){ 				//checks if value already present in struct and increments frequency count, otherwise generates new node in bucket
	Node *n; 							
	char * word = (char *)value; 							//must be casted to be manipulated like a string
	int key = ((int)(crc64(word) % h->maxSize));

	Node *cursor;											//cursor to navigate linked lists in buckets

	cursor = h->bucketArray[key];

	if (cursor == NULL){ //bucket is empty and node can be placed in the first location of the LL
		n = (Node*) malloc(sizeof(Node));
		assert(n != NULL);
		n->value = word;
		n->key = key;
		n->count = 1;
		h->bucketArray[key] = n; //stores node in proper bucket
		
		n->nextNode = NULL;
		h->elements++; //increments when inserting word pair from file
		return;
	}
	else{
		int depth = 0; 										//counter to keep track of how many nodes deep the linked lists are
		if (strcmp((char *)cursor->value, value) == 0){ 	//if matching value, increment frequency and free redundant memory
			cursor->count++;
			free(value);
			return;
		}
		while (cursor->nextNode != NULL){ 					//navigates LL and continues checking for matches
			if (strcmp((char *)cursor->value, value) == 0){
				cursor->count++;
				free(value); 								//already in hash table, no need to keep duplicates
				return;
			}
			depth++;
			cursor = cursor->nextNode;
		}
															//at end of linked list and no matches found, so new node must be generated and inserted
		n = (Node*) malloc(sizeof(Node)); //placing new node in linked list
		assert(n != NULL);
		n->value = word;
		n->key = key;
		n->count = 1;
		n->nextNode = NULL;
		cursor->nextNode = n;
		h->elements++;
		if (depth > 3){ //linked list has more than 3 chains deep, meaning more buckets needed
			hashTableGrow(h);
		}
		return;
	}


}

void hashTableGrow(HashTable *h){ 									//rather than strictly growing the hash table, instead grows the bucket array by a factor of 3
																	//and generates new nodes with the proper key
	Node *n;
	Node *garbage;
	Node **bucketArrayNew;
	char* value;
	int count;
	int newSize = h->maxSize * 3;
	bucketArrayNew = (Node**)calloc(newSize, sizeof(Node*)); 		//mallocs array of nodes with number of maxsize elements

	for (int i = 0; i < h->maxSize; i++){							//navigates every element and potential chain in the bucket array and frees it from memory
		n = h->bucketArray[i];
		while (n != NULL){
			count = n->count;
			value = (char *)n->value;
			bucketArrayPut(bucketArrayNew, value, count, newSize); //inserts node into new bucket array
			garbage = n;
			n = n->nextNode;
			free(garbage); 											//bucketArrayPut generates a new node, so the old node must be freed
		}
	}

	h->maxSize = newSize;  											//update hash table with new number of buckets and free old bucket array
	free(h->bucketArray);
	h->bucketArray = bucketArrayNew;
	return;


}

void bucketArrayPut(Node **b, char* value, int count, int buckets){ //similar to hashTablePut but with no checks for existing nodes of same value
	Node *n; 
	char * word = (char *)value;
	int key = ((int)(crc64(word) % buckets));

	Node *cursor;										//cursor to navigate linked lists in buckets

	cursor = b[key];

	if (cursor == NULL){ 								//bucket is empty and node can be placed in the first location of the LL
		n = (Node*) malloc(sizeof(Node));
		assert(n != NULL);
		n->value = word;
		n->key = key;
		n->count = count;
		b[key] = n; 									//stores node in proper bucket
		
		n->nextNode = NULL;
		return;
	}
	else{
		
		while (cursor->nextNode != NULL){
			cursor = cursor->nextNode;
		}
		n = (Node*) malloc(sizeof(Node)); 				//placing new node in linked list
		assert(n != NULL);
		n->value = word;
		n->key = key;
		n->count = count;
		n->nextNode = NULL;
		cursor->nextNode = n;
		return;
	}
}

void hashTableClear(HashTable *h){
	Node *n = NULL;
	Node *temp;
	for (int i = 0; i < h->maxSize; i++){				//navigates every element and potential chain in the bucket array and frees it from memory
		n = h->bucketArray[i];
		while (n != NULL){ 
			temp = n->nextNode;
			free(n->value); 							//value gets strdup'd in getWord, gets freed here
			free(n);
			n = temp;

		}
	}
	free(h->bucketArray);
	free(h); //clears hash table struct from memory
	return;
}

void hashTablePrint(HashTable *h, int count){
	Node *n = NULL;
	Node **sortingArray;
	int arrayIndex = 0;
	sortingArray = (Node **)calloc((h->elements), sizeof(Node*)); 		//no linked lists for qsort, so the array must be wide enough to fit all elements
	for (int i = 0; i < h->maxSize; i++){								//navigates every element and potential chain in the bucket array
		n = h->bucketArray[i];
		while (n != NULL && arrayIndex < h->elements){
			sortingArray[arrayIndex] = n; 								//populates sortingArray with every node from the bucket
			arrayIndex++;			
			n = n->nextNode;
		}
	}

																		//Now we sort the array based on the frequency of the word pair
	qsort(sortingArray, h->elements, sizeof(Node*), nodeCompare);
	arrayIndex = 0; 													//reset index
	if (count > h->elements){
		fprintf( stderr, "Less word pairs than count specified, printing up to count");
		count = -99;
	}
	if (count != -99){ 													//-99 is the flag used for no count
		while (arrayIndex < count){
			n = sortingArray[arrayIndex];
			printf("%10d %s\n", n->count, (char *)n->value);
			arrayIndex++;
		}
		free(sortingArray);
		return;
	}
	while (arrayIndex < h->elements){ 									//will print all elements in the sorting array
		n = sortingArray[arrayIndex];
		printf("%10d %s\n", n->count, (char *)n->value);
		arrayIndex++;
	}
	free(sortingArray); 												//no need for sortingArray after printing everything
	return;


}

int nodeCompare(const void * a, const void * b){
	int c = (*(Node**)a)->count; //weird dereferencing because we are getting a pointer to a pointer array, have to initially cast to sortingArray, 
	int d = (*(Node**)b)->count; //then pointer cast, then dereference for count
	if (c > d){
		return -1;
	}
	if (c < d){
		return 1;
	}
	return (0); //will sort from greatest to least
}

