/***********************************************************************
name:
	readable -- recursively count readable files.
description:	
	See CS 360 Files and Directories lectures for details.
***********************************************************************/

/* Includes and definitions */
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**********************************************************************
Take as input some path. Recursively count the files that are readable.
If a file is not readable, do not count it. Return the negative value
of any error codes you may get. However, consider what errors you want
to respond to.
**********************************************************************/


int isDirectory (char *pathname) { 						//returns 1 if path is a directory, else 0
	struct stat area, *s = &area;
	
	return ((lstat(pathname, s) == 0) && S_ISDIR (s->st_mode));
}

void bufferManipulate(char * newBuffer, char * buffer, char * d_name){ 		//copies buffer to newbuffer, then concatenates the latest path entry to it
	strcpy(newBuffer, buffer);
	strcat(newBuffer, "/");
	strcat(newBuffer, d_name);
}

void readableRecursive(char * buffer, int *count, int maxPath, int *errorFlag){ //recursive function that continually scans entries in directory and maintains buffers for each recursion
	DIR *dirp = NULL;
	struct dirent *ent = NULL;
	int check = 0;
	char newBuffer[maxPath];
	memset(newBuffer, '\0', maxPath); 	//similar to calloc, ensures buffer isn't full of garbage data
	
	if(*errorFlag != 0){			//error occured somewhere, just need to recursively return out of this function and ensure that no streams were left open
		return;
	}
	
	dirp = opendir(buffer); 		//will need to make sure we're always opening searchable directories before the recursive call
	if (dirp == NULL){
		*errorFlag = errno;
		return;
	}
	errno = 0; 				//have to reset errno because readdir has same return value for end of directory and error
	
	while ((ent = readdir(dirp)) != NULL){ 						//will scan through every entry in the directory
		if (ent == NULL && errno != 0){
			*errorFlag = errno;
			closedir(dirp);
			return;
		}
		
		else {
			errno = 0; 								//reset for access check
		
			if (strcmp(".", ent->d_name) != 0 && strcmp("..", ent->d_name) != 0){ 	//ensures we skip the current directory and previous directory inodes
			
				bufferManipulate(newBuffer, buffer, ent->d_name); 		//sets up new buffer path, will be passed to the recursive call if path is directory
				if (access(newBuffer, R_OK) == 0){ 				//is file readable?
					if (isDirectory(newBuffer)){ 				//is readable file a directory?
						readableRecursive(newBuffer, count, maxPath, errorFlag); 	//pass full path
					}
					else{ 							//we know the file is readable and not a directory, therefore it's a regular file
						(*count)++;		//dereference pointer then increment the count
					}
				}			
			}
		}		
	}
		
		closedir(dirp);
		return; 
}
int readable(char* inputPath) {  					//this function will set everything up for the recursive call, then check if a 
									//correct count or error code was given, and then return it//
	int set = 0, *count = &set; 					//pointer to set so that we can maintain it recursively
	DIR *dirp = NULL;
	struct dirent *ent = NULL;
	char * buffer;
	int isError = 0, *errorFlag = &isError; 			//error flag so we can keep track of an error and return out of recursion
	int maxPath = pathconf("/", _PC_PATH_MAX);
	buffer = (char *)calloc(maxPath, sizeof(char)); 		//allocate buffer in memory
	
	if (inputPath == NULL){
		if (getcwd(buffer, maxPath) == NULL){ 			//store current working directory in buffer
			return -errno; 					//have to make errno negative
		}
	}
	else {
		strcpy(buffer, inputPath); 				//move inputPath into buffer of correct size
		if (buffer[strlen(inputPath)-1] == '/'){ 		//checks if input string already has a slash to avoid having "//" in file path
			buffer[strlen(inputPath)-1] = '\0';
			
		}
	}
	if (!isDirectory(buffer)){
		free(buffer);
		if (errno == 0){
			return 1; 					//path given was to a file, not a directory
		}
		else return -errno; 					//path most likely does not exist
	}
	
	dirp = opendir(buffer);
	if (dirp == NULL){
		return -errno;
	}
	if (closedir(dirp) < 0){
		return -errno;
	}
	readableRecursive(buffer, count, maxPath, errorFlag); 		//call recursive function
	
	free(buffer); 							//free buffer from memory
	
	if (*errorFlag == 0) return *count; 	//return readable regular files
	else return -(*errorFlag); 		//error was found during recursion, return negative errno
}

