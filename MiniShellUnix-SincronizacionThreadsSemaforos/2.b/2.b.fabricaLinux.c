/**,
    Ejercicio 2.2b [Proyecto 1] Fabrica DCIC
    Joaquin Piersigilli, Utizi Sebastian
**/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define CANTP 8

sem_t semPersonas, espera, maquinaEncendida, trabajando, tercero; //semaforos relacionados a la maquina
pthread_mutex_t seccionCritica, banio;
sem_t maxHombres, mutexHombre, cantActualHombres, maxMujeres, mutexMujer, cantActualMujeres; //semaforos relacionados la baño

unsigned long long rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

void trabajar(int id_persona){
	printf("%d Me toca trabajar: \n",id_persona);
	fflush(stdout);
	
	if( 0 == sem_trywait(&espera)){
   		printf("maquina esta apagada y el trabajador %d espera que sean 3\n",id_persona);
		fflush(stdout);
		sem_post(&semPersonas); //antes de bloquearse esperando que lleguen mas personas aviso que pueden mandar a hacer tareas a mas personas.
    		sem_wait(&tercero);
	}else{
     	      if(sem_trywait(&maquinaEncendida)==-1){ //cuando se va a encender la maquina sé que hay exactamente 3 personas trabajando, entonces aumento el semaforo trabajando a 2 (como inicializandolo en el momento que arranca la maquina) y luego lo continuo aumentando el mismo cada vez que se suma alguien a trabajar, de manera que el semaforo siempre va a tener 1 persona menos de las que realmente hay, luego, cuando las personas dejan de trabajar decremento el semaforo con un trywait, por lo que al irse la anteultima persona el semaforo pasara a valer 0 y el ultimo producira un error en el trywait y así sé que debo apagar la maquina.
		     printf("Con %d son 3 y se enciende la maquina\n",id_persona);
		     fflush(stdout);
       		     sem_post(&maquinaEncendida);
         	     sem_post(&tercero);
         	     sem_post(&tercero);
		     sem_post(&trabajando);
		     sem_post(&trabajando);
              }else{
                        sem_post(&maquinaEncendida); //para incrementar de nuevo el semaforo que se decremento en el trywait del segundo if
			sem_post(&trabajando); //a partir del 4to trabajador viene aca y suman de a 1 normalmente
                  }

	       sem_post(&semPersonas); //antes de ponerme a trabajar aviso q pueden mandar mas gente a hacer tareas
             }
	
	//trabaja un tiempo random"
	int rnd = rdtsc() % 7000;
	printf("%d va a trabajar %i milisegs\n",id_persona,rnd);
	fflush(stdout);
	usleep(rnd);


	//va a dejar de trabajar una persona, se fija si quedan 0 para saber si hay que apagar la maquina
	pthread_mutex_lock(&seccionCritica);
	if(-1 == sem_trywait(&trabajando)){
		printf("deja de trabajar %d\n",id_persona);
		fflush(stdout);
		printf("Como no queda mas gente trabajando se apaga la maquina\n");
		fflush(stdout);
		sem_wait(&maquinaEncendida);
    		sem_post(&espera);
    		sem_post(&espera);
	}else{
		printf("deja de trabajar %d\n",id_persona);
		fflush(stdout);
	}
	pthread_mutex_unlock(&seccionCritica);
}


char que_soy(int id_persona){
    //Asumimos que si el id de la persona es par entonces es mujer
    if (id_persona %2 == 0)
        return 'M';
    else //Si es impar entonces es hombre
        return 'H';
}


