#include "comun.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

/*Estructuras auxiliares que usaremos para almacenar informacion.*/
/*****************************************************************/
/*Estructura donde almacenamos los datos de los subscriptores.*/
typedef struct sub{
	int id;
	char addr[32];
	int port;
} Subscriptor;

/*Estructura donde almacenamos cada tema, y sus subscriptores.*/
typedef struct topic{
	int id;
	char name[64];
	int numSubs;
	int *subsId; //Cada tema tiene asociada una lista con sus subscriptores.
} Topic;
/*****************************************************************/

/*Atributos que usaremos para la ejecucion de intermediario.*/
/*****************************************************************/
Topic *topicsList; //Lista dinamica de temas.
Subscriptor *subsList;
int nTopics, nSubs; //Numero de temas que tenemos.
/*****************************************************************/

/*********************/
/* METODOS AUXILIARES*/
/*********************/

/*Metodos de busqueda*/
/*****************************************************************/
/*Devuelve el id del tema, o -1 si fallo.*/
int searchTopic(char *topic){
	int i, id = -1;
	for (i = 0; i < nTopics && id == -1; i++){
		/*Comparamos los nombres para comprobar si existe el tema.*/
		if (strcmp(topicsList[i].name,topic) == 0) id = i;
	}
	return id;
}

/*Devuelve el id de un subscriptor o -1 si fallo.*/
int searchSub(char *sub, int puerto){
	int i, id = -1;
	for (i = 0; i < nSubs && id == -1; i++){
		/*Comparamos los nombres para comprobar si existe el tema.*/
		if (strcmp(subsList[i].addr,sub) == 0 && subsList[i].port == puerto) id = i;
	}
	return id;
}

/*Devuelve si el subscriptor esta en el tema y su posicion en la lista, o -1 si fallo.*/
int searchSubInTopic(char *topic, int idSub){
	int i, tmp, id = -1;
	if ((tmp = searchTopic(topic)) == -1) return -1;
	for (i = 0; i < topicsList[tmp].numSubs && id == -1; i++){
		/*Comparamos las direcciones de los subscriptores para comprobar si existe.*/
		if (topicsList[tmp].subsId[i] == idSub) id = i;
	}
	return id;
}
/*****************************************************************/

/*Metodos de comprobacion.*/
/*Los usaremos en pruebas para comprobar que el funcionamiento es correcto.*/
/*****************************************************************/
/*Imprime una lista de cada tema con su id y el numero de subscriptores.*/
void printTemas(){
	int i;
	printf("+-------------------------------+---------------+\n");
	printf("| TopicId \t| Topic \t| N_subs \t|\n");
	for(i=0; i<nTopics; i++){
		printf("| %d \t\t| %s \t| %d \t\t|\n", topicsList[i].id, topicsList[i].name, topicsList[i].numSubs);
	}
}

/*Imprime una lista con todos los Subscriptores inscritos en el sistema.*/
void printSubs(){
	int i;
	printf("+-------------------------------+---------------+\n");
	printf("| SubId \t| SubAddr \t| SubPort \t|\n");
	for(i=0; i<nSubs; i++){
		printf("| %d \t\t| %s \t| %d \t\t|\n", subsList[i].id, subsList[i].addr, subsList[i].port);
	}
}

/*Imprime una lista de los subscriptores de cada tema, su direccion y su id.*/
void printSubscripciones(){
	int i, j;
	printf("+-------------------------------+\n");
	printf("| Topic \t| IDSub \t\t\t|\n");
	for (i=0; i < nTopics; i++){
		for (j=0; j < topicsList[i].numSubs; j++){
			printf("| %d \t\t| %d \t\t|\n", topicsList[i].id, topicsList[i].subsId[j]);
		}
	}
}
/*****************************************************************/

