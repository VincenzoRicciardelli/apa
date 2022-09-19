//
// Created by vinci on 27/08/2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PQ.h"

struct pqueue { char *array; int heapsize; };

static int LEFT(int i);
static int RIGHT(int i);
static int PARENT(int i);
static void Swap(PQ pq, int n1, int n2);

int  KEYcmp(int k1, int k2) {
    if (k1 < k2)
        return -1;
    else if ( k1 == k2)
        return 0;
    return 1;
}

static int LEFT(int i) {
    return i*2+1;
}

static int RIGHT(int i) {
    return i*2+2;
}

static int PARENT(int i) {
    return (i-1)/2;
}

PQ PQinit(int maxN){
    PQ pq;
    pq = malloc(sizeof(*pq));
    pq->array = (char *)malloc(maxN*sizeof(char));
    pq->heapsize = 0;
    return pq;
}

void PQfree(PQ pq){
    free(pq->array);
    free(pq);
}

int PQempty(PQ pq) {
    return pq->heapsize == 0;
}

int PQsize(PQ pq) {
    return pq->heapsize;
}

void PQinsert (PQ pq, char *val) {
    int i;
    i  = pq->heapsize++;
    while( (i>=1) && (KEYcmp((pq->array[PARENT(i)]), (val)) == -1) ) {
        pq->array[i] = pq->array[PARENT(i)];
        i = (i-1)/2;
    }
    pq->array[i] = val;
}

static void Swap(PQ pq, int n1, int n2) {
    char *temp;

    temp  = pq->array[n1];
    pq->array[n1] = pq->array[n2];
    pq->array[n2] = temp;
}


void Heapify(PQ pq, int i) {
    int l, r, largest;
    l = LEFT(i);
    r = RIGHT(i);
    if ( (l < pq->heapsize) && (KEYcmp((pq->array[l]), (pq->array[i])) == 1) )
        largest = l;
    else
        largest = i;
    if ( (r < pq->heapsize) && (KEYcmp((pq->array[r]), (pq->array[largest])) == 1))
        largest = r;
    if (largest != i) {
        Swap(pq, i,largest);
        Heapify(pq, largest);
    }
}

char *PQextractMax(PQ pq) {
    char *val;
    Swap (pq, 0,pq->heapsize-1);
    val = pq->array[pq->heapsize-1];
    pq->heapsize--;
    Heapify(pq, 0);
    return val;
}

char *PQshowMax(PQ pq) {
    return pq->array[0];
}

static void ITEMstore(char *val) {
    printf("name: %s\n", val);
}

void PQdisplay(PQ pq) {
    int i;
    for (i = 0; i < pq->heapsize; i++)
        ITEMstore(pq->array[i]);
}

void PQchange (PQ pq, int pos, char *val) {
    while( (pos>=1) && (KEYcmp((pq->array[PARENT(pos)]), (val)) == -1)) {
        pq->array[pos] = pq->array[PARENT(pos)];
        pos = (pos-1)/2;
    }
    pq->array[pos] = val;
    Heapify(pq, pos);
    return;
}

