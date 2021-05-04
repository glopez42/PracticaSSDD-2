/*DEFINICION DE controladorLC.c*/

#ifndef _CONTROLADOR_H
#define _CONTROLADOR_H

int event(int *logicClock, int pos, char * name);

int combineLC(int *lc1, int *lc2, int length);

int esAnterior(int *lcProceso1, int *lcProceso2, int id1, int id2, int length);

void printLC(int * logicClock, int length);

void copyClock(int *lc, int * dest, int length);

#endif