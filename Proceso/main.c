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
#include "lista.h"

int puerto_udp;

int main(int argc, char *argv[])
{
	int port, sck;
	int * logicClock;
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

		/*guardamos memoria para el proceso*/
		p = malloc(sizeof(struct proceso));
		strcpy(p->nombre, proc);
		p->puerto = port;

		/*se guarda en la lista*/
		addProceso(p);

		if (!strcmp(proc, argv[1]))
		{ /* Este proceso soy yo */
		}
	}

	/* Inicializar Reloj, un elemento por cada proceso de la lista */
	logicClock = malloc(sizeof(int) * lista.length);

	/* Procesar Acciones */
	while (fgets(line, 80, stdin))
	{
		
	}
	

	return 0;
}
