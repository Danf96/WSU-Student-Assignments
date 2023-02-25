/***********************************************************************
name:
	assignment4 -- acts as a pipe using ":" to seperate programs.
description:	
	See CS 360 Processes and Exec/Pipes lecture for helpful tips.
***********************************************************************/

/* Includes and definitions */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

void bothArgs(int argc, char *argv[], int i); 		// ./assignment4 <arg1> : <arg2>
void leftArgColon(int argc, char *argv[], int i); 	// ./assignment4 <arg1> :
void leftArg(int argc, char *argv[]);			// ./assignment4 <arg1>
void rightArg(int argc, char *argv[], int i);		// ./assignment4        : <arg2>

/**********************************************************************
./assignment4 <arg1> : <arg2>

    Where: <arg1> and <arg2> are optional parameters that specify the programs
    to be run. If <arg1> is specified but <arg2> is not, then <arg1> should be
    run as though there was not a colon. Same for if <arg2> is specified but
    <arg1> is not.
**********************************************************************/


int main(int argc, char *argv[]){
	int i;
	int pipeFlag = 0;
	
	for (i = 1; i < argc; i++){ 		//scans argv for our pipe symbol
		if(strcmp(":", argv[i]) == 0){
			pipeFlag = 1; 		//our symbol for pipe ":" has been found
			if ((i + 1) == argc){
				pipeFlag = 2; 	// ./assignment4 <arg1> :
				break;
			}
			else if (i == 1){
				pipeFlag = 4; 	// ./assignment4 : <arg2>
				break;
			}
			else break;
		}
	}
	
	if (pipeFlag == 0 && (argc > 1)){
		pipeFlag = 3; 			// ./assignment4 <arg1>
	}
	
	switch(pipeFlag){
		case 0:
			fprintf(stdout, "No Arguments Specified");
			break;
		case 1:
			bothArgs(argc, argv, i);
			break;
		case 2:
			leftArgColon(argc, argv, i);
			break;
		case 3:
			leftArg(argc, argv);
			break;
		case 4:
			rightArg(argc, argv, i);
			break;
	
	}
	
	return 0;
}

void bothArgs(int argc, char *argv[], int i){
	int fd[2];
	int n;
	int err;
	
	err = pipe(fd);
	if (err != 0){
		fprintf(stdout, "%s\n", strerror(errno));
	}
	
	err = fork(); 
	if (err == -1){
		fprintf(stdout, "%s\n", strerror(errno));
	}
	if(err > 0){ 				//parent process handles pipe read
		close(fd[1]); 			//close write end
		err = dup2(fd[0], 0); 		//sets pipe read to stdin
		if (err == -1){
			fprintf(stdout, "%s\n", strerror(errno));
		}
		close(fd[0]); //close read end
		
		char * args[argc-i];
		int c;
		for (n = i, c = 0; n < argc; n++, c++){
			args[c] = argv[n+1];
		}
		args[c+1] = NULL;
		execvp(argv[i+1], args);
		fprintf(stdout, "%s\n", strerror(errno));
	
	}
	else { 	
						//child process handles write to the pipe
		close(fd[0]); 			//close read end of pipe
		err = dup2(fd[1], 1); 		//will close stdout then put write end of pipe in stdout
		if (err == -1){
			fprintf(stdout, "%s\n", strerror(errno));
		}
		close(fd[1]); 			//close write end of pipe
	
		char * args[i];
		for(n = 0; n < (i-1); n++){ 	//populates string array with correct arguments
			args[n] = argv[n + 1];
		}
		args[i-1] = NULL; 		//last string in array needs to be null
		execvp(argv[1], args);
		fprintf(stdout, "%s\n", strerror(errno)); //program only returns after execlp on an error
	}
}

//every other case is similar to bothArgs() but with different indexing for populating the array we pass to execvp()

void leftArgColon(int argc, char *argv[], int i){ 		
	int n;
	char * args[i];
	for(n = 0; n < (i-1); n++){ 		
		args[n] = argv[n + 1]; 		//index based on the knowledge that out arguments are everything past argv[0] and before the colon
	}
	args[i-1] = NULL; 			
	execvp(argv[1], args);
	fprintf(stdout, "%s\n", strerror(errno)); 
} 
void leftArg(int argc, char *argv[]){
	int n;
	char * args[argc];
	for (n = 0; n < argc; n++){ 
		args[n] = argv[n + 1];		//index is the same but uses argc rather than i to determine when to stop incrementing n
	}
	args[argc-1] = NULL;
	execvp(argv[1], args);
	fprintf(stdout, "%s\n", strerror(errno)); 

}
void rightArg(int argc, char *argv[], int i){
	int n;
	int c;
	char * args[argc-1];
	for (n = i, c = 0; n < (argc-1); n++, c++){
		args[c] = argv[n + 1];		//just like parent process of bothArg()
	}
	args[c] = NULL;
	execvp(argv[2], args);
	fprintf(stdout, "%s\n", strerror(errno));
}

