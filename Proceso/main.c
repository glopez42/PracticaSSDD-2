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
#include "controladorLC.h"

int puerto_udp, socket_udp;

struct mensaje
{
	int tipo; /*MSG = 0, LOCK = 1, OK = 2*/
	char buff[80];
	char emisor[80];
};

/*función auxiliar para mandar mensajes udp a un proceso*/
int enviar(char *destName, struct mensaje *m)
{
	struct proceso p;
	struct sockaddr_in addr_env;
	addr_env.sin_family = AF_INET;
	addr_env.sin_addr.s_addr = INADDR_ANY;

	/*obtenemos el puerto destino*/
	p = getProceso(destName);
	addr_env.sin_port = p.puerto;

	/*mandamos el mensaje*/
	if (sendto(socket_udp, m, sizeof(struct mensaje), 0, (struct sockaddr *)&addr_env, sizeof(addr_env)) < 0)
	{
		printf("Error en el sendto\n");
		close(socket_udp);
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int port, id, procesoActual;
	int *logicClock;
	socklen_t len;
	char line[80], proc[80], aux[80], tipo[5];
	struct sockaddr_in addr;
	struct proceso *p;
	struct mensaje msj, respuesta;

	if (argc < 2)
	{
		fprintf(stderr, "Uso: proceso <ID>\n");
		return 1;
	}

	/* Establece el modo buffer de entrada/salida a linea */
	setvbuf(stdout, (char *)malloc(sizeof(char) * 80), _IOLBF, 80);
	setvbuf(stdin, (char *)malloc(sizeof(char) * 80), _IOLBF, 80);

	/*Preparamos socket*/
	if ((socket_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
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
	if (bind(socket_udp, (const struct sockaddr *)&addr, len) < 0)
	{
		perror("bind fallido");
		return 1;
	}

	/*cogemos el puerto del proceso actual*/
	getsockname(socket_udp, (struct sockaddr *)&addr, &len);
	puerto_udp = addr.sin_port;

	fprintf(stdout, "%s: %d\n", argv[1], puerto_udp);

	id = 1;
	for (; fgets(line, 80, stdin);)
	{
		if (!strcmp(line, "START\n"))
			break;

		sscanf(line, "%[^:]: %d", proc, &port);

		/*guardamos memoria para el proceso*/
		p = malloc(sizeof(struct proceso));
		strcpy(p->nombre, proc);
		p->puerto = port;
		p->id = id;

		/*se guarda en la lista*/
		addProceso(p);

		if (!strcmp(proc, argv[1]))
		{ /* Este proceso soy yo */
			procesoActual = id;
		}

		/*se actualiza el nº de id*/
		id = id + 1;
	}

	/* Inicializar Reloj, un elemento por cada proceso de la lista */
	logicClock = malloc(sizeof(int) * lista.length);

	/* Procesar Acciones */
	while (fgets(line, 80, stdin))
	{
		if (strcmp(line, "EVENT\n") == 0)
		{
			event(logicClock, procesoActual, argv[1]);
			continue;
		}

		if (strcmp(line, "GETCLOCK\n") == 0)
		{
			fprintf(stdout, "%s: ", argv[1]);
			printLC(logicClock, lista.length);
			continue;
		}

		if (strcmp(line, "RECEIVE\n") == 0)
		{
			if (read(socket_udp, &respuesta, sizeof(struct mensaje)) < 0)
			{
				fprintf(stderr, "Error en el read\n");
				close(socket_udp);
				return -1;
			}

			/*para mirar de qué tipo es la respuesta*/
			switch (respuesta.tipo)
			{
			/*tipo MSG*/
			case 0:
				strcpy(tipo, "MSG");
				break;

			/*tipo LOCK*/
			case 1:
				strcpy(tipo, "LOCK");
				break;

			/*tipo OK*/
			case 2:
				strcpy(tipo, "OK");
				break;
			}

			fprintf(stdout, "RECEIVE(%s,%s)\n", tipo, respuesta.emisor);
			continue;
		}

		sscanf(line, "%s %s", aux, proc);

		if (strcmp(aux, "MESSAGETO") == 0)
		{
			/*se genera un evento*/
			event(logicClock, procesoActual, argv[1]);
			/*guardamos el reloj en formato string en el buffer del mensaje*/
			toString(logicClock, lista.length, msj.buff);
			/*mandamos el mensaje de tipo MSG*/
			strcpy(msj.emisor, argv[1]);
			msj.tipo = 0;
			enviar(proc, &msj);

			fprintf(stdout, "SEND(MSG,%s)\n", proc);
			continue;
		}

	}

	freeLista();
	close(socket_udp);

	return 0;
}
