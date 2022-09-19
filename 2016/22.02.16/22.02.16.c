
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "ts.h"


graph leggiGrafo(char *argv, TS ts);
int *leggiColori(char *argv, TS ts);
int controllaColore(int *colori, int id, char *col);
void camminoSempliceMassimo(graph g, TS ts, int *col);
void camminoSempliceR(graph g, TS ts, int *col, int *path, int *bestPath, int *max, int currW, int currL, int k, int *visitato);
void sottoGrafoMassimo(graph g, TS ts, int *col);
void camminoSempliceCompletoR(graph g, TS ts, int *col, int *path, int *bestPath, int *max, int currW, int currL, int k, int *visitato);
int check(graph g, int v, int w, TS ts, int *col);
int checkValidita(graph g, TS ts, int *path, int currL, int *col);

int main(int argc, char **argv){

    int *colori;
    TS ts= TSinit(1);
    graph g;
    if(argc!=2)
        exit(-1);

    printf("---------------------1---------------------\n");
    g= leggiGrafo(argv[1], ts);
    colori= leggiColori(argv[1], ts);
    printf("Grafo letto e verifica dei colori dei nodi fatta!\n");

    printf("---------------------2---------------------\n");
    camminoSempliceMassimo(g, ts, colori);

    printf("---------------------3---------------------\n");
    sottoGrafoMassimo(g, ts, colori);
}

graph leggiGrafo(char *argv, TS ts){
    FILE *fp= fopen(argv, "r");
    char str[21], str2[21], col[7], col2[7];
    int id1, id2, w, i;
    graph tmp;

    while(fscanf(fp, "%s %*s %*d %s %*s", str, str2)==2){
        id1= TScercaOaggiungi(ts, str);
        id2= TScercaOaggiungi(ts, str2);
    }

    tmp= GRAPHinit(TSnum(ts));
    rewind(fp);

    while(fscanf(fp, "%s %s %d %s %s", str, col, &w, str2,col2)==5){
        id1= TScerca(ts, str);
        id2= TScerca(ts, str2);
        GRAPHinsertE(tmp, id1, id2, w);
    }

    fclose(fp);
    return tmp;

}

int *leggiColori(char *argv, TS ts){
    FILE *fp= fopen(argv, "r");
    char str[21], str2[21], col[7], col2[7];
    int id1, id2;
    int *colori= calloc(TSnum(ts), sizeof(int));

    while(fscanf(fp, "%s %s %*d %s %s", str, col, str2,col2)==4){
        id1= TScerca(ts, str);
        id2= TScerca(ts, str2);
        if(colori[id1]==0){
            if(strcmp(col, "ROSSO")==0){
                colori[id1]=1;
            } else{
                colori[id1]=2;
            }
        }
        if(colori[id2]==0){
            if(strcmp(col2, "ROSSO")==0){
                colori[id2]=1;
            } else{
                colori[id2]=2;
            }
        }
        if(colori[id1]!=0 || colori[id2]!=0){
            if(!controllaColore(colori, id1, col))
                exit(-2);
            if(!controllaColore(colori, id2, col2))
                exit(-3);
        }
    }

    return colori;
}

int controllaColore(int *colori, int id, char *col){

    if(strcmp(col, "ROSSO")==0 && colori[id]!=1){
        printf("Errore nella stesura del grafico\n");
        return 0;
    } else{
        if(strcmp(col, "NERO")==0 && colori[id]!=2){
            printf("Errore nella stesura del grafico\n");
            return 0;
        }
    }
    return 1;
}


