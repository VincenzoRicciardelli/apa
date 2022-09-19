
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "ts.h"

graph leggiGrafo(char *argv, TS ts);
void leggiProposta(char *argv, graph g, TS ts);
void insiemeIndipendenteMassimale(char *argv, graph g, TS ts);
void combinazioniSempliciR(int pos, int k, int start, graph g, int *sol, int *bestSol, int currLun, int *max);
int check(graph g, int *sol, int currLun);

int main(int argc, char **argv){
    graph g;
    TS ts= TSinit(1);

    if(argc!=4)
        exit(-1);

    printf("----------------1----------------\n");
    g= leggiGrafo(argv[1], ts);
    //printf("%d\n", GRAPHgetNV(g));
    printf("La lettura del grafo e' avvenuta con successo!\n");

    printf("----------------2----------------\n");
    leggiProposta(argv[2], g, ts);

    printf("----------------3----------------\n");
    insiemeIndipendenteMassimale(argv[3], g, ts);
}

graph leggiGrafo(char *argv, TS ts){
    FILE *fp= fopen(argv, "r");
    graph tmp;
    int id1, id2;
    char str1[21], str2[21];
    while(fscanf(fp, "%s %s", str1, str2)==2){
        id1= TScercaOaggiungi(ts, str1);
        id2= TScercaOaggiungi(ts, str2);
    }

    tmp= GRAPHinit(TSnum(ts));
    rewind(fp);

    while(fscanf(fp, "%s %s", str1, str2)==2){
        id1= TScerca(ts, str1);
        id2= TScerca(ts, str2);
        if(id1<0 || id2<0)
            exit(-2);
        GRAPHinsertE(tmp, id1, id2, 1);
    }

    fclose(fp);

    return tmp;
}

void leggiProposta(char *argv, graph g, TS ts){
    FILE *fp= fopen(argv, "r");

    int id, cont=0, proposta[GRAPHgetNV(g)];
    int i, j, adiacenti=0;
    char str[21];

    while(fscanf(fp, "%s", str)==1){
        id= TScerca(ts, str);
        proposta[cont++]=id;
    }

    for(i=0; i<cont-1 && !adiacenti; i++){
        for(j=i+1; j<cont; j++){
            if(GRAPHadiacenti(g, proposta[i], proposta[j]))
                adiacenti=1;
        }
    }

    if(!adiacenti){
        printf("La soluzione proposta nel file e' un insieme stabile!\n");
    } else{
        printf("La soluzione proposta nel file non e' un insieme stabile!\n");
    }

    fclose(fp);
}

void insiemeIndipendenteMassimale(char *argv, graph g, TS ts){
    FILE *fp= fopen(argv, "w");
    int i, *sol= calloc(GRAPHgetNV(g), sizeof(int)), *bestSol= calloc(GRAPHgetNV(g), sizeof(int));
    int max=0;

    for(i=0; i< GRAPHgetNV(g); i++){
        sol[i]= -1;
        bestSol[i]=-1;
    }

    for(i=1; i<= GRAPHgetNV(g); i++){
        combinazioniSempliciR(0, i, 0, g, sol, bestSol, 0, &max);
    }

    if(max!=0){
        printf("Soluzione trovata e ha cardinalita': %d\n", max);
        for(i=0; i<max; i++){
            fprintf(fp,"%s\n", TSleggi(ts, bestSol[i]));
        }
    }
    else
        printf("Soluzione non trovata!\n");

    free(sol);
    free(bestSol);

    fclose(fp);
}

void combinazioniSempliciR(int pos, int k, int start, graph g, int *sol, int *bestSol, int currLun, int *max){
    int i;
    if(pos>=k){
        if(check(g, sol, currLun)){
            if(currLun>*max){
                *max= currLun;
                memcpy(bestSol, sol, k* sizeof(int));
            }
        }
        return;
    }

    for(i=start; i< GRAPHgetNV(g); i++){
        sol[pos]=i;
        combinazioniSempliciR(pos+1, k, i+1, g, sol, bestSol, currLun+1, max);
    }
}

int check(graph g, int *sol, int currLun){
    int i, j, adiacenti=0;

    for(i=0; i<currLun-1 && !adiacenti; i++){
        for(j=i+1; j<currLun; j++){
            if(GRAPHadiacenti(g, sol[i], sol[j])){
                adiacenti=1;
            }
        }
    }

    if(adiacenti)
        return 0;
    return 1;
}
