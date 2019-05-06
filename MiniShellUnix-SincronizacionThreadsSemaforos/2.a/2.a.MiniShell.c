/**
** Ejercicio miniShell - Linux
** Joaquin Pergisilli
** Utizi Sebastian
**/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#define TRUE 1

int main(int argc,char *argv[])
{
    char* comando = (char*)malloc(sizeof(char)*50);
    char* argumento = (char*)malloc(sizeof(char)*50);
    char directorio[50];

    while(TRUE)
    {
        getcwd(directorio,sizeof(directorio));
        printf("%s-shellSO$: ",directorio);
        scanf("%s",comando);

        //Determino que comando fue ingresado
        if(strcmp(comando,"exit")==0)
        {
            exit(0);
        }
        else
        {
            if(strcmp(comando,"rm")==0)
            {
                scanf("%s",argumento);
                int e =remove(argumento);
                if(e !=0)
                    printf("bash: cd: %s: No such file or directory\n", argumento);
            }
            else
            {
                if(strcmp(comando,"ls")==0)
                {

                    DIR *d;
                    struct dirent *dire;
                    d = opendir(directorio);
                    if (d)
                    {
                        while ((dire = readdir(d)) != NULL)
                        {
                            if( strcmp(dire->d_name,".") != 0)
                                printf("%s   ", dire->d_name);
                        }
                        printf("\n");
                        closedir(d);
                    }


                }
                else{

                    if(strcmp(comando,"cd")==0)
                    {
                        scanf("%s",argumento);
                        if(strcmp(argumento,"..")==0)
                    {
                        chdir("..");
                    }
                    else
                    {
                        strcat(directorio,argumento);
                        int e = chdir(directorio);
                        if(e!=0)
                            printf("bash: cd: %s: No such file or directory\n", argumento);
                    }
}
                    else
                    {
                        if(strcmp(comando,"time")==0)
                        {
                            time_t t;
                            struct tm * hora;
                            time(&t);
                            hora = localtime(&t);
                            printf("Fecha y hora actual: %s", asctime(hora));
                        }
                        else
                        {
                            printf("el comando ingresado no es valido \n");
                        }
                    }
                }
            }
        }


    }


    return 1;
}
