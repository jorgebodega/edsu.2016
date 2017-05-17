/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/

#include "edsu_comun.h"

int obtener_puerto(int *puerto){
  /*Asignamos el valor de la variable de entorno PUERTO al argumento.*/
  if (getenv("PUERTO")!=NULL) {
    *puerto=strtol(getenv("PUERTO"),NULL,10);
    return 0;
  }
  /*En caso de no existir la variable de entorno retornamos error.*/
  else return -1;
}

int obtener_servidor(char **servidor){
  /*Asignamos la variable de entorno SERVIDOR al argumento. Si no existe, retornamos error. */
  return ((*servidor = getenv("SERVIDOR"))!=NULL)?  0 : -1;
}

int conexion(){
  /*Asignamos el puerto en la direccion de memoria.*/
  int puerto_conex;
  if(obtener_puerto(&puerto_conex) != 0) return -1;

  /*Asignamos el servidor.*/
  char *servidor;
	if(obtener_servidor(&servidor) != 0) return -1;

  /*Usamos la estructura hostent donde almacenaremos el host del servidor.*/
  struct hostent *host;
	if((host = gethostbyname(servidor)) == NULL) return -1;

  /*Usamos sd como descriptor de socket para manejar la conexion.*/
  int sd;
  if ((sd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0) return -1;

  /*Creamos un handler para la conexión.*/
  struct sockaddr_in handler;
  /*Limpiamos el espacio de la estructura.*/
  bzero((char *)&handler,sizeof(handler));

  /*Establecemos los parametros para la conexion con el intermediario.*/
  handler.sin_family = AF_INET; //Tipo de conexion
  memcpy(&(handler.sin_addr),host->h_addr,(size_t)sizeof(host->h_addr)); //Copiamos los addr
  handler.sin_port = htons(puerto_conex); //Asiganamos el puerto

  /*Conectamos el socket.*/
  if(connect(sd,(struct sockaddr*) &handler,sizeof(struct sockaddr_in))<0){
		close(sd); //En caso de error cerramos el descriptor y devolvemos error.
		return -1;
	}

  /*Devolvemos el descriptor del socket si todo ha ido bien.*/
	return sd;
}
