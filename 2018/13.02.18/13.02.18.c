
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graph.h"
#include "ts.h"

typedef struct node_s{
    char *id;
    int cod;
    int x;
    int y;
}nodo_t;

nodo_t *leggiVertici(char *argv, TS ts);
graph leggiGrafo(char *argv, TS ts, int *nE, nodo_t *nodo);
int controllaSol(char *argv, graph g, TS ts, nodo_t *nodo);
void clearVisitato(int *visitato, int n);
int camminiMinimi(graph g, TS ts);
void camminiMinimiR(graph g, int v, int w, int *lun, int currLun, int *path, int *visitato);

int main(int argc, char **argv){
    graph g;
    nodo_t *nodo;
    int nE, lun1, lun2;
    TS ts= TSinit(1);

    if(argc!=4)
        exit(-1);

    nodo= leggiVertici(argv[1], ts);
    g= leggiGrafo(argv[1], ts, &nE, nodo);

    if(g==NULL)
        exit(-2);

    printf("------------1------------\n");

    lun1= controllaSol(argv[2], g, ts, nodo);
    //printf("%d\n", lun1);
    lun2= controllaSol(argv[3], g, ts, nodo);
    //printf("%d\n", lun2);

    if(lun1<lun2)
        printf("La soluzione 1 ha diametro inferiore: %d\n", lun1);
    if(lun2<lun1)
        printf("La soluzione 2 ha diametro inferiore: %d\n", lun2);
    if(lun1==lun2)
        printf("La soluzione non puo' essere trovata\n");
    if(lun1<0)
        printf("Grafo non completo!\n");
    if(lun2<0)
        printf("Grafo non completo!\n");

    printf("------------2------------\n");

    risolviDiametro(g, ts);

}

nodo_t *leggiVertici(char *argv, TS ts){

    char str[11];
    nodo_t *nodo;
    int i, n, id1, id2;
    FILE *fp= fopen(argv, "r");

    fscanf(fp, "%d", &n);

    nodo= malloc(n*sizeof(nodo_t));

    for(i=0; i<n; i++){
        fscanf(fp, "%s %d %d", str, &nodo[i].x, &nodo[i].y);
        nodo[i].id= strdup(str);
        nodo[i].cod= TScercaOaggiungi(ts, str);
    }

    return nodo;
}

graph leggiGrafo(char *argv, TS ts, int *nE, nodo_t *nodo){
    FILE *fp= fopen(argv, "r");
    graph tmp;
    int i, n, id1, id2;
    char x[11], y[11];

    fscanf(fp, "%d", &n);

    tmp= GRAPHinit(n);

    while(fscanf(fp, "%s %s", x, y)==2){
        int tmpx=0, tmpy=0;
        int dist=0;
        id1= TScerca(ts, x);
        id2= TScerca(ts, y);
        for(i=0; i<n; i++){
            if(id1==nodo[i].cod) {
                tmpx += nodo[i].x;
                tmpy += nodo[i].y;
            }
            if(id2==nodo[i].cod){
                tmpx -= nodo[i].x;
                tmpy -= nodo[i].y;
            }
            //printf("x:%d ", tmpx);
            //printf("y:%d ", tmpy);
        }
        //printf("x:%d ", tmpx);
        //printf("y:%d ", tmpy);
        dist= (int)sqrt((tmpx*tmpx) + (tmpy*tmpy));
        //printf("dist: %d\n", dist);
        GRAPHinsertE(tmp, id1, id2, dist);
        nE++;
    }
    return tmp;
}

int controllaSol(char *argv, graph g, TS ts, nodo_t *nodo){
    FILE *fp= fopen(argv, "r");
    char str1[11], str2[11];
    int i, id1, id2, lun;
    graph copia;
    copia = g;

    //printf("numero: %d\n", GRAPHgetNV(copia));

    while(fscanf(fp, "%s %s", str1, str2)==2){
        id1= TScercaOaggiungi(ts, str1);
        id2= TScercaOaggiungi(ts, str2);
        if(id1<0 || id2<0)
            exit(-2);
    }

    rewind(fp);

    while(fscanf(fp, "%s %s", str1, str2)==2){
        int tmpx=0, tmpy=0;
        int dist=0;
        id1= TScerca(ts, str1);
        id2= TScerca(ts, str2);
        for(i=0; i< GRAPHgetNV(copia); i++){
            if(nodo[i].cod == id1){
                tmpx+=nodo[i].x;
                tmpy+=nodo[i].y;
            }
            if(nodo[i].cod == id2){
                tmpx-=nodo[i].x;
                tmpy-=nodo[i].y;
            }
        }
        dist= (int) sqrt((tmpx*tmpx)+(tmpy*tmpy));

        GRAPHinsertE(copia, id1, id2, dist);
    }

    lun= camminiMinimi(copia, ts);
}

int camminiMinimi(graph g, TS ts){
    int v, w, lun=0, max=0;
    int *path= calloc(GRAPHgetNV(g), sizeof(int));
    int *visitato= calloc(GRAPHgetNV(g), sizeof(int));

    /*for(v=0; v<GRAPHgetNV(g); v++){
        for(int k=0; k< GRAPHgetNV(g); k++){
            visitato[k]=0;
        }
        camminiMinimiR(g, v, &lun, 0, path, visitato);
    }*/

    for(v=0; v< GRAPHgetNV(g)-1; v++){
        clearVisitato(visitato, GRAPHgetNV(g));
        for(w=v+1; w< GRAPHgetNV(g); w++){
            camminiMinimiR(g, v, w, &lun, 0, path, visitato);
            if(lun==0){
                printf("Grafo non completo!\n");
                return -1;
            }
            if(lun > max)
                max=lun;
        }
    }

    return max;
}

void camminiMinimiR(graph g, int v, int w, int *lun, int currLun, int *path, int *visitato){
    int i;
    path[currLun]= v;
    visitato[v]=1;

    /*printf("currlun:%d\n", currLun);
    for(int k=0; k<currLun+1; k++)
        printf("%d ", path[k]);
    printf("\n");
    for(int k=0; k< GRAPHgetNV(g); k++)
        printf("visitato:%d ", visitato[k]);
    printf("\n");*/
    if(v==w){
        *lun= currLun;
        /*printf("lun:%d\n", *lun);
        for(i=0; i<*lun; i++)
            printf("%d ", path[i]);
        printf("\n");*/
    }

    for(i=0; i<GRAPHgetNV(g); i++){
        if(GRAPHadiacenti(g, v, i)){
            if(!visitato[i]){
                //printf("i:%d ", i);
                camminiMinimiR(g, i, w, lun, currLun+1, path, visitato);
                visitato[i]=0;
            }
        }
    }
}

void clearVisitato(int *visitato, int n){
    for(int i=0; i<n; i++)
        visitato[i]=0;
};

void risolviDiametro(graph g, TS ts){
    int k=2, cont;
    int min= INT_MAX;
    int *sol= calloc(k, sizeof(int));

    combinazioniSemplici(g, ts, k, sol, 0, 0, &min, &cont);
}

void combinazioniSemplici(graph g, TS ts, int k, int *sol, int pos, int start, int *min, int *cont){
    int i;
    if(pos>=k){
        if(!GRAPHadiacenti(g, sol[0], sol[1])){
            graph copia = g;
            GRAPHinsertE()
        }
    }

    for(i=start; i< GRAPHgetNV(g); i++){
        sol[pos]=i;
        combinazioniSemplici(g, ts, k, sol, pos+1, i+1, min, cont);
    }
}