/*Metodos de insercion/borrado.*/
/*****************************************************************/
/*Insertamos el tema en la lista, o -1 si ya existe.*/
int insertTopic(char *topic){
	if(searchTopic(topic) != -1) return -1;

	int new_id = nTopics++;
	Topic new_topic;
	new_topic.id = new_id;
	new_topic.numSubs = 0;
	strcpy(new_topic.name, topic); //Copiamos el nombre del tema al atributo de la estructura.

	/*Reservamos espacio en memoria para la lista de subscriptores.*/
	new_topic.subsId = malloc(sizeof(int));
	if(new_topic.subsId == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}

	topicsList[new_id] = new_topic; //Añadimos el nuevo tema a la lista.

	/*Aumentamos el tamaño reservado en memoria para tener espacio para una estructura mas.*/
	topicsList = realloc(topicsList,(nTopics+1)*sizeof(struct topic));
	if(topicsList == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	return 0;
}

/*Borramos el tema, o -1 si no existe el subscriptor.*/
int removeTopic(char *tema){
	int id_tema, i;

	if ((id_tema = searchTopic(tema)) == -1) return -1;

	/*Liberamos el espacio asignado a la lista de subscriptores.*/
	free(topicsList[id_tema].subsId);

	/*Reordenamos la lista sobreponiendo estructuras, y reduciendo su id.*/
	for (i = id_tema; i < nTopics; i++){
		topicsList[i] = topicsList[i+1];
		topicsList[i].id--;
	}

	/*Reasignamos la memoria dinámica para borrar el espacio innecesario.*/
	topicsList = realloc(topicsList,(nTopics)*sizeof(struct topic));
	if(topicsList == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	nTopics--;

	return 0;
}

/*Insertamos el nuevo subscriptor al sistema y devuelve su id, o -1 si ya esta dentro.*/
int createSub(char *subAddr, int puerto){
	if(searchSub(subAddr, puerto) != -1) return -1;

	int new_id = nSubs++;
	Subscriptor new_sub;
	new_sub.id = new_id;
	strcpy(new_sub.addr,subAddr); //Copiamos el atributo de la direccion al subscriptor.
	new_sub.port = puerto;

	/*Añadimos el nuevo subscriptor a la lista.*/
	subsList[new_id] = new_sub;

	/*Aumentamos el tamaño reservado en memoria para tener espacio para una estructura mas.*/
	subsList = realloc(subsList,(nSubs+1)*sizeof(struct sub));
	if(subsList == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}

	return new_id;
}

/*Borramos el subscriptor del sistema, o -1 si no existe el subscriptor.*/
int removeSub(char *subAddr, int puerto){
	int id_tema, i;

	if ((id_tema = searchSub(subAddr, puerto)) == -1) return -1;

	/*Reordenamos la lista sobreponiendo estructuras, y reduciendo su id.*/
	for (i = id_tema; i < nSubs; i++){
		subsList[i] = subsList[i+1];
		subsList[i].id--;
	}

	/*Reasignamos la memoria dinámica para borrar el espacio innecesario.*/
	subsList = realloc(subsList,(nSubs)*sizeof(struct sub));
	if(subsList == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	nSubs--;

	return 0;
}

/*Insertamos el subscriptor en la lista del tema, o -1 si no hay tema o ya esta subscrito.*/
int altaSub(int subscriptor, char *tema){
	int id_tema;

	if ((id_tema = searchTopic(tema)) == -1) return -1; //Buscamos que exista el tema.
	if (searchSubInTopic(tema, subscriptor) != -1) return -1; //Vemos si el subscriptor esta ya subscrito.

	int n_subs = topicsList[id_tema].numSubs++;

	topicsList[id_tema].subsId[n_subs] = subscriptor; //Lo añadimos a la lista de subscritos.

	/*Aumentamos el tamaño reservado en memoria para tener espacio para una estructura mas.*/
	topicsList[id_tema].subsId = realloc(topicsList[id_tema].subsId,(topicsList[id_tema].numSubs+1)*sizeof(int));
	if(topicsList[id_tema].subsId == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}

	return 0;
}

/*Borramos el subscriptor de la lista del tema, o -1 si no existe el subscriptor.*/
int bajaSub(int subscriptor, char *tema){	
	int id_tema, id_sub, i;

	/*Comprobamos que existen tanto el tema como el subscriptor.*/
	if ((id_tema = searchTopic(tema)) == -1) return -1;
	if ((id_sub = searchSubInTopic(tema, subscriptor)) == -1) return -1;

	/*Reordenamos la lista sobreponiendo estructuras, y reduciendo su id.*/
	for (i = id_sub; i < topicsList[id_tema].numSubs; i++) topicsList[id_tema].subsId[i] = topicsList[id_tema].subsId[i+1];

	/*Reasignamos la memoria dinámica para borrar el espacio innecesario.*/
	topicsList[id_tema].subsId = realloc(topicsList[id_tema].subsId,(topicsList[id_tema].numSubs)*sizeof(int));
	if(topicsList[id_tema].subsId == NULL){
		fprintf(stderr, "No se pudo ubicar la memoria dinamica necesaria\n");
		return -1;
	}
	topicsList[id_tema].numSubs--;

	return 0;
}
/*****************************************************************/

/*Metodos de envio.*/
/*****************************************************************/
/*Lo usaremos para notificar a los subscriptores cuando se reciba un evento, o se deba comunicar una adición o borrado de tema.*/
int notificar(int op, char *tema, char *mensaje){
	int sock_not, i;
	struct sockaddr_in sock_addr;
	Evento event;

	crear_evento(op, 0, tema, mensaje, &event);

	if (op == EVENTO){
		if (searchTopic(tema) == -1) return -1;
	
		for(i=0; i < nSubs; i++){
			if (searchSubInTopic(tema, subsList[i].id) == -1) continue;
			if ((sock_not = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) return -1;

			bzero((char *) &sock_addr, sizeof(sock_addr));  // Inicializar estructura

			sock_addr.sin_family = AF_INET;
			sock_addr.sin_port = subsList[i].port;
			inet_aton(subsList[i].addr, &sock_addr.sin_addr);

			if(connect(sock_not,(struct sockaddr*) &sock_addr,sizeof(struct sockaddr_in))<0) continue;

			/* Mandar evento a suscriptor i*/
			/* Abrir conexion con nuevo socket tcp y puerto de notificaciones */
			send(sock_not,&event,sizeof(struct evento),0);
			close(sock_not);
		}
	}
	else{
		for(i=0; i < nSubs; i++){
			if((sock_not = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) return -1;

			bzero((char *) &sock_addr, sizeof(sock_addr));  // Inicializar estructura

			sock_addr.sin_family = AF_INET;
			sock_addr.sin_port = subsList[i].port;
			sock_addr.sin_addr.s_addr = inet_addr(subsList[i].addr);

			if(connect(sock_not,(struct sockaddr*) &sock_addr,sizeof(struct sockaddr_in))<0) continue;

			/* Mandar evento a suscriptor i*/
			/* Abrir conexion con nuevo socket tcp y puerto de notificaciones */
			send(sock_not,&event,sizeof(struct evento),0);
			close(sock_not);
		}
	}
	return 0;
}

/*Lo usaremos para notificar a un nuevo Subscriptor los temas que ya existen.*/
int notificar_temas(int op){
	int sock_not, i;
	struct sockaddr_in sock_addr;
	Evento event;

	/*Recorremos los temas para enviar al subscriptor un mensaje con cada uno de ellos.*/
	for(i=0; i < nTopics; i++){
		bzero((Evento *)&event, sizeof(Evento));
		crear_evento(ALTA_TEMA, 0, topicsList[i].name, "", &event);

		if ((sock_not = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) return -1;

		bzero((char *) &sock_addr, sizeof(sock_addr));  // Inicializar estructura

		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = subsList[op].port;
		inet_aton(subsList[op].addr, &sock_addr.sin_addr);

		if(connect(sock_not,(struct sockaddr*) &sock_addr,sizeof(struct sockaddr_in))<0) continue;

		/* Mandar evento a suscriptor i*/
		/* Abrir conexion con nuevo socket tcp y puerto de notificaciones */
		send(sock_not,&event,sizeof(struct evento),0);

		close(sock_not);
	}
	return 0;
}
/*****************************************************************/

/**********************/
/* PROGRAMA PRINCIPAL */
/**********************/

/*****************************************************************/
int main(int argc, char *argv[]) {

	/*Comprobamos errores en la ejecucion del programa.*/
	if (argc!=3) {
		fprintf(stderr, "Uso: %s puerto fichero_temas\n", argv[0]);
		return 1;
	}

	FILE *fichero;
	char linea[64];
	nTopics = 0;
	nSubs = 0;

	/*Abrimos el fichero de temas para su lectura.*/
	if ((fichero = fopen(argv[2],"r")) == NULL){
		fprintf(stderr,"Fichero de temas no disponible\n");
		return -1;
	}

	/*Asignamos espacio en memoria para la lista de temas.*/
	topicsList = malloc(sizeof(struct topic));
	subsList = malloc(sizeof(struct sub));

	/*Recorremos el fichero de temas para añadirlos a la lista.*/
	while (fgets(linea,64,fichero)!= NULL){
		/*Cortamos de esta forma el resto de la linea.*/
		char *c = strchr(linea, '\n');
		if (c) *c = 0;
		/*Lo insertamos.*/
		insertTopic(linea);
	}

	fclose(fichero);

	int socket_in; /*Descriptor del socket.*/
	struct sockaddr_in socket_addr; /*Estructuras para el control de entradas y salidas.*/

	/*Creamos el socket al que se irán conectando los elementos.*/
	if((socket_in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) return -1;

  	/* Limpiamos el espacio asignado a la estructura.*/    
	bzero((char *) &socket_addr, sizeof(socket_addr));

	/*Asignamos el tipo de conexion y el puerto que tendrá.*/
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = INADDR_ANY;
	socket_addr.sin_port = htons(atoi(argv[1]));

	/*Asignamos la dirección.*/
	if(bind(socket_in, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0){
		close(socket_in);
		return -1;
	}

  	/* Aceptamos conexiones por el socket.*/
	if(listen(socket_in,10) < 0) return -1;

	///*Nos quedamos de forma indefinida esperando mensajes.*/
	Evento mensaje;
	struct sockaddr_in conexion;
	int result, socket_rec, size = sizeof(struct sockaddr_in);

	while(1){
		//printSubs();
		//printf("\n");
		//printTemas(); /*Imprimimos la lista de temas.*/
		//printf("\n");
		//printSubscripciones();
		//printf("\n");
		bzero((char *)&conexion, sizeof(conexion));
		/*Buscamos una conexion entrante.*/
		if((socket_rec=accept(socket_in, (struct sockaddr *) &conexion, (socklen_t *) &size)) < 0) continue;
		else{ 
			/*Recibimos el mensaje y lo almacenamos.*/
			recv(socket_rec,(Evento *)&mensaje,sizeof(Evento),0);
			switch(ntohs(mensaje.codigo)){
				case EVENTO:
					/*En caso de ser un evento, notificamos a los subscritos el mensaje.*/
					result = searchTopic(mensaje.tema);
					send(socket_rec,&result,sizeof(int),0);
					notificar(EVENTO, mensaje.tema, mensaje.mensaje);
					close(socket_rec);
					break;
				case ALTA_SUB:
					/*Subscribimos al tema indicado.*/
					if ((result = searchSub(inet_ntoa(conexion.sin_addr), mensaje.puerto)) != -1) result = altaSub(result, mensaje.tema);
					send(socket_rec,&result,sizeof(int),0);
					close(socket_rec);
					break;
				case BAJA_SUB:
					/*Damos de baja del tema.*/
					if ((result = searchSub(inet_ntoa(conexion.sin_addr), mensaje.puerto)) != -1) result = bajaSub(result, mensaje.tema);
					send(socket_rec,&result,sizeof(int),0);
					close(socket_rec);
					break;
				case CREAR_SUB:
					/*Creamos un nuevo Subscriptor.*/
					if ((result = searchSub(inet_ntoa(conexion.sin_addr), mensaje.puerto)) == -1) {
						result = createSub(inet_ntoa(conexion.sin_addr), mensaje.puerto);
						notificar_temas(searchSub(inet_ntoa(conexion.sin_addr), mensaje.puerto));
					}
					send(socket_rec,&result,sizeof(int),0);
					close(socket_rec);
					break;
				case BORRAR_SUB:
					/*Borramos el Subscriptor.*/
					if ((result = (searchSub(inet_ntoa(conexion.sin_addr), mensaje.puerto)) != -1)) result = removeSub(inet_ntoa(conexion.sin_addr), mensaje.puerto);
					send(socket_rec,&result,sizeof(int),0);
					close(socket_rec);
					break;
				case ALTA_TEMA:
					/*Creamos nuevo tema y notificamos a todos los subscriptores.*/
					result = insertTopic(mensaje.tema);
					send(socket_rec,&result,sizeof(int),0);
					if (result != -1) notificar(ALTA_TEMA, mensaje.tema, "");
					close(socket_rec);
					break;
				case BAJA_TEMA:
					/*Borramos un tema y notificamos a todos los subscriptores.*/
					result = removeTopic(mensaje.tema);
					send(socket_rec,&result,sizeof(int),0);
					notificar(BAJA_TEMA, mensaje.tema, "");
					close(socket_rec);
					break;
				default:
					close(socket_rec);
					break;
			}
		}
	}
	free(topicsList);
	return 0;
}
/*****************************************************************/
