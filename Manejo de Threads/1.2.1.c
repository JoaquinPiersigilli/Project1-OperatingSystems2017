
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *routine(void *msg){

	int it;
	char* lid;
	lid = (char*) msg;
	for (it=1; it<=1000000;it++){
		printf("Letra: %s |idThread: %lu |iteracion: %d\n", lid, pthread_self(),it);
	}
	return NULL;

}



int main(int argc, char *argv[])
{
    pthread_t t1,t2,t3,t4,t5;

	//Creo 5 threads
	pthread_create(&t1, NULL, routine, (void*)"A");
	pthread_create(&t2, NULL, routine, (void*)"B");
	pthread_create(&t3, NULL, routine, (void*)"C");
	pthread_create(&t4, NULL, routine, (void*)"D");
	pthread_create(&t5, NULL, routine, (void*)"E");

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    pthread_join(t5,NULL);

	//Espero a los threads creados

	return 0;
}
