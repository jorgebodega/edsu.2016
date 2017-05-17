/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

#include "comun.h"
#include <string.h>
#include <arpa/inet.h>

void crear_evento(int cod, int port, char *topic, char *msj, Evento *evento){

	/*Inicializamos estructura.*/
	bzero((void*)evento, sizeof(evento));

	/*Escribimos los datos de los mensajes.*/
	evento->codigo = htons(cod);
	evento->puerto = port;
	if (strcmp(topic,"") != 0) strcpy(evento->tema, topic);
	if (strcmp(msj,"") != 0) strcpy(evento->mensaje, msj);
}

