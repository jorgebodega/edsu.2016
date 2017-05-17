/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/

#include "comun.h"
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

/*Funcion para obtener el puerto guardado en una variable de entorno.*/
int obtener_puerto(int *puerto);

/*Funcion para obtener el servidor guardado en una variable de entorno.*/
int obtener_servidor(char **servidor);

/*Funcion que devuelve el descriptor de un nuevo socket, o -1 si fallo.*/
int conexion();
