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
struct sockaddr_in addr, origen;
char *nombreProceso;
struct lista lista;

struct mensaje
{
	char emisor[80];
	char region[80];
	int lc[80];
	int tipo; /*MSG = 0, LOCK = 1, OK = 2*/
};

/*para guardar información de una seccion*/
struct seccion
{
	char nombre[80];
	int nOK;
	int quiereEntrar;
	int dentro;
	struct lista *pendientes;
};

/*lista de secciones*/
struct seccion secciones[80];
int nSecciones;

/*función auxiliar para mandar mensajes udp a un proceso*/
int enviar(char *destName, struct mensaje *m, int t)
{
	struct proceso p;
	struct sockaddr_in addr_env;
	char tipo[5];
	addr_env.sin_family = AF_INET;
	addr_env.sin_addr.s_addr = INADDR_ANY;

	/*obtenemos el puerto destino*/
	p = getProceso(&lista, destName);
	addr_env.sin_port = p.puerto;

	/*si es de tipo MSG*/
	if (t == 0)
	{
		strcpy(tipo, "MSG");
	}

	/*si es de tipo LOCK*/
	if (t == 1)
	{
		strcpy(tipo, "LOCK");
	}

	/*si es de tipo MSG*/
	if (t == 2)
	{
		strcpy(tipo, "OK");
	}

	/*mandamos el mensaje*/
	if (sendto(socket_udp, m, sizeof(struct mensaje), 0, (struct sockaddr *)&addr_env, sizeof(addr_env)) < 0)
	{
		printf("Error en el sendto\n");
		close(socket_udp);
		return -1;
	}

	printf("%s: SEND(%s,%s)\n", nombreProceso, tipo, destName);
	return 0;
}

/*función auxiliar que comprueba si el proceso quiere entrar a una seccion dada*/
struct seccion * contains(char *nombreSeccion, int *encontrado)
{
	int i, pos = 0;
	*encontrado = 1;
	for (i = 0; i < nSecciones; i++)
	{
		if (strcmp(nombreSeccion, secciones[i].nombre) == 0)
		{
			*encontrado = 0;
			pos = i;
			break;
		}
	}
	return &secciones[pos];
}

