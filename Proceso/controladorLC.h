/*DEFINICION DE controladorLC.c*/

#ifndef _CONTROLADOR_H
#define _CONTROLADOR_H

int event(int *logicClock, int pos, char * name);

int combineLC(int *lc1, int *lc2, int length, char * name);

int esAnterior(int *lcProceso1, int *lcProceso2, int id1, int id2, int length);

void toString(int *logicClock, int length, char *buff);

void printLC(int * logicClock, int length);


#endif