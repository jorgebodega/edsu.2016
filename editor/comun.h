/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

#define NULL '\0'
#define EVENTO 0
#define ALTA_SUB 1
#define BAJA_SUB 2
#define CREAR_SUB 3
#define BORRAR_SUB 4
#define ALTA_TEMA 5
#define BAJA_TEMA 6

/* Mensaje de alta, baja o evento */
typedef struct evento{
	int codigo;
	int puerto;
	char tema[64];
	char mensaje[64];
} Evento;

void crear_evento(int cod, int puerto, char *topic, char *msj, Evento *evento);
