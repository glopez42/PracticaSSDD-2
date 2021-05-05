
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"


/*función que devuelve en p, el proceso de la lista*/
/*que coincida con el nombre dado*/
struct proceso getProceso(struct lista *lista, char *nombre)
{

	int found = -1;
	struct nodo *nodoActual;
	struct proceso res;
	nodoActual = lista->inicio;

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

/*función que añade un proceso a la lista*/
int addProceso(struct lista *lista, struct proceso *p)
{
	struct nodo *n;
	n = malloc(sizeof(struct nodo));
	n->proc = *p;

	if (lista->length == 0)
	{
		lista->inicio = n;
		lista->end = n;
	}
	else
	{
		lista->end->next = n;
		lista->end = n;
	}
	lista->length = lista->length + 1;
    n->pos = lista->length;
	return 0;
}

/*función que libera la memoria usada por una lista*/
void freeLista(struct lista *lista)
{
	struct nodo *nodoActual;
	struct nodo *nodoSiguiente;
	nodoActual = lista->inicio;

	while (nodoActual != NULL)
	{
		nodoSiguiente = nodoActual->next;
		free(nodoActual);
		nodoActual = nodoSiguiente;
	}

}