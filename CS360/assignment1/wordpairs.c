#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "wordpairs.h"
#include "hashtable.h"
#include "getWord.h"
// this function will be treated as the main and will keep track of word counts

void scanPairs(void *ptr, FILE* fp){
	char * str1;
	char * space = " ";
	char * str2;

	HashTable *h = (HashTable*)ptr; //cast void pointer to correct data type
	str1 = getNextWord(fp);
	str2 = getNextWord(fp);

	
	while (str2 != NULL){
		str1 = (char *) realloc(str1, ((strlen(str1)+strlen(str2))+2)); //strcat needs space for null term and space between words as well
		strcat(str1, space);
		strcat(str1, str2);
		hashTablePut(h, str1);

		str1 = strdup(str2); //shift word 2 to first position of word pair
		free(str2);
		str2 = getNextWord(fp);
	}
	free(str1);
}

int main (int argc, char *argv[]){
	int pairCount = 0;
	HashTable * myHashTable;
	char argBuffer[256]; //no filenames longer than 255 characters
	FILE *fp = NULL;
	int c = 1;
	myHashTable = hashTableInit(10, 0); //initialization of our hash table with a size of 10 and no elements inside
	sscanf(argv[c], "%d", &pairCount); //if <-count> flag is not present, will return a 0

	if (pairCount == 0){ //indicating that <-count> was improperly formatted
		pairCount = -99; //-99 used as a flag to know that no count was specified
		sscanf(argv[c], "%s", argBuffer); //first arg is a potential filename	
	}
	else{
		pairCount = -pairCount;
		c++; 

	}
	while (c < argc){

		sscanf(argv[c], "%s", argBuffer);

		fp = fopen(argBuffer, "r"); 	//reads filename1.txt to stream NOTE: will have to be closed
		assert(fp);

		scanPairs(myHashTable, fp); 		//will scan the entire text file and grow hash table as needed

		fclose(fp);
		c++;
		
	}
	hashTablePrint(myHashTable, pairCount);
	hashTableClear(myHashTable);
}