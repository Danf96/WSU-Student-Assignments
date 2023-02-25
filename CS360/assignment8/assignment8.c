#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/sem.h>

#define MY_PORT_NUMBER 49999
#define BACKLOG 10

void client(char * address);
void server();

typedef struct _hostList {
	char * hostName;
	int count;
    struct _hostList *nextHost;
} HostList; 

HostList * hostListInit (char * hostName);
int navHostList(char * hostName, HostList * myList, int sid);





int main(int argc, char const *argv[]){
    if (argc < 2){
    	fprintf(stderr, "Error: Not enough arguments\n");
    	exit(1);
    }
    if (argc == 2 && (strcmp("server", argv[1]) == 0)){ //do strcmp
    	server();
    }
    else if (argc >= 2 && (strcmp("client", argv[1]) == 0)){ //add case where localhost if no address and change check
    	char * address;
    	if (argc == 2){
    		address = strdup("127.0.0.1"); 	//no address specified so we'll use loopback address
    	}
    	else {
    		address = strdup(argv[2]);
    	}
    	client(address);
    	free(address); 	//free allocated memory
    }
    else{
    	fprintf(stderr, "Error: Bad input\n");
    	exit(1);
    }

    return 0;
}

void server(){
	int sid;
	int listenfd, connectfd, length;
	int err = 0;
	struct sockaddr_in servAddr;

	sid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600);
	if (sid < 0){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
	if (semctl(sid, 0, SETVAL, 1) == -1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	HostList * myList = hostListInit("head"); 		//initialization of linked list that will track our hosts
	

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		exit(1);
	}
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
		perror("Error");
	} //error check
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(MY_PORT_NUMBER);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(listenfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
		perror("Error");
		exit(1);
	}

	err = listen(listenfd, BACKLOG);
	if (err < 0){
		perror("Error");
		exit(1);
	}
	
	length = sizeof(struct sockaddr_in);
	struct sockaddr_in clientAddr;
	char buf[19] = {0};

	//int count = 0;


	
	while (1){
		err = 0;
		connectfd = accept(listenfd, (struct sockaddr *) &clientAddr, &length); 	//after this we should log hostname of client and # of times connected and paste to stdout
		if (connectfd < 0){ //add checks for connection aborted , don't want to kill the program jus tbecause of that
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(1);
		}

		int pid = fork(); 
		if (pid == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno));
		}
		if (pid == 0){ //child process will handle the connection, pass current date and time to client, also write hostname and num times connected
			

			time_t myTime;
    		time(&myTime);
    		char * tempbuf;
    		char buf[19];
    		tempbuf = ctime(&myTime); //insert current time into buffer
    		strncpy(buf, tempbuf, 18);
    		
    		buf[18] = '\0';
    		
			err = write(connectfd, buf, 19);
			if(err < 0){
				fprintf(stderr, "Error: %s\n", strerror(err));
				exit(1);
			}
			close(connectfd);
			exit(0);

		
		}
		else{ 	//this is parent of process
			char hostName [NI_MAXHOST];
			int hostEntry;
			hostEntry = getnameinfo((struct sockaddr*)&clientAddr, sizeof(clientAddr), hostName, sizeof(hostName), NULL, 0, NI_NUMERICSERV);
			if (hostEntry != 0){
				fprintf(stderr, "Error: %s\n", gai_strerror(hostEntry));
			}
			//call navHostList here, then output to stdout
			int count = navHostList(hostName, myList, sid);
			//count++;
			printf("%s %d\n", hostName, count);
			fflush(stdout);
			close(connectfd);
		}
	}
}
HostList * hostListInit (char * hostName){
	HostList *h;
	h = (HostList*) malloc(sizeof(HostList));
	h->hostName = strdup(hostName); 									//maxSize will be used to keep track of # of buckets and elements will be used in sorting array
	h->count = 1;
	h->nextHost = NULL;
	
	return h;
}

int navHostList(char * hostName, HostList * myList, int sid){ //will get passed the HostList, navigate it and do strcmps to check if host already exists, if exists, increment, else add new entry to linked list
	HostList *cursor;
	int err = 0;
	char flag = 0;
	struct sembuf semlock[1] = {{0, -1, 0}};
	struct sembuf semunlock[1] = {{0, 1, 0}};

	

	err = semop(sid, semlock, 1); //lock if already in use
	if(err == -1){
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
	}
	if (err == 0){
		cursor = myList;
		while(cursor->nextHost != NULL){
			cursor = cursor->nextHost;
			if(strcmp(cursor->hostName, hostName) == 0){
				cursor->count++;
				err = semop(sid, semunlock, 1);
				if(err == -1){
					fprintf(stderr, "Error: %s\n", strerror(errno)); //errror here
					exit(1);
				}
				return cursor->count; 		//existing hostname was found in linked list so we just increment and return number
			}
		}
		HostList * h;
		h = hostListInit(hostName);
		cursor->nextHost = h;
		err = semop(sid, semunlock, 1);
		if(err == -1){
			fprintf(stderr, "Error: %s\n", strerror(errno)); //errror here
			exit(1);
		}
		return h->count;
	}
	


}

void client(char * address){
	int socketfd, err, numberRead = 0;
	struct addrinfo hints, *actualdata;
	char buf[19] = {0};

	memset(&hints, 0, sizeof(hints));

	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	err = getaddrinfo(address, "49999", &hints, &actualdata);
	if(err != 0){
		fprintf(stderr, "Error: %s\n", gai_strerror(err));
	}
	socketfd = socket(actualdata->ai_family, actualdata->ai_socktype, 0);
	if (socketfd < 0){
		fprintf(stderr, "Error: %s\n", strerror(err));
	}
	
	
	if(connect(socketfd, actualdata->ai_addr, actualdata->ai_addrlen) < 0){
		perror("Connect");
		exit(1);
	}

		numberRead = read(socketfd, buf, 19);
		if(strlen(buf) != 18){
			fprintf(stderr, "Error: Not enough bytes read.\n");
			exit(1);
		}
		printf("%s\n", buf);
	close(socketfd); //maybe error check?

	
}
