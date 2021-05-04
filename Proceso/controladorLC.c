
/*Clase que ofrece operaciones de control a un reloj lógico*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "controladorLC.h"
#include "lista.h"

/*operación que aumenta el contador de una posición del reloj*/
int event(int *logicClock, int pos, char *name)
{
    fprintf(stdout, "%s: TICK\n", name);
    logicClock[pos - 1] = logicClock[pos - 1] + 1;
    return 0;
}

/*operacion que combina dos relojes lógicos. Esta operación se usará*/
/*si el proceso con el reloj lc1  recibe un mensaje del proceso de lc2.*/
int combineLC(int *lc1, int *lc2, int length)
{
    int i, aux;
    for (i = 0; i < length; i++)
    {
        /*si el componente del reloj del proceso que envía el mensaje*/
        /*es mayor, se sustituye en el proceso receptor*/
        if (lc1[i] < lc2[i])
        {
            aux = lc2[i];
            lc1[i] = aux;
        }
    }

    return 0;
}

/*operación que devuelve 0 si lcProceso1 es anterior a lcProceso2, 1 en caso contrario*/
int esAnterior(int *lcProceso1, int *lcProceso2, int id1, int id2, int length)
{
    int i, totalIguales, anterior;
    for (i = 0; i < length; i++)
    {
        /*si un componente es mayor, el proceso2 será anterior*/
        if (lcProceso1[i] > lcProceso2[i])
        {
            return 1;
        }

        /*si los componentes son iguales se suma el contador*/
        if (lcProceso1[i] == lcProceso2[i])
        {
            totalIguales = totalIguales + 1;
        }
    }

    /*si todos los componentes son iguales, el anterior será el de identificador menor*/
    if (totalIguales == length)
    {
        anterior = (id1 < id2) ? 0 : 1;
        return anterior;
    }

    /*si llega hasta aquí, el proceso1 es anterior*/
    return 0;
}


/*operación que imprime un reloj lógico*/
void printLC(int *logicClock, int length)
{

    fprintf(stdout, "LC[");
    int i;
    for (i = 0; i < length; i++)
    {
        fprintf(stdout, "%d", logicClock[i]);
        if (i < length - 1)
        {
            fprintf(stdout, ",");
        }
        
    }
    
    fprintf(stdout, "]\n");
}

/*operación que copia el reloj lc y lo devuelve en dest*/
void copyClock(int *lc, int * dest, int length)
{
   memcpy(dest, lc, length * sizeof(int));
}
