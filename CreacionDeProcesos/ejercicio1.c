#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define CANTP 10

//Muestra el contenido de file por consola
void mostrarArchivo(FILE* file){
    char c;
     while((c = getc(file)) != EOF)
            putchar(c);
    fclose(file);
}

int main(){

    FILE *f = fopen("duracion.txt", "w+");
    if (f == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    //Creo un arreglo con todos los procesos
    pid_t conductores [CANTP];
    int i;
    for (i=0; i<CANTP; i++){
        conductores[i] = fork();
      if (conductores[i] < 0) {
            perror("fork");
            abort();
      }
      else
        if (conductores[i] == 0){
            srandom(i);
            int rnd = rand();
            fprintf(f, "conductor: %d duracion del viaje: %d\n", i, rnd);
            exit(0);
        }
    }//End for
    close((long)f);

    //Espero a que todos los hijos terminen
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0);

    //Abro el archivo en modo lectura
    FILE *file = fopen("duracion.txt", "r");
    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }

    mostrarArchivo(file);
    return 0;
}
