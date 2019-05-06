#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t seml, semp1_1, semp1_2, semp1_3, semp2, semp3, semp4, semp5, semp6, parap3, contador;

void* l(void* arg){
	while(1){
	sem_wait(&semp6);
	printf("Estoy l.\n");
	usleep(900000);
	fflush(stdout);
	sem_post(&seml);
	}
}
void* p1(void* arg){
	while(1){
	sem_wait(&seml);
	printf("Estoy p1.\n");
	usleep(900000);
	fflush(stdout);
	sem_post(&semp1_1);
	sem_post(&semp1_2);
	sem_post(&semp1_3);
	}
}
void* p2(void* arg){
	while(1){
	sem_wait(&semp1_1);
	printf("Estoy p2.\n");
	usleep(900000);
	fflush(stdout);
	sem_post(&contador);
	}
}
void* p3(void* arg){
	while(1){
	sem_wait(&semp1_2);
	sem_wait(&parap3);
	sem_wait(&parap3);
	printf("Estoy p3.\n");
	usleep(900000);
	fflush(stdout);
	sem_post(&contador);
	}
}
void* p4(void* arg){
	while(1){
	sem_wait(&semp1_3);
	printf("Estoy p4.\n");
	usleep(900000);	
	sem_post(&parap3);
	sem_post(&semp4);
	}
}
void* p5(void* arg){
	while(1){	
	sem_wait(&semp4);
	printf("Estoy p5.\n");
	fflush(stdout);
	usleep(900000);
	sem_post(&parap3);
	sem_post(&contador);
	}
}
void* p6(void* arg){
	while(1){
	sem_wait(&contador);
	sem_wait(&contador);
	sem_wait(&contador);
	printf("Estoy p6.\n");
	usleep(900000);
	sem_post(&semp6);
	}
}
void main(){
	sem_init(&seml,0,0);
	sem_init(&semp1_1,0,0);
	sem_init(&semp1_2,0,0);
	sem_init(&semp1_3,0,0);
	sem_init(&semp2,0,0);
	sem_init(&semp3,0,0);
	sem_init(&semp4,0,0);
	sem_init(&semp5,0,0);
	sem_init(&semp6,0,1);
	sem_init(&parap3,0,0);
	sem_init(&contador,0,0);
	
	pthread_attr_t attr;
   	pthread_attr_init(&attr);
	pthread_t threads[7];
	pthread_create(&threads[0],&attr,l,NULL);
	pthread_create(&threads[1],&attr,p1,NULL);
	pthread_create(&threads[2],&attr,p2,NULL);
	pthread_create(&threads[3],&attr,p3,NULL);
	pthread_create(&threads[4],&attr,p4,NULL);
	pthread_create(&threads[5],&attr,p5,NULL);
	pthread_create(&threads[6],&attr,p6,NULL);
	int i;
	for (i=0; i<7; i++) {
		pthread_join(threads[i],NULL);
	}
}
