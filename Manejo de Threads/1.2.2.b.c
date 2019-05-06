#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
//sem_t sem;

struct arg_struct {
    pthread_mutex_t *arg1;
    pthread_mutex_t *arg2;
};

struct argA_struct {
    pthread_mutex_t *arg1;
    pthread_mutex_t *arg2;
    pthread_mutex_t *arg3;
    sem_t *arg4;
};

void* Af(void * arg){
	while(1){
		struct argA_struct *args = arg;
		sem_t *sem = args->arg4; 
		pthread_mutex_t *A = args->arg1;
		pthread_mutex_lock(A);
		printf("A");
		fflush(stdout);		
		usleep(500000);
		int a = sem_trywait(sem);
		if(a==0){//tengo que escribir a
			printf("A");
			fflush(stdout);		
			usleep(500000);
			pthread_mutex_t *C = args->arg3;
			pthread_mutex_unlock(C);
		}
		else{	
			sem_post(sem);
			pthread_mutex_t *B = args->arg2;
			pthread_mutex_unlock(B);
		}	
	}
}
void* Bf(void * arg){
	while(1){
	struct arg_struct *args = arg;
	pthread_mutex_t *B = args->arg1;
	pthread_mutex_lock(B);
	printf("B");
	fflush(stdout);		
	usleep(500000);
	pthread_mutex_t *C = args->arg2;
	pthread_mutex_unlock(C);
	}
}
void* Cf(void * arg){
	while(1){	
	struct arg_struct *args = arg;
	pthread_mutex_t *C = args->arg1;
	pthread_mutex_lock(C);
	printf("C");
	fflush(stdout);		
	usleep(500000);
	pthread_mutex_t *D = args->arg2;
	pthread_mutex_unlock(D);
	}
}
void* Df(void * arg){
	while(1){	
	struct arg_struct *args = arg;
	pthread_mutex_t *D = args->arg1;
	pthread_mutex_lock(D);
	printf("D");
	fflush(stdout);		
	usleep(500000);
	pthread_mutex_t *E = args->arg2;
	pthread_mutex_unlock(E);
	}
}
void* Ef(void * arg){
	while(1){
	struct arg_struct *args = arg;
	pthread_mutex_t *E = args->arg1;
	pthread_mutex_lock(E);
	printf("E");
	fflush(stdout);		
	usleep(500000);
	pthread_mutex_t *A = args->arg2;
	pthread_mutex_unlock(A);	
	}
}
int main(){

	sem_t sem;
	pthread_mutex_t A,B,C,D,E;
	sem_init(&sem,0,1);

	pthread_mutex_init(&A,0);
	pthread_mutex_init(&B,0);
	pthread_mutex_init(&C,0);
	pthread_mutex_init(&D,0);
	pthread_mutex_init(&E,0);

	pthread_mutex_unlock(&A);
	pthread_mutex_lock(&B);
	pthread_mutex_lock(&C);
	pthread_mutex_lock(&D);
	pthread_mutex_lock(&E);

	pthread_t threads[5];

	struct argA_struct *args1 = malloc(sizeof(struct argA_struct));
	args1->arg1 = &A;
	args1->arg2 = &B;
	args1->arg3 = &C;
	args1->arg4 = &sem;

	struct arg_struct *args2 = malloc(sizeof(struct arg_struct));
	args2->arg1 = &B;
	args2->arg2 = &C;

	struct arg_struct *args3 = malloc(sizeof(struct arg_struct));
	args3->arg1 = &C;
	args3->arg2 = &D;

	struct arg_struct *args4 = malloc(sizeof(struct arg_struct));
	args4->arg1 = &D;
	args4->arg2 = &E;

	struct arg_struct *args5 = malloc(sizeof(struct arg_struct));
	args5->arg1 = &E;
	args5->arg2 = &A;

	pthread_create(&threads[0],NULL,Af,(void*)args1); //Thread para A
	pthread_create(&threads[1],NULL,Bf,(void*)args2); //Thread para B
	pthread_create(&threads[2],NULL,Cf,(void*)args3); //Thread para C
	pthread_create(&threads[3],NULL,Df,(void*)args4); //Thread para D
	pthread_create(&threads[4],NULL,Ef,(void*)args5); //Thread para E


	//El proceso espera a que terminen los 5 threads
	int i;
	for (i=0; i<5; i++) {
		pthread_join(threads[i],NULL);
	}

	free(args1);
	free(args2);
	free(args3);
	free(args4);
	free(args5);
}
