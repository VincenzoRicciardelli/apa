//
// Created by vinci on 31/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ts.h"

typedef struct tsItem_{
    char *id;
}tsItem;

struct ts_t{
    tsItem *items;
    int N, M;
};

TS TSinit(int maxN){
    TS ts;
    ts = calloc(maxN, sizeof(*ts));
    ts->N = 0;
    ts->M = maxN;
    ts->items = calloc(ts->M, sizeof(tsItem));
    return ts;
}

int TSnum(TS ts){
    return ts->N;
}

int TSaggiungi(TS ts, char *str){
    if(ts->N == ts->M){
        ts->M *= 2;
        ts->items = realloc(ts->items, ts->M* sizeof(tsItem));
    }
    ts->items[ts->N++].id = strdup(str);
    return ts->N-1;
}

int TScerca(TS ts, char *str){
    int i;
    for(i=0; i<ts->N; i++)
        if(!strcmp(ts->items[i].id, str)) return i;
    return -1;
}

int TScercaOaggiungi(TS ts, char *str){
    int id = TScerca(ts, str);
    if(id==-1){
        return TSaggiungi(ts, str);
    }
    return id;
}

void TSlibera(TS ts){
    int i;
    if(ts == NULL) return;
    if(ts->items != NULL) {
        for(i=0; i<ts->N; i++){
            if(ts->items[i].id != NULL) free(ts->items[i].id);
        }
        free(ts->items);
    }
    free(ts);
}

char *TSleggi(TS ts, int index){
    if(index < 0 || index > ts->N) return NULL;
    return ts->items[index].id;
}