//
// Created by vinci on 31/08/2022.
//

#ifndef GRAPH_H
#define GRAPH_H

typedef struct G *graph;

graph GRAPHinit(int nV);
void GRAPHinsertE(graph g, int src, int dst, int w);
int GRAPHgetNV(graph g);
int GRAPHadiacenti(graph g, int x, int y);
int GRAPHgetPeso(graph g, int x, int y);
void GRAPHfree(graph g);

#endif //GRAPH_H