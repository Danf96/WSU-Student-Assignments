#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

pthread_cond_t wantTask;
pthread_cond_t spaceForTask;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

char task = '\0';

char *input = "hello world! How are you today? It sure is windy";

char produceTask(){
    char c = *input;
    *input++;
    return c;
}

void consumeTask(char c){
    printf("%c", c);
    fflush(stdout);
}

void *producer(){
    while(*input != '\0'){
        char temp = produceTask();
        pthread_mutex_lock(&mtx);
        if(task != '\0'){
            pthread_cond_wait(&spaceForTask, &mtx);
        }
        task = temp;
        pthread_cond_signal(&wantTask);
        pthread_mutex_unlock(&mtx);
        sleep(2);
    }
    return NULL;
}

void *consumer(){
    while(1){
        char temp ='\0';
        pthread_mutex_lock(&mtx);
        if(task == '\0'){
            pthread_cond_wait(&wantTask, &mtx);
        }
        if (task != '\0'){
            temp = task;
            task = '\0';
            pthread_cond_signal(&spaceForTask);
        }
        
        if(temp) consumeTask(temp);
        pthread_mutex_unlock(&mtx);
        sleep(1);
    }
    return NULL;
}

int main(int argc, char const *argv[]){
    pthread_t producerThread, consumerThread1, consumerThread2;
    
    pthread_cond_init(&spaceForTask, NULL);
    pthread_cond_init(&wantTask, NULL);

    pthread_create(&consumerThread1, NULL, consumer, NULL);
    pthread_create(&consumerThread2, NULL, consumer, NULL);
    pthread_create(&producerThread, NULL, producer, NULL);

    pthread_join(consumerThread1, NULL);
    pthread_join(consumerThread2, NULL);
    pthread_join(producerThread, NULL);
    return 0;
}