void camminoSempliceMassimo(graph g, TS ts, int *col){
    int k, i;
    int max=0;
    int *path= calloc(GRAPHgetNV(g), sizeof(int));
    int *bestPath= calloc(GRAPHgetNV(g), sizeof(int));
    int *visitato= calloc(GRAPHgetNV(g), sizeof(int));

    for(i=0; i< GRAPHgetNV(g); i++){
        path[i]=bestPath[i]=-1;
    }

    for(k=0; k<GRAPHgetNV(g); k++){
        for (i=0; i< GRAPHgetNV(g); i++) {
            visitato[i] = 0;
        }
        camminoSempliceR(g, ts, col, path, bestPath, &max, 0, 0, k, visitato);
    }

    printf("Il cammino semplice a peso massimo %d trovato e': \n", max);
    for(i=0; i<GRAPHgetNV(g) && bestPath[i]!=-1; i++){
        printf("%s ", TSleggi(ts, bestPath[i]));
    }
    printf("\n");

    free(visitato);
    free(bestPath);
    free(path);
}

void camminoSempliceR(graph g, TS ts, int *col, int *path, int *bestPath, int *max, int currW, int currL, int k, int *visitato){
    int i;
    visitato[k]=1;
    path[currL]=k;

    if(currW>*max){
        *max=currW;
        int lun= currL+1;
        memcpy(bestPath, path, lun* sizeof(int));
    }

    for(i=0; i< GRAPHgetNV(g); i++){
        if(GRAPHadiacenti(g, k, i)){
            if(check(g, k, i, ts, col)) {
                if (!visitato[i]) {
                    currW+= GRAPHgetPeso(g, k, i);
                    camminoSempliceR(g, ts, col, path, bestPath, max, currW, currL + 1, i, visitato);
                    visitato[i] = 0;
                    currW-= GRAPHgetPeso(g, k, i);
                }
            }
        }
    }
}

int check(graph g, int v, int w, TS ts, int *col){
    if(col[v]==1 && col[w]==1)
        return 0;
    return 1;
}

void sottoGrafoMassimo(graph g, TS ts, int *col){
    int k, i;
    int max=0;
    int *path= calloc(GRAPHgetNV(g), sizeof(int));
    int *bestPath= calloc(GRAPHgetNV(g), sizeof(int));
    int *visitato= calloc(GRAPHgetNV(g), sizeof(int));

    for(i=0; i< GRAPHgetNV(g); i++){
        path[i]=bestPath[i]=-1;
    }

    for(k=0; k<GRAPHgetNV(g); k++){
        for (i=0; i< GRAPHgetNV(g); i++) {
            visitato[i] = 0;
        }
        camminoSempliceCompletoR(g, ts, col, path, bestPath, &max, 0, 0, k, visitato);
    }

    printf("Il sottografo completo a peso massimo %d trovato e': \n", max);
    for(i=0; i<GRAPHgetNV(g) && bestPath[i]!=-1; i++){
        printf("%s ", TSleggi(ts, bestPath[i]));
    }
    printf("\n");

    free(visitato);
    free(bestPath);
    free(path);
}

void camminoSempliceCompletoR(graph g, TS ts, int *col, int *path, int *bestPath, int *max, int currW, int currL, int k, int *visitato){
    int i;
    visitato[k]=1;
    path[currL]=k;

    if(checkValidita(g, ts, path, currL, col)){
        if(currW>*max){
            *max=currW;
            int lun= currL+1;
            memcpy(bestPath, path, lun* sizeof(int));
        }
    }

    for(i=0; i< GRAPHgetNV(g); i++){
        if(GRAPHadiacenti(g, k, i)){
            if(check(g, k, i, ts, col)) {
                if (!visitato[i]) {
                    currW+= GRAPHgetPeso(g, k, i);
                    camminoSempliceCompletoR(g, ts, col, path, bestPath, max, currW, currL + 1, i, visitato);
                    visitato[i] = 0;
                    currW-= GRAPHgetPeso(g, k, i);
                }
            }
        }
    }
}

int checkValidita(graph g, TS ts, int *path, int currL, int *col){
    int i, j;
    int cN=0, cR=0;

    for(i=0; i<currL; i++){
        for(j=i+1; j<currL+1; j++){
            if(!GRAPHadiacenti(g, path[i], path[j])) {
                return 0;
            }
        }

        if(col[path[i]]==1){
            cR++;
        } else{
            cN++;
        }
    }

    if(abs(cR-cN)>2)
        return 0;

    return 1;
}
