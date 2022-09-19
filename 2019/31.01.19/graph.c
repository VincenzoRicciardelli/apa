//
// Created by vinci on 15/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"


struct G{
    int **madj;
    int nV;
};

int GRAPHgetNV(graph g){
    if(g!=NULL) return g->nV;
    return -1;
}

graph GRAPHinit(int nV){
    int i;
    graph g = malloc(sizeof (*g));
    if(g==NULL) return NULL;
    g->nV = nV;
    g->madj = calloc(nV,sizeof (int*));
    for(i=0; i<nV; i++) g->madj[i] = calloc(nV, sizeof(int));
    return g;
}

void GRAPHinsertE(graph g, int src, int dst, int w){
    if(src < 0 || dst < 0) return;
    if(g->madj == NULL) return;
    g->madj[src][dst] = w;
    return;
}

int GRAPHadiacenti(graph g, int x, int y){
    if(g==NULL || g->madj==NULL) return 0;
    return g->madj[x][y] !=0;
}

int GRAPHgetPeso(graph g, int x, int y){
    if(g==NULL || g->madj==NULL) return 0;
    return g->madj[x][y];
}

void GRAPHfree(graph g){
    int i, nV= g->nV;
    for (int i = 0; i < nV; i++) free(g->madj[i]);
    free(g->madj);
    free(g);
}
