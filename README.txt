/**
**   PROYECTO 1 Sistemsas operativos
**    Joaquin Piersigilli, Utizi Sebastian
**/

Observaciones generales:


- Para poder ejecutar desde consola aquellos programas que utilicen hilos, se debe agregar la libreria
	como parametro adicional al momento de realizar la compilacion.
	Este parametro adiconal se agrega como: -lpthread
	Por ejemplo:
		$ gcc  -o  1.2.4.Barbero  1.2.4.Barbero.c  -lpthread
		$ ./1.2.3Barbero
	Ninguno de los programas requiere parametros adicionales para ejecutarse.

- Para el ejercicio MiniShell decidimos implementar los siguientes comandos: cd, ls, rm, time, exit
	En particular para el comando cd las direcciones deben comenzar siempre con /
	Por ejemplo:
		cd /Desktop
		cd /Desktop/Proyecto

- Para ejercicio fabrica DCIC, existen dos versiones una en linux y otra para windows
