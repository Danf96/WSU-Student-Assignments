/* Includes and definitions */
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>



int randomGaussian(int mean, int stddev);
void philoRun(int sid, int philid);

int main(int argc, char *argv[]){
	int pid;
	int sid;
	int i;
	
	
	sid = semget(IPC_PRIVATE, 5, IPC_CREAT | IPC_EXCL | 0600 | IPC_NOWAIT);
	if (sid < 0){
		fprintf(stdout, "%s\n", strerror(errno));
		exit(1);
	}
	printf("Semaphore ID = %d\n", sid);
	
	//initialize semaphores so you can pick it up because you can't by default in a loop for 5 semaphores (semctl)
	for(i = 0; i < 5; i++){
		if (semctl(sid, i, SETVAL, 1) == -1){
			fprintf(stdout, "%s\n", strerror(errno));
			exit(1);
		}
		printf("Semaphore %d initial value = 1\n", i);
	}
	
	
	for(i = 0; i < 5; i++){ //forks 5 child processes, parent does nothing within loop, only waits and cleans up afterwards
		pid = fork();
		if (pid == -1){
			fprintf(stdout, "%s\n", strerror(errno));
			for(int n = 0; n < 5; n++){ //have to remove semaphores before exiting in main
				semctl(sid, n, IPC_RMID);
			}
			exit(1);
		}
		if (pid > 0){
			//nothing here
		}
		else{
			philoRun(sid, i);
		}
	}
	//loop here for 5 waits then semctl
	for(int n = 0; n < 5; n++){
		wait(NULL);	
	}
	
	if (semctl(sid, 0, IPC_RMID) == -1){
		fprintf(stdout, "%s\n", strerror(errno));
		exit(1);
	}
	printf("Semaphore successfully removed\n");
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
void philoRun(int sid, int philid){ 
	int chop1, chop2, pick, cycles;
	struct sembuf semStickUp[2];
	struct sembuf semStickDown[2];
	if (philid == 4){
		chop1 = 4;
		chop2 = 0;
	}
	else{
		chop1 = philid;
		chop2 = philid + 1; 
	}
	semStickUp[0].sem_num = chop1;
	semStickUp[0].sem_op = -1;
	semStickUp[0].sem_flg = 0;
	semStickUp[1].sem_num = chop2;
	semStickUp[1].sem_op = -1;
	semStickUp[1].sem_flg = 0;
	semStickDown[0].sem_num = chop1;
	semStickDown[0].sem_op = 1;
	semStickDown[0].sem_flg = 0;
	semStickDown[1].sem_num = chop2;
	semStickDown[1].sem_op = 1;
	semStickDown[1].sem_flg = 0;
	
	cycles = 0;
	srand(time(0)*philid);
	
	int thinkTotal = 0;
	int eatTotal = 0;
	printf("Forked philosopher %d process (%d)\n", philid+1, getpid());
	
	while(eatTotal < 100){
		printf("Philosopher %d wants chopstick %d and %d\n", philid+1, chop1+1, chop2+1);
		fflush(stdout);
		pick = semop(sid, semStickUp, 2); //picks up both sticks or none at all
		if(pick == -1 && errno != EAGAIN){
				fprintf(stderr, "%s\n", strerror(errno));
				exit(1);
			}

		if (pick == 0){
			cycles++; 
			printf("Philosopher %d has obtained chopstick %d and %d\n", philid+1, chop1+1, chop2+1);
			int randNum = randomGaussian(9, 3);
			if (randNum < 0) randNum = 0;
			eatTotal += randNum;
			printf("Philosopher %d is eating for %d seconds (%d)\n", philid+1, randNum, eatTotal);
			sleep(randNum);

			pick = semop(sid, semStickDown, 2); //put down both sticks, should always work
			if(pick == -1){
				fprintf(stderr, "%s\n", strerror(errno));
				exit(1);
			}
			
			randNum = randomGaussian(11,7);
			if (randNum < 0) randNum = 0;
			thinkTotal += randNum;
			printf("Philosopher %d is thinking for %d seconds (%d)\n", philid+1, randNum, thinkTotal);
			sleep(randNum);
		}

		
	}
	printf("Philosopher %d ate for a total of %d seconds and thought for a total of %d seconds, over %d cycles\n", philid+1, eatTotal, thinkTotal, cycles);
	exit(0);
}
