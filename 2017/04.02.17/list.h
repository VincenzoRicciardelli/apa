//
// Created by vinci on 15/08/2022.
//

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>

#include "ts.h"

typedef struct lista_s *lista_t;

lista_t LISTAinit();
void LISTAinsert(lista_t, int);
void LISTAshow(lista_t);
void LISTAshowConNomi(lista_t, TS);
void LISTA2vett(lista_t, int *v);
int LISTAnum(lista_t l);
void LISTAfree(lista_t l);

#endif //LIST_H
