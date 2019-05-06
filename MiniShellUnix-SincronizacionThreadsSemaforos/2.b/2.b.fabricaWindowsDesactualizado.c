/**
    Ejercicio 2.2b Fabrica DCIC
    Joaquin Piersigilli, Utizi Sebastian
**/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <pthread.h>
#include <time.h>
#define CANTP 8

HANDLE semPersonas, semEspera1, semEspera2, semEsperaBanio; //semaforos

int cantEnMaquina = 0;
int maquinaTrabajando = 0;
int cantEsperando = 0;
int gabinetesLibres = 2;

struct gabinete{
	int ocupado;
	char sexo[1];
}*g1, *g2;

struct banio {
	struct gabinete gabinete1;
	struct gabinete gabinete2;
}*b;


void trabajar(int id_persona){
	printf("%d Me toca trabajar: \n",id_persona);
   	cantEnMaquina++;
	if(maquinaTrabajando){
		printf("Se sumo %i a la maquina, ahora hay %i personas\n",id_persona, cantEnMaquina);
		ReleaseSemaphore(semPersonas, 1, NULL);
		Sleep(5000);
	}else{
		if(cantEnMaquina<3){
			printf("Se sumo %i a la maquina, ahora hay %i personas por lo tanto ",id_persona, cantEnMaquina);
			printf("falta gente para encender la maquina, %i espera\n",id_persona);
			ReleaseSemaphore(semPersonas, 1, NULL);
			
			if(cantEnMaquina==1)
				WaitForSingleObject(semEspera1, INFINITE);
			if(cantEnMaquina==2)
				WaitForSingleObject(semEspera2, INFINITE);
			Sleep(4000);
		}else{
			printf("Se sumo %i a la maquina, ahora hay %i personas por lo tanto ",id_persona, cantEnMaquina);
			printf("se enciende la maquina\n");
			maquinaTrabajando=1;
			ReleaseSemaphore(semEspera1, 1, NULL);
			Sleep(1000);
			ReleaseSemaphore(semEspera2, 1, NULL);
			ReleaseSemaphore(semPersonas, 1, NULL);
			Sleep(3000);	
		}	
	     }
	

	cantEnMaquina--;
	printf("La persona %i dejo de trabajar, ahora hay %i personas en la maquina\n",id_persona, cantEnMaquina);	
	if(cantEnMaquina==0){
		maquinaTrabajando=0;
		printf("No queda gente en la maquina, esta se ha apagado\n");	
	}

}


// Indica el sexo de una persona
//  Retorna H si es hombre
//  Retorna M si es mujer
char que_soy(int id_persona){
    //Asumimos que si el id de la persona es par entonces es mujer
    if (id_persona %2 == 0)
        return 'M';
    else //Si es impar entonces es hombre
        return 'H';
}

void uso_gabinete(int p){	
	printf("%d Me toca ir al baño \n",p);
	char sex = que_soy(p);
	int useGabinete = 0;
	while(!useGabinete){
		
		if(gabinetesLibres==2){
			gabinetesLibres--;		
			b->gabinete1.sexo[1]=sex;
			b->gabinete1.ocupado=1;
			printf("%i, %c, Ocupo un gabinete, hay %i libres \n",p,sex,gabinetesLibres);
			ReleaseSemaphore(semPersonas, 1, NULL);
			Sleep(4000);
			b->gabinete1.ocupado=0;
			gabinetesLibres++;
			useGabinete++;
			printf("%i Desocupo un gabinete, hay %i libres \n",p,gabinetesLibres);
		}else if(gabinetesLibres==1){
			int gabolibre; //numero de gabinete libre
			char sexgabo;  //sexo de la persona en el gabinete ocupado
			if (b->gabinete1.ocupado){			
				sexgabo = b->gabinete1.sexo[1];
				gabolibre = 2;
			}else{
				sexgabo = b->gabinete2.sexo[1];
				gabolibre = 1;

			}
			if(sex==sexgabo){
				gabinetesLibres--;				
				printf("%i, %c, Ocupo un gabinete, hay %i libres \n",p,sex,gabinetesLibres);

				if(gabolibre==1){
					b->gabinete1.sexo[1]=sex;
					b->gabinete1.ocupado=1;
				}
				else {
					b->gabinete2.sexo[1]=sex;
					b->gabinete2.ocupado=1;
				}

				ReleaseSemaphore(semPersonas, 1, NULL);
				Sleep(4000);
				gabinetesLibres++;
				
				if(gabolibre == 1) 
					b->gabinete1.ocupado=0; //dejo libre el gabinete de nuevo.
				else b->gabinete2.ocupado=0; //dejo libre el gabinete de nuevo.


				useGabinete++;	
				printf("%i Desocupo un gabinete, hay %i libres \n",p,gabinetesLibres);
			}else{
				printf("%i, %c, No pudo entrar al baño porque esta ocupado por alguien del sexo opuesto\n",p,sex);
				ReleaseSemaphore(semPersonas, 1, NULL);
				Sleep(2000);
		    	 }

		      }else if(gabinetesLibres==0){
				printf("%i No pudo entrar al baño porque estan los 2 gabinetes ocupados\n",p);
				ReleaseSemaphore(semPersonas, 1, NULL);
				Sleep(2000);
			     }
	}	
}

//no tocar esta funcion que tarde muchisimo en encontrar algo que genere randoms distintos
unsigned long long rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}


void descansar(int p){
	printf("%d Me toca descansar \n",p);		
		    ReleaseSemaphore(semPersonas, 1, NULL);
		    Sleep(10000);
	
}

void que_hacer(int p){ 

	int rnd = rdtsc() % 89;
	//printf("Random es: %lu\n",rnd);
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

DWORD WINAPI persona(void *p){
	

	int id_persona= pthread_self();

	while(1){
		WaitForSingleObject(semPersonas, INFINITE);
   		que_hacer(id_persona);
		Sleep(3000);
	}

}

HANDLE CreateNewSemaphore(LPSECURITY_ATTRIBUTES lpsa, LONG cInitial, LONG cMax, LPTSTR lpszName){
    HANDLE hSem;
    // create or open a named semaphore.
    hSem = CreateSemaphore(
        lpsa,               // security attributes, NULL = handle cannot be inherited
        cInitial,            // initial count
        cMax,             // maximum count
        lpszName);    // semaphore name

    // close handle, and return NULL if existing semaphore opened.
    if (hSem == NULL && GetLastError() == ERROR_ALREADY_EXISTS){
        CloseHandle(hSem);
        return NULL;
    }
    // if new semaphore was created, return the handle.
    return hSem;
}


int main(){
	srand((unsigned)time(NULL));
	struct gabinete gab1 = {0," "};
    g1 = &gab1;  
    struct gabinete gab2 = {0," "};
    g2 = &gab2;	
    struct banio ban = {gab1, gab2};
    b = &ban;
	
	semPersonas = CreateNewSemaphore(NULL, 1, CANTP, "semPersonas");
    semEspera1 = CreateNewSemaphore(NULL, 0, CANTP, "semEspera1");
    semEspera2 = CreateNewSemaphore(NULL, 0, CANTP, "semEspera2");
	semEsperaBanio = CreateNewSemaphore(NULL, 0, CANTP, "semEsperaBanio");

	HANDLE  hThreadArray[CANTP];
	int i;

    for(i=0; i<CANTP; i++){
        hThreadArray[i]=CreateThread(NULL, 0, persona, NULL, 0, NULL);

    }
	
	WaitForMultipleObjects(CANTP, hThreadArray, TRUE, INFINITE);
	
	free(g1);
    free(g2);
    free(b);
	
	return 0;
}
