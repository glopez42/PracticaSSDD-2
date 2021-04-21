/* DMUTEX (2009) Sistemas Operativos Distribuidos
 * Codigo de Apoyo
 *
 * ESTE CODIGO DEBE COMPLETARLO EL ALUMNO:
 *    - Para desarrollar las funciones de mensajes, reloj y
 *      gestion del bucle de tareas se recomienda la implementacion
 *      de las mismas en diferentes ficheros.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int puerto_udp;

/*struct para guardar informacion de un proceso*/
struct proceso
{
	char nombre[80];
	int puerto;
};

/*lista para guardar procesos*/
struct nodo
{
	struct proceso proc;
	struct nodo *next;
};

struct lista
{
	struct nodo *inicio;
	struct nodo *final;
	int length;
} lista;

/*función que devuelve en p, el proceso de la lista*/
/*que coincida con el nombre dado*/
struct proceso getProceso(char *nombre)
{

	int found = -1;
	struct nodo *nodoActual;
	struct proceso res;
	nodoActual = lista.inicio;

	while (nodoActual != NULL && found == -1)
	{
		/*se comparan los nombres*/
		if (strcmp(nombre, nodoActual->proc.nombre) == 0)
		{
			found = 0;
			res = nodoActual->proc;
			continue;
		}
		/*si no se ha encontrado, sigue con el siguiente*/
		nodoActual = nodoActual->next;
	}
	if (found != 0)
		return res;
	else
		return res;
}

int addProceso(struct proceso *p)
{
	struct nodo *n;
	n = malloc(sizeof(struct nodo));
	n->proc = *p;

	if (lista.length == 0)
	{
		lista.inicio = n;
		lista.final = n;
	}
	else
	{
		lista.final->next = n;
		lista.final = n;
	}

	lista.length = lista.length + 1;
	return 0;
}

int main(int argc, char *argv[])
{
	int port, sck;
	socklen_t len;
	char line[80], proc[80];
	struct sockaddr_in addr;
	struct proceso *p, a;

	if (argc < 2)
	{
		fprintf(stderr, "Uso: proceso <ID>\n");
		return 1;
	}

	/* Establece el modo buffer de entrada/salida a l�nea */
	setvbuf(stdout, (char *)malloc(sizeof(char) * 80), _IOLBF, 80);
	setvbuf(stdin, (char *)malloc(sizeof(char) * 80), _IOLBF, 80);

	/*Preparamos socket*/
	if ((sck = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Error");
		return 1;
	}

	/*Preparamos sockaddr_in*/
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	/*Para que decida el sistema el puerto, se deja a 0*/
	addr.sin_port = 0;

	len = sizeof(addr);

	// realizamos bind con el socket
	if (bind(sck, (const struct sockaddr *)&addr, len) < 0)
	{
		perror("bind fallido");
		return 1;
	}

	/*cogemos el puerto del proceso*/
	getsockname(sck, (struct sockaddr *)&addr, &len);
	puerto_udp = addr.sin_port;

	fprintf(stdout, "%s: %d\n", argv[1], puerto_udp);

	for (; fgets(line, 80, stdin);)
	{
		if (!strcmp(line, "START\n"))
			break;

		sscanf(line, "%[^:]: %d", proc, &port);
		/* Habra que guardarlo en algun sitio */

		/*guardamos memoria para el proceso*/
		p = malloc(sizeof(struct proceso));
		strcpy(p->nombre, proc);
		p->puerto = port;

		/*lo añadimos a la lista*/
		addProceso(p);
		if (!strcmp(proc, argv[1]))
		{ /* Este proceso soy yo */
		}
	}

	a = getProceso("A");
	printf("%s\n", a.nombre);
	a = getProceso("B");
	printf("%s\n", a.nombre);


	/* Inicializar Reloj */

	/* Procesar Acciones */

	return 0;
}
