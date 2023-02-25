/* Includes and definitions */
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

static pthread_mutex_t mtx[5];

int randomGaussian(int mean, int stddev);
void philoRun(void * philid);

int main(int argc, char *argv[]){
	int n[5] = {1,2,3,4,5};
	int check;
	check = pthread_mutex_init(mtx, NULL);
	if (check != 0){
		fprintf(stderr, "%s\n", strerror(check));
		exit(1);
	}
	
	pthread_t philo[5]; //initialize an array of 5 philosophers
	
	for (int i = 0; i < 5; i++){ //create 5 threads for the philosophers
		check = pthread_create(&philo[i], NULL, (void *) philoRun, &n[i]);
		if (check != 0){
			fprintf(stderr, "%s\n", strerror(check));
			exit(1);
		}
	}
    	
	for (int i = 0; i < 5; i++){
		check = pthread_join(philo[i], NULL);
		if (check != 0){
			fprintf(stderr, "%s\n", strerror(check));
			exit(1);
		}
	}
}
int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}
void philoRun(void * philid){ 
	int chop1, chop2, pick1, pick2, cycles;
	int * phil;
	phil = (int *) philid;
	if (*phil == 5){
		chop1 = 4;
		chop2 = 0;
	}
	else{
		chop1 = *phil - 1;
		chop2 = *phil; 
	}
	cycles = 0;
	
	srand(time(0)*(*phil+1));
	
	int thinkTotal = 0;
	int eatTotal = 0;
	
	while(eatTotal < 100){
		//picks up both sticks or none at all
		pick1 = pthread_mutex_trylock(&mtx[chop1]);
		if (pick1 != 0 && pick1 != EBUSY){
			fprintf(stderr, "%stest\n", strerror(pick1));
			exit(1);
		}
		
		pick2 = pthread_mutex_trylock(&mtx[chop2]);
		if (pick2 != 0 && pick2 != EBUSY){
			fprintf(stderr, "%stest2\n", strerror(pick2));
			exit(1);
		}
		
		if ((pick1 == 0) && (pick2 == 0)){ 		//if both successfully picked up
			cycles++; 		 		//increment number of times we are going through the loop
			int randNum = randomGaussian(9, 3);
			if (randNum < 0) randNum = 0;
			eatTotal += randNum;
			printf("Philosopher %d is eating for %d seconds (%d)\n", *phil, randNum, eatTotal);
			sleep(randNum);
			
			pick1 = pthread_mutex_unlock(&mtx[chop1]); 	//put down both sticks
			if (pick1 != 0){
				fprintf(stderr, "%s\n", strerror(pick1));
				exit(1);
			}
			
			pick2 = pthread_mutex_unlock(&mtx[chop2]);
			if (pick2 != 0){
				fprintf(stderr, "%s\n", strerror(pick2));
				exit(1);
			}
			
			randNum = randomGaussian(11,7);
			if (randNum < 0) randNum = 0;
			thinkTotal += randNum;
			printf("Philosopher %d is thinking for %d seconds (%d)\n", *phil, randNum, thinkTotal);
			sleep(randNum);
		}
		else if (pick1 == 0){
			pick1 = pthread_mutex_unlock(&mtx[chop1]); 	//put down both sticks
			if (pick1 != 0){
				fprintf(stderr, "%s\n", strerror(pick1));
				exit(1);
			}
		}
		else if (pick2 == 0){
			pick2 = pthread_mutex_unlock(&mtx[chop2]);
			if (pick2 != 0){
				fprintf(stderr, "%s\n", strerror(pick2));
				exit(1);
			}
		
		}
	}
	printf("Philosopher %d ate for a total of %d seconds and thought for a total of %d seconds, over %d cycles\n", *phil, eatTotal, thinkTotal, cycles);
	//exit(0);
}
