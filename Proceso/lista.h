
/*DEFINICION DE LA ESTRUCTURA DE DATOS LISTA*/

#ifndef _LISTA_H
#define _LISTA_H

/*struct para guardar informacion de un proceso*/
struct proceso
{
	char nombre[80];
	int puerto;
};

/*struct de la estructura de datos*/
struct lista
{
	struct nodo *inicio;
	struct nodo *final;
	int length;
} lista;

/*función que devuelve el proceso de la lista*/
/*que coincida con el nombre dado*/
struct proceso getProceso(char *nombre);

/*función que añade un proceso a la lista*/
int addProceso(struct proceso *p);


#endif