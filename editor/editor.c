#include "editor.h"
#include "edsu_comun.h"

int generar_evento(const char *tema, const char *valor);
/* solo para la version avanzada */
int crear_tema(const char *tema);
/* solo para la version avanzada */
int eliminar_tema(const char *tema);


int generar_evento(const char *tema, const char *valor) {
	int sock, resultado;
	/*Creamos el socket para la conexion.*/
	if ((sock = conexion()) == -1) return -1;

	/*Creamos un nuevo mensaje e inscribimos en el los valores para asi enviarlo.*/
	Evento evento;
	crear_evento(EVENTO, 0, (char *)tema, (char *)valor, &evento);

	/*Enviamos el mensaje por el socket.*/
	send(sock, &evento, sizeof(Evento), 0);

	/*Recibimos la respuesta por el socket y la almacenamos en una variable.*/
	recv(sock, &resultado, sizeof(int), 0);
	close(sock);

	return resultado;
}

/* solo para la version avanzada */
int crear_tema(const char *tema) {
	int sock, resultado;
	/*Creamos el socket para la conexion.*/
	if ((sock = conexion()) == -1) return -1;

	/*Creamos un nuevo mensaje e inscribimos en el los valores para asi enviarlo.*/
	Evento evento;
	crear_evento(ALTA_TEMA, 0, (char *)tema, "", &evento);

	/*Enviamos el mensaje por el socket.*/
	send(sock, &evento, sizeof(Evento), 0);

	/*Recibimos la respuesta por el socket y la almacenamos en una variable.*/
	recv(sock, &resultado, sizeof(int), 0);
	close(sock);

	return resultado;
}

/* solo para la version avanzada */
int eliminar_tema(const char *tema) {
	int sock, resultado;
	/*Creamos el socket para la conexion.*/
	if ((sock = conexion()) == -1) return -1;

	/*Creamos un nuevo mensaje e inscribimos en el los valores para asi enviarlo.*/
	Evento evento;
	crear_evento(BAJA_TEMA, 0, (char *)tema, "", &evento);

	/*Enviamos el mensaje por el socket.*/
	send(sock, &evento, sizeof(Evento), 0);

	/*Recibimos la respuesta por el socket y la almacenamos en una variable.*/
	recv(sock, &resultado, sizeof(int), 0);
	close(sock);

	return resultado;
}

