#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFERSIZE 10

static char buffer[BUFFERSIZE];
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int available = 0;
static int prod = 0;
static int cons = 0;

static void *producer(void *arg){
	char *input = (char*)arg;
	int i = 0;
	while(input[i]){
		pthread_mutex_lock(&mtx);
		if(available < BUFFERSIZE){
			buffer[prod % BUFFERSIZE] = input[i];
			prod++;
			available++;
			i++;
		}
		pthread_mutex_unlock(&mtx);
		//sleep(1);
	}
	return NULL;
}

static void *consumer(){
	while(1){
		pthread_mutex_lock(&mtx);
		if(available > 0){
			printf("%c", buffer[cons % BUFFERSIZE]);
			fflush(stdout);
			available--;
			cons++;
		}
		pthread_mutex_unlock(&mtx);
		sleep(1);
	}
	return NULL;
}

int main(int argc, char const *argv[]){
	char* input = "hello world!";
	pthread_t thread1, thread2, thread3;
	pthread_create(&thread1, NULL, producer, input);
	pthread_create(&thread2, NULL, consumer, NULL);
	pthread_create(&thread3, NULL, consumer, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	return 0;
}