void uso_gabinete(int p){
	printf("%d Me toca ir al baño: \n",p);		
	sem_post(&semPersonas);
	char sex = que_soy(p);
	if(sex=='M') //una mujer tiene que usar el baño
	{
		sem_wait(&maxMujeres); //si hay 2 mujeres adentro del baño esta se queda esperando
		sem_wait(&mutexMujer); //si no habia 2 mujeres, pide el mutex para entrar a seccion critica

		//si no habia ninguna mujer antes (o sea cantActualMujeres estaba en 0) se tiene q fijar q no haya hombres, en cambio
		//si ya habia 1 mujer, la actual simplemente entra al baño
		if (sem_trywait(&cantActualMujeres) == -1) 
		{	
			sem_post(&cantActualMujeres); //aumento la cantidad de mujeres
			pthread_mutex_lock(&banio);   //intento obtener el mutex banio, si no puedo quiere decir que lo tiene un hombre			
		}else{ //realizo dos post debido a que si habia una mujer, con el trywait el semaforo que indica la cantidad de mujeres se decremento a 0
			sem_post(&cantActualMujeres);
			sem_post(&cantActualMujeres);
		}
		sem_post(&mutexMujer); //libero el mutex de la seccion critica, puede entrar otra mujer

		// uso el baño
		printf("Mujer %i usa el baño \n",p);
		fflush(stdout);
		sleep(3);

		sem_wait(&mutexMujer);             //seccion critica si mas de una mujer termina de usar el baño
		sem_wait(&cantActualMujeres);	   //decremento la cantidad de mujeres
		printf("Mujer %i deja el baño \n",p);
		if (sem_trywait(&cantActualMujeres) == -1) //si no quedan mas mujeres, libero el banio para que puedan entrar hombres en caso de que alguno este bloqueado en el lock mutex banio.
		{
			pthread_mutex_unlock(&banio);
		}else{
			sem_post(&cantActualMujeres);	//si todavia quedan mujeres, incremento el semaforo por el decremento que se produjo al realizar el trywait	
		}
		sem_post(&mutexMujer);	//libera mutex de seccion critica para que se vaya otra mujer si es necesario
		sem_post(&maxMujeres);	//se va una mujer, se aumenta la cantidad maxima de mujeres que puede haber
		
	}
	else //un hombre tiene que usar el baño
	{
		sem_wait(&maxHombres); //si hay 2 hombres adentro del baño este se queda esperando
		sem_wait(&mutexHombre); //si no habia 2 hombres, pide el mutex para entrar a seccion critica
		
		//si no habia ningun hombre antes (o sea cantActualHombres estaba en 0) se tiene q fijar q no haya mujeres, en cambio
		//si ya habia 1 hombre simplemente entra al baño
		if (sem_trywait(&cantActualHombres) == -1) 
		{
			sem_post(&cantActualHombres);
			pthread_mutex_lock(&banio);
		}else{
			sem_post(&cantActualHombres);
			sem_post(&cantActualHombres);	
		}
		sem_post(&mutexHombre); //libero el mutex de la seccion critica


		// uso el baño
		printf("Hombre %i usa el baño \n",p);
		fflush(stdout);
		sleep(3);

		sem_wait(&mutexHombre);
		sem_wait(&cantActualHombres);
		printf("Hombre %i deja el baño \n",p);
		fflush(stdout);
		
		if (sem_trywait(&cantActualHombres) == -1) //si habia 1 solo hombre (el q se esta yendo), avisa que pueden pasar ya sean hombres o mujeres
		{
			pthread_mutex_unlock(&banio);
		}else{
			sem_post(&cantActualHombres); //por el que decremento el trywait
		}
		sem_post(&mutexHombre);
		sem_post(&maxHombres);	
		

	}
}

void descansar(int p){
		printf("%d Me toca descansar: \n",p);		
		sem_post(&semPersonas);
		sleep(5);	
}

void que_hacer(int p){

	int rnd = rdtsc() % 89;
	//printf("Random es: %i\n",rnd);
   	if(rnd>=0 && rnd<=30){
        	trabajar(p);
		}
  	  	else{ 	if(rnd>=31 && rnd<=60){
				descansar(p);
		}       		
      		else{
           	        uso_gabinete(p);
		}
		}

}

void *persona(void *p){
	
	int id_persona= *(int*)p;
	while(1){
		sem_wait(&semPersonas);
   		que_hacer(id_persona);
		sleep(3);
	}

}

int main()
{
 
    sem_init(&semPersonas,0,1);	
    sem_init(&espera,0,2);	
    sem_init(&maquinaEncendida,0,0);
    sem_init(&tercero,0,0);
    sem_init(&trabajando,0,0);	

    sem_init(&maxMujeres,0,2);	
    sem_init(&maxHombres,0,2);	
    sem_init(&mutexMujer,0,1);	
    sem_init(&mutexHombre,0,1);	
    sem_init(&cantActualHombres,0,0);	
    sem_init(&cantActualMujeres,0,0);	     
  

    pthread_mutex_init(&banio,0);
    pthread_mutex_init(&seccionCritica,0);


    int personas[CANTP];
    pthread_t personas_t[CANTP];
    int i;
    for (i=0; i<CANTP; i++)
        personas[i] = i;


    for (i=0; i<CANTP; i++)
        pthread_create(&personas_t[i], NULL, persona, (void *)&personas[i]);

    //Espero a que todos finalicen	
    for (i=0; i<CANTP; i++)
        pthread_join(personas_t[i],NULL);
}