int main(int argc, char *argv[])
{
	int port, id, procesoActual, i, encontrado;
	int *logicClock, lcPeticion[80];
	socklen_t len;
	char line[80], proc[80], aux[80], tipo[5], argumento[80], seccion[80];
	struct proceso *p;
	struct nodo *actual;
	struct mensaje msj, respuesta;
	struct seccion *sec;

	nombreProceso = argv[1];
	nSecciones = 0;

	if (argc < 2)
	{
		fprintf(stderr, "Uso: proceso <ID>\n");
		return 1;
	}

	/* Establece el modo buffer de entrada/salida a linea */
	setvbuf(stdout, (char *)malloc(sizeof(char) * 80), _IOLBF, 80);
	setvbuf(stdin, (char *)malloc(sizeof(char) * 80), _IOLBF, 80);

	if ((socket_udp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("error creando socket");
		return 1;
	}

	/*Preparamos la dirección del socket*/
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

	printf("%s: %d\n", nombreProceso, puerto_udp);

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
		addProceso(&lista, p);

		if (!strcmp(proc, nombreProceso))
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
		if (strcmp(line, "FINISH\n") == 0)
		{
			break;
		}

		if (strcmp(line, "EVENT\n") == 0)
		{
			event(logicClock, procesoActual, nombreProceso);
			continue;
		}

		if (strcmp(line, "GETCLOCK\n") == 0)
		{
			printf("%s: ", nombreProceso);
			printLC(logicClock, lista.length);
			continue;
		}

		if (strcmp(line, "RECEIVE\n") == 0)
		{
			len = sizeof(struct sockaddr_in);

			if (recvfrom(socket_udp, &respuesta, sizeof(struct mensaje), 0, (struct sockaddr *)&origen, &len) < 0)
			{
				fprintf(stderr, "Error en el read\n");
				close(socket_udp);
				return -1;
			}

			/*si es de tipo MSG*/
			if (respuesta.tipo == 0)
			{
				strcpy(tipo, "MSG");
			}

			/*si es de tipo LOCK*/
			if (respuesta.tipo == 1)
			{
				strcpy(tipo, "LOCK");
			}

			/*si es de tipo OK*/
			if (respuesta.tipo == 2)
			{
				strcpy(tipo, "OK");
			}

			/*se imprime la traza*/
			printf("%s: RECEIVE(%s,%s)\n", nombreProceso, tipo, respuesta.emisor);
			/*la recepción de un mensaje produce un evento*/
			event(logicClock, procesoActual, nombreProceso);
			/*se combina el reloj del proceso con el del mensaje*/
			combineLC(logicClock, respuesta.lc, lista.length);

			/*para mirar cómo se debe reaccionar ante el mensaje*/
			switch (respuesta.tipo)
			{
			/*tipo MSG*/
			case 0:

				break;

			/*tipo LOCK*/
			case 1:

				/*si se quiere entrar en la misma sección crítica*/
				sec = contains(respuesta.region, &encontrado);
				if (encontrado == 0 && sec->quiereEntrar == 0)
				{
					id = getProceso(&lista, respuesta.emisor).id;
					/*se comparan los relojes*/
					if (esAnterior(lcPeticion, respuesta.lc, procesoActual, id, lista.length) == 0)
					{
						/*si el proceso receptor es anterior no se responde*/
						/*se guarda en la lista de pendientes para mandar un mensaje OK después*/
						*p = getProceso(&lista, respuesta.emisor);
						addProceso(sec->pendientes, p);
					}
					else
					{
						/*se envía OK si el del mensaje es anterior*/
						event(logicClock, procesoActual, nombreProceso);
						strcpy(msj.emisor, nombreProceso);
						strcpy(msj.region, respuesta.region);
						msj.tipo = 2;
						copyClock(logicClock, msj.lc, lista.length);
						enviar(respuesta.emisor, &msj, 2);
					}
				}
				/*si ya está dentro de la sección crítica*/
				else if (encontrado == 0 && sec->dentro == 0)
				{
					/*se guarda en la lista de pendientes para mandar un OK después*/
					*p = getProceso(&lista, respuesta.emisor);
					addProceso(sec->pendientes, p);
				}
				else
				{
					/*si no quiere entrar en esa sección se envía OK*/
					event(logicClock, procesoActual, nombreProceso);
					strcpy(msj.emisor, nombreProceso);
					strcpy(msj.region, respuesta.region);
					msj.tipo = 2;
					copyClock(logicClock, msj.lc, lista.length);
					enviar(respuesta.emisor, &msj, 2);
				}

				break;

			/*tipo OK*/
			case 2:

				sec = contains(respuesta.region, &encontrado);
				sec->nOK = sec->nOK + 1;
				if (sec->nOK == (lista.length - 1))
				{
					/*entra dentro de la sección*/
					sec->dentro = 0;
					sec->quiereEntrar = 1;
					printf("%s: MUTEX(%s)\n", nombreProceso, sec->nombre);
				}

				break;
			}

			continue;
		}

		sscanf(line, "%s %s", aux, argumento);

		if (strcmp(aux, "MESSAGETO") == 0)
		{
			/*se genera un evento*/
			event(logicClock, procesoActual, nombreProceso);
			/*guardamos el reloj*/
			copyClock(logicClock, msj.lc, lista.length);
			/*mandamos el mensaje de tipo MSG*/
			strcpy(msj.emisor, nombreProceso);
			msj.tipo = 0;
			enviar(argumento, &msj, 0);
			continue;
		}

		if (strcmp(aux, "LOCK") == 0)
		{
			/*el argumento pasado por consola será la seccion a la que se quiere acceder*/
			strcpy(seccion, argumento);
			/*se genera un evento*/
			event(logicClock, procesoActual, nombreProceso);
			/*guardamos el reloj*/
			copyClock(logicClock, msj.lc, lista.length);
			/*mandamos el mensaje de tipo LOCK*/
			strcpy(msj.emisor, nombreProceso);
			msj.tipo = 1;
			strcpy(msj.region, argumento);

			/*mandamos un mensaje a todos los de la lista*/
			actual = lista.inicio;
			for (i = 1; i <= lista.length; i++)
			{
				/*hay que saltarse el proceso que envía el mensaje*/
				if (i != procesoActual)
				{
					enviar(actual->proc.nombre, &msj, 1);
				}
				actual = actual->next;
			}

			sec = contains(seccion, &encontrado);
			if (encontrado == 1)
			{
				strcpy(secciones[nSecciones].nombre, seccion);
				secciones[nSecciones].quiereEntrar = 0;
				secciones[nSecciones].dentro = 1;
				secciones[nSecciones].nOK = 0;
				secciones[nSecciones].pendientes = malloc(sizeof(struct lista));
				nSecciones = nSecciones + 1;
			}
			else
			{
				sec->quiereEntrar = 0;
				sec->dentro = 1;
				sec->nOK = 0;
				secciones[nSecciones].pendientes = malloc(sizeof(struct lista));
			}

			/*Se guarda el reloj en el que se hizo la petición*/
			copyClock(logicClock, lcPeticion, lista.length);
			continue;
		}

		sec = contains(argumento, &encontrado);
		if (strcmp(aux, "UNLOCK") == 0 && encontrado == 0)
		{
			sec->dentro = 1;
			if (sec->pendientes->length > 0)
			{
				/*mandamos el mensaje de tipo OK a los procesos pendientes*/
				strcpy(msj.emisor, nombreProceso);
				strcpy(msj.region, sec->nombre);
				msj.tipo = 2;
				actual = sec->pendientes->inicio;
				for (i = 1; i <= sec->pendientes->length; i++)
				{
					event(logicClock, procesoActual, nombreProceso);
					copyClock(logicClock, msj.lc, lista.length);
					enviar(actual->proc.nombre, &msj, 2);
					actual = actual->next;
				}
				/*liberamos la lista de pendientes*/
				freeLista(sec->pendientes);
			}
			free(sec->pendientes);
		}
	}

	free(p);
	freeLista(&lista);
	close(socket_udp);

	return 0;
}
