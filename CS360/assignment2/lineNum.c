/***********************************************************************
name:
	lineNum -- see if a word is in the online dictionary
description:	
	See CS 360 IO lecture for details.
***********************************************************************/

/* Includes and definitions */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

/**********************************************************************
Search for given word in dictionary using file descriptor fd.
Return the line number the word was found on, negative of the last line searched
if not found, or the error number if an error occurs.
**********************************************************************/
void printError(int fd, int err, char * buffer, char * cmpWord){
	fprintf(stderr, "%s\n", strerror(err)); 	//prints error based on errno passed to it
	close(fd); 					
	if (buffer != NULL){ 				//can be called before buffer is malloced so we must avoid a double free
		free(buffer);
	}
	if (cmpWord != NULL){
		free(cmpWord);
	}
	
}

int lineNum(char *dictionaryName, char *word, int length) {

	/*declaration of all the variables that will be used throughout the program:
	buffer will be used to store what is read in from the source file, cmpWord will be used to store the comparison word and will be operated on to make comparing between the
	buffer easier, the Range and Line variables will be used to track which line in the dict we are on, which will be converted into bytes when passed to index
	fd is the file descriptor, and amtRead ensures no error occurs when reading into the buffer
	*/
	char* buffer;
	char* cmpWord;
	int index;
	int startRange = 0;
	int endRange;
	int numLines;
	int currLine;
	int fd;
	int amtRead;
	int cmpCheck;
	
	cmpWord = (char*)calloc(length, sizeof(char));  //nullified character array
	if (strlen(word) + 1 > length){ 		//word is too long and must be truncated
		for (int i = 0; i < (length - 1); i++){
			cmpWord[i] = word[i];
		}
	}
	else{
		for (int i = 0; i < strlen(word); i++){
			cmpWord[i] = word[i];
		}
		for (int i = strlen(word); i < length-1; i++){
			cmpWord[i] = ' ';
		}
		
	}
	
	
	fd = open(dictionaryName, O_RDONLY);
	if (fd < 0){
		int err = errno;
		printError(fd, err, NULL, cmpWord);
		return err;
	}
	index = lseek(fd, 0, SEEK_END); //navigate to end of file
	if (index < 0){
		int err = errno;
		printError(fd, err, NULL, cmpWord);
		return err;
	}
	numLines = index/length; //divide by length to determine total number of lines
	endRange = numLines - 1; //need to decrement since we are internally treating the first entry as 0
	currLine = (numLines / 2); //finds middle of dictionary in terms of lines
	index = currLine * length; //goes to end of middle of dict in byte form
	
	index = lseek(fd, index, SEEK_SET); //seeks to middle of dictionary
	if (index < 0){
		int err = errno;
		printError(fd, err, NULL, cmpWord);
		return err;
	}
	buffer = calloc(length, sizeof(char)); 			//allocates buffer memory
	
	while (endRange >= startRange){ 			//reads to buffer and makes sure binary search is not at its end
		bufferClean(buffer, length);
		amtRead = read(fd, buffer, length);
		if (amtRead <= 0){ 
			int err = errno;
			printError(fd, err, buffer, cmpWord);
			return err;
		}
		buffer[strlen(buffer)-1] = '\0'; 		//removes newline to match with cmpWord
		
		cmpCheck = strcmp(cmpWord, buffer); 		// if cmpWord < buffer, returns negative
		if (cmpCheck == 0){
			close(fd); 				//have to free malloced memory and close file
			free(cmpWord);
			free(buffer);
	 		return (currLine + 1);
		}
		else if (cmpCheck < 0){
								//startrange is unchanged, endrange will be midway point
			endRange = currLine - 1; 		//adjusts endrange to the entry just under the current word being compared
			currLine = (startRange + endRange) / 2;
			index = currLine * length;
			index = lseek(fd, index, SEEK_SET); 
			if (index < 0){
				int err = errno;
				printError(fd, err, buffer, cmpWord);
				return err;
			}
		}
		else{
								//have to go to bottom of range
			startRange = currLine + 1;
			currLine = (startRange + endRange) / 2;
			index = currLine * length; //adjusted
			index = lseek(fd, index, SEEK_SET);
			if (index < 0){
				int err = errno;
				printError(fd, err, buffer, cmpWord);
				return err;
			}
		}
	
	}
	
								//word not found, close and free files/memory and return last line visited
	close(fd);
	free(buffer);
	free(cmpWord);
	return -(currLine + 1);
}

void bufferClean(char * word, int length){
								//nullifies everything in string since freeing and callocing again is unnecessary
	for (int i = 0; i < length; i++){
		word[i] = '\0';
	}
}

