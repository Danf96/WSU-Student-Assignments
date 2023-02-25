#ifndef HASHTABLE_H
#define HASHTABLE_H

/* This file will contain the hash table struct that contains word pair nodes as well as interfaces
for the functions that manipulate the hash table and print out the results*/

typedef struct _node { //linked list that will contain key/value pairs and the number of occurences of each value

	int key;
	void * value; //will have to cast
	struct _node *nextNode;
	int count;
} Node;

typedef struct _hashTable {
	struct _node **bucketArray;
	int maxSize; //used to determine number of buckets in hash table
	int elements; //determines each word pair, will be used to determine when num of buckets must be expanded
} HashTable;


HashTable * hashTableInit (int maxSize, int elements);

void hashTablePut(HashTable *h, void* value); 	//inserts elements into the hash table and determines when the table must grow, 

void bucketArrayPut(Node **b, char* value, int count, int buckets); //inserts new nodes into bucketArray when growing our hash table, needs info from hash table passed with it

void hashTableGrow(HashTable *h); 				//generates a larger bucket array, moves all elements into the array, calls for old bucket array removal

void hashTableClear(HashTable *h); 				//scans through table and frees memory from buckets and linked lists

void hashTablePrint(HashTable *h, int count); 	//moves elements into a single array, sorts them, then prints them

int nodeCompare(const void * a, const void * b); //comparison function used for qsort

#endif