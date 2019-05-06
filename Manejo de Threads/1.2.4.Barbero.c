/**
    Ejercicio 1.2.4- Barbero dormilon
    Joaquin Piersigilli, Utizi Sebastian
**/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Defino el numero de clientes del barbero
#define NCLIENTES 20
//Defino el numero de sillas de la sala de espera
#define NSILLAS 10

sem_t salaDeEspera;
sem_t sillaBarbero;
sem_t barberoDuerme;
sem_t barberoCortando;

int termino = 0;

void *cliente(void *cl)
{
    int id_cliente = *(int*)cl;
    printf("El cliente: %d acaba de llegar.\n", id_cliente);
    //Espero a que haya lugar en la sala de espera
    sem_wait(&salaDeEspera);
    printf("El cliente %d ocupa una silla de la sala.\n", id_cliente);

    //Espero a que la silla del barbero este libre
    sem_wait(&sillaBarbero);
    sem_post(&salaDeEspera);

    printf("El cliente %d\n despierta al barbero\n", id_cliente);
    sem_post(&barberoDuerme);
    sem_wait(&barberoCortando);
    sem_post(&sillaBarbero);
    printf("El barbero termino de atender a %d.\n", id_cliente);
}

void *barbero(void *x)
{
    //Mientras haya clientes por atender
    while (!termino)
    {
        printf("El barbero esta durmiendo\n");
        sem_wait(&barberoDuerme);
        if (!termino)
        {
            printf("El barbero comienza a trabajar\n");
            //Simulo el tiempo que el barbero esta tarabando
            sleep(2);
            printf("El barbero termina de trabajar.\n");
            sem_post(&barberoCortando);
        }
    }
}

int main()
{
    pthread_t barbero_t;
    pthread_t clientes_t[20];
    int clientes[NCLIENTES];
    int i;

    //Inicializo el arreglo para poder identificar a los clientes
    for (i=0; i<NCLIENTES; i++)
        clientes[i] = i;

    //Inicializacion de los semaforos
    sem_init(&salaDeEspera, 0, NSILLAS);
    sem_init(&sillaBarbero, 0, 1);
    sem_init(&barberoDuerme, 0, 0);
    sem_init(&barberoCortando, 0, 0);

    //Creacion de los hilos
    pthread_create(&barbero_t, NULL, barbero, NULL);
    for (i=0; i<NCLIENTES; i++)
        pthread_create(&clientes_t[i], NULL, cliente, (void *)&clientes[i]);

    //Espero a que todos finalicen
    for (i=0; i<NCLIENTES; i++)
        pthread_join(clientes_t[i],NULL);

    //Todos los clientes fueron atendidos
    termino = 1;
    sem_post(&barberoDuerme);
    pthread_join(barbero_t,NULL);
}
