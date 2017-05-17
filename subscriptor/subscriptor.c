#include "edsu_comun.h"
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

/*Estructuras auxiliares que usaremos para almacenar informacion.*/
/*****************************************************************/
int sock_env, sock_int, port_recv; //Descriptores de los sockets y puerto
struct sockaddr_in socket_addr, tcp_port; //Estructuras donde almacenaremos la informacion para la conexion.
/*Funciones que usaremos para almacenar los parametros al iniciar sesión y asi ejecutar el thread.*/
void (*func_notif)(const char*, const char*); 
void (*func_alta)(const char*);
void (*func_baja)(const char*);
int rec_connection(); //Método del Thread.
int alta_subscripcion_tema(const char *tema);
int baja_subscripcion_tema(const char *tema);
/* para la version inicial solo se usa el primer argumento dejando los restantes a NULL */
int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *));                           
/* solo para la version avanzada */
int fin_subscriptor(); 
/*****************************************************************/

/*Metodos de alta/baja.*/
/*****************************************************************/
/*Damos de alta al subscriptor al tema.*/
int alta_subscripcion_tema(const char *tema) {
	int sock, resultado;
	/*Creamos el socket para la conexion.*/
	if ((sock = conexion()) == -1) return -1;

	/*Creamos un nuevo mensaje e inscribimos en el los valores para asi enviarlo.*/
	Evento evento;
	crear_evento(ALTA_SUB, port_recv, tema, "", &evento);

	/*Enviamos el mensaje por el socket.*/
	send(sock, &evento, sizeof(Evento), 0);

	/*Recibimos la respuesta por el socket y la almacenamos en una variable.*/
	recv(sock, &resultado, sizeof(int), 0);
	close(sock);

	return resultado;
}

/*Damos de baja al subscriptor del tema.*/
int baja_subscripcion_tema(const char *tema) {
	int sock, resultado;
	/*Creamos el socket para la conexion.*/
	if ((sock = conexion()) == -1) return -1;

	/*Creamos un nuevo mensaje e inscribimos en el los valores para asi enviarlo.*/
	Evento evento;
	crear_evento(BAJA_SUB, port_recv, tema, "", &evento);

	/*Enviamos el mensaje por el socket.*/
	send(sock, &evento, sizeof(Evento), 0);

	/*Recibimos la respuesta por el socket y la almacenamos en una variable.*/
	recv(sock, &resultado, sizeof(int), 0);
	close(sock);

	return resultado;
}
/*****************************************************************/

/*INICIAMOS SUBSCRIPTOR.*/
/*****************************************************************/
int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *)) {
	int resultado;
	struct sockaddr_in tcp_port; /*Estructuras para el control de entradas y salidas.*/

	/*Creamos el socket que recibirá datos de intermediario.*/
	if((sock_int = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) return -1;

  	/* Limpiamos el espacio asignado a la estructura.*/    
	bzero((char *) &socket_addr, sizeof(socket_addr));

	/*Asignamos el tipo de conexion y el puerto que tendrá.*/
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = INADDR_ANY;

	/*Asignamos la dirección.*/
	if(bind(sock_int, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0){
		close(sock_int);
		return -1;
	}

	/* Aceptamos conexiones por el socket.*/
	if(listen(sock_int,1) < 0) return -1;

	/*Igualamos las funciones declaradas para poder ejecutar desde el thread*/
	func_notif = notif_evento;
	func_alta = alta_tema;
	func_baja = baja_tema;

	/*Creamos el thread y lo lanzamos.*/
	pthread_t threadEvents;
	pthread_attr_t attr_threadEvents;
	pthread_attr_init(&attr_threadEvents);
	pthread_attr_setdetachstate(&attr_threadEvents, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadEvents, &attr_threadEvents, (void *)rec_connection, NULL);

	/*Creamos el socket para la conexion.*/
	if ((sock_env = conexion()) == -1) return -1;	

	int sa_len = sizeof(tcp_port);
	bzero((char *) &tcp_port, sizeof(tcp_port));
	getsockname(sock_int, (struct sockaddr *) &tcp_port, (socklen_t *) &sa_len);

	/*Creamos un nuevo mensaje e inscribimos en el los valores para asi enviarlo.*/
	Evento evento;
	port_recv = tcp_port.sin_port; //Obtenemos el puerto por el que recibiremos de intermediario.
	crear_evento(CREAR_SUB, port_recv, "", "", &evento);

	/*Enviamos el mensaje por el socket.*/
	send(sock_env, &evento, sizeof(Evento), 0);

	/*Recibimos la respuesta por el socket y la almacenamos en una variable.*/
	recv(sock_env, &resultado, sizeof(int), 0);

	if (resultado == -1) return -1;

	close(sock_env);  	

	return 0;
}
/*****************************************************************/

/*Mandamos una señal al intermediario para indicarle el fin del subscriptor.*/
/*****************************************************************/
int fin_subscriptor() {
	int sock, resultado;
	/*Creamos el socket para la conexion.*/
	if ((sock = conexion()) == -1) return -1;

	/*Creamos un nuevo mensaje e inscribimos en el los valores para asi enviarlo.*/
	Evento evento;
	crear_evento(BORRAR_SUB, port_recv, "", "", &evento);

	/*Enviamos el mensaje por el socket.*/
	send(sock, &evento, sizeof(Evento), 0);

	/*Recibimos la respuesta por el socket y la almacenamos en una variable.*/
	recv(sock, &resultado, sizeof(int), 0);
	close(sock);

	return resultado;
}
/*****************************************************************/

/*Metodo del thread que se quedará ejecutando en background.*/
/*****************************************************************/
int rec_connection(){
	Evento mensaje;
	int socket_rec, size = sizeof(struct sockaddr_in);

	while(1){
		/*Recibimos el mensaje y lo almacenamos.*/
		if((socket_rec=accept(sock_int, (struct sockaddr *) &socket_addr, (socklen_t *) &size)) < 0) continue;
		bzero((Evento *) &mensaje, sizeof(struct evento));
		recv(socket_rec,(Evento *)&mensaje,sizeof(struct evento),0);
		/*Buscamos que tipo de operación debe realizar.*/
		switch(ntohs(mensaje.codigo)){
			case EVENTO:
				func_notif(mensaje.tema, mensaje.mensaje);
				break;
			case ALTA_TEMA:
				if(func_alta != NULL)func_alta(mensaje.tema);
				break;
			case BAJA_TEMA:
				if(func_baja != NULL)func_baja(mensaje.tema);
				break;
			default:
				break;
		}
		close(socket_rec);
	}
	return 0;
}
/*****************************************************************/