
/*Clase que ofrece operaciones de control a un reloj lógico*/

#include <stdio.h>
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

/*operacion que combina dos relojes lógicos. Esta operación*/
/*se usará si el proceso con el reloj lc1 (proceso actual) recibe */
/*un mensaje del proceso de lc2.*/
int combineLC(int *lc1, int *lc2, int length, char * name)
{
    struct proceso p;
    int i;
    for (i = 0; i < length; i++)
    {
        /*si el componente del reloj del proceso que envía el mensaje*/
        /*es mayor, se sustituye en el proceso receptor*/
        if (lc1[i] < lc2[i])
        {
            lc1[i] = lc2[i];
        }
    }
    /*obtenemos información del proceso receptor*/
    p = getProceso(name);

    /*esta operación conlleva un evento, por lo que hay que actualizar el reloj del proceso.*/
    /*La posición que se actualizará, es la del proceso (el id).*/
    return event(lc1, p.id, name);
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

/*operación que devuelve en buff el reloj lógico en formato String*/
void toString(int *logicClock, int length, char *buff)
{
    int i, j = 3;
    buff[0] = 'L';
    buff[1] = 'C';
    buff[2] = '[';
    for (i = 0; i < length; i++)
    {
        buff[j] = logicClock[i] + '0';
        j = j + 1;
        if (i < length - 1)
        {
            buff[j] = ',';
            j = j + 1;
        }
    }

    buff[j] = ']';
    j = j + 1;
    buff[j] = '\0';
}

/*operación que imprime un reloj lógico*/
void printLC(int *logicClock, int length)
{
    char buff[80];
    toString(logicClock, length, buff);
    fprintf(stdout, "%s\n", buff);
}
