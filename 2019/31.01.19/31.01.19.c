
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "ts.h"

graph leggiGrafo(char *argv, TS ts);
int *leggiProposta(char *argv, int *nk, TS ts);
int checkProposta(graph g, int *kp, int nk);
void kernelMinimo(char *argv, graph g, TS ts);
void combinazioniSemplici(graph g, int k, int *sol, int *bestSol, int *min, int pos, int start);
int *leggiKernelSoluzione(char *argv, TS ts, int *n);
void camminoSemplice(graph g, int *kernel, int n);
void camminoSempliceR(graph g, int v, int *kernel, int n, int *visited, int *path, int *bestPath, int *lun, int *nA, int curr_lun);
int checkCammino(graph g, int *kernel, int n, int *path, int curr_lun);

int main(int argc, char **argv){
    int *kp, nk=0, n=0;
    int *kernel;
    graph g;
    TS ts= TSinit(1);

    if(argc!=4){
        printf("Errore nel numero di parametri di argomenti!\n");
        exit(-1);
    }

    g= leggiGrafo(argv[1], ts);

    if(g == NULL)
        exit(-5);

    printf("\n------------1------------\n");

    kp= leggiProposta(argv[2], &nk, ts);

    printf("La proposta%se' valida\n", (checkProposta(g, kp, nk))? " " : " NON ");

    printf("------------2------------\n");

    kernelMinimo(argv[3], g, ts);

    kernel= leggiKernelSoluzione(argv[3], ts, &n);

    printf("\n------------3------------");

    camminoSemplice(g, kernel, n);

    return 0;
}

graph leggiGrafo(char *argv, TS ts){
    graph *tmp;
    char str1[21], str2[21];
    int i, V, id1=-1, id2=-1;
    FILE *fp;
    fp= fopen(argv, "r");
    if(fp==NULL){
        printf("Errore nell'apertura del file!|\n");
        exit(-2);
    }

    while(fscanf(fp, "%s %s", str1, str2)==2){
        id1= TScercaOaggiungi(ts, str1);
        id2= TScercaOaggiungi(ts, str2);
        //printf("%d %d\n", id1, id2);
        if(id1<0 || id2<0)
            exit(-3);
    }

    V= TSnum(ts);
    tmp= GRAPHinit(V);
    rewind(fp);

    while(fscanf(fp, "%s %s", str1, str2)==2){
        id1= TScerca(ts, str1);
        id2= TScerca(ts, str2);
        if(id1<0 || id2<0)
            exit(-4);
        GRAPHinsertE(tmp, id1, id2, 1);
    }

    fclose(fp);

    return tmp;
}

int *leggiProposta(char *argv, int *nk, TS ts){

    char str[21];
    int i, *tmp;
    FILE *fp;
    fp= fopen(argv, "r");
    if(fp==NULL){
        printf("Errore nell'apertura del file!|\n");
    exit(-2);
    }

    while(fscanf(fp, "%s", str)==1){
        *nk+=1;
    }

    //printf("%d\n",TSnum(ts));
    rewind(fp);
    tmp= calloc(*nk, sizeof(int));

    for(i=0; i<*nk; i++){
        fscanf(fp, "%s", str);
        int id= TScerca(ts, str);
        if(id<0)
            exit(-6);
        tmp[i]=id;
    }

    fclose(fp);
    return tmp;
}

int checkProposta(graph g, int *kp, int nk){
    int V= GRAPHgetNV(g);
    int trovato=0;
    int i, j;


    for(i=0; i<nk-1; i++){
        for(j=i+1; j<nk; j++){
            if(GRAPHadiacenti(g, kp[i], kp[j]))
                return 0;
            if(GRAPHadiacenti(g, kp[j], kp[i]))
                return 0;
        }
    }

    for(i=0; i<V; i++){
        for(j=0; j<nk; j++){
            if(GRAPHadiacenti(g, kp[j], i) || i==kp[j]) {
                trovato = 1;
                break;
            }
            else{
                trovato = 0;
            }
        }
        if(!trovato)
            break;
    }

    if(trovato==1)
        return trovato;
    else
        return 0;
}

void kernelMinimo(char *argv, graph g, TS ts){
    int k, min= GRAPHgetNV(g);
    FILE *fp;
    fp = fopen(argv, "w");
    int *sol= calloc(GRAPHgetNV(g), sizeof(int));
    int *bestSol= calloc(GRAPHgetNV(g), sizeof(int));

    for(k=0; k< GRAPHgetNV(g); k++){
        sol[k]=-1;
        bestSol[k]=-1;
    }

    if(sol==NULL)
        exit(-1);
    if(bestSol==NULL)
        exit(-1);

    for(k=0; k< GRAPHgetNV(g); k++){
        combinazioniSemplici(g, k, sol, bestSol, &min, 0, 0);
    }

    printf("Il kernel a cardinalita' minima %d e': ", min);
    for(k=0; k<min; k++) {
        printf("%s ", TSleggi(ts, bestSol[k]));
        fprintf(fp, "%s\n", TSleggi(ts, bestSol[k]));
    }

    free(sol);
    free(bestSol);
    fclose(fp);
}

void combinazioniSemplici(graph g, int k, int *sol, int *bestSol, int *min, int pos, int start){
    int i, V= GRAPHgetNV(g);

    if(pos>=k){
        if(checkProposta(g, sol, k)){
            if(k< *min){
                *min = k;
                memcpy(bestSol, sol, k* sizeof(int));
            }
        }
    }

    for(i=start; i<V; i++){
        sol[pos] = i;
        combinazioniSemplici(g, k, sol, bestSol, min, pos+1, i+1);
    }
}

int *leggiKernelSoluzione(char *argv, TS ts, int *n){
    FILE *fp= fopen(argv, "r");
    char str[21];
    int *tmp;
    while(fscanf(fp, "%s", str)==1){
        *n+=1;
    }

    rewind(fp);
    tmp= calloc(*n, sizeof(int));

    for(int i=0; i<*n; i++){
        fscanf(fp, "%s", str);
        tmp[i]= TScerca(ts, str);
    }

    return tmp;
}

void camminoSemplice(graph g, int *kernel, int n){
    int v, lun=0, nA=0;
    int *visited = calloc(GRAPHgetNV(g), sizeof(int));

    int *path= malloc(GRAPHgetNV(g) * sizeof(int));
    int *bestPath= malloc(GRAPHgetNV(g) * sizeof(int));

    for(v=0; v< GRAPHgetNV(g); v++) {
        camminoSempliceR(g, v, kernel, n, visited, path, bestPath, &lun, &nA, 0);
    }

    printf("\nPercorso a lunghezza %d: ", lun);
    for(v=0; v<lun; v++)
        printf("%d ", bestPath[v]);

    free(visited);
    free(path);
    return;
}

void camminoSempliceR(graph g, int v, int *kernel, int n, int *visited, int *path, int *bestPath, int *lun, int *nA, int curr_lun){
    int i;
    int j;
    visited[v]=1;
    path[curr_lun] = v;

    if((curr_lun+1) > *lun){
        if(checkCammino(g, kernel, n, path, curr_lun+1)>=*nA){
            *nA = checkCammino(g, kernel, n, path, curr_lun+1);
            *lun = curr_lun+1;
            memcpy(bestPath, path, *lun* sizeof(int));
            //return;
        }
    }
    for(i=0; i< GRAPHgetNV(g); i++){
        if(GRAPHadiacenti(g, v, i)){
            if(!visited[i]){
                camminoSempliceR(g, i, kernel, n, visited, path, bestPath, lun, nA,curr_lun+1);
                visited[i]=0;
            }
        }
    }
}

int checkCammino(graph g, int *kernel, int n, int *path, int curr_lun){
    int i, j, cont=0;
    int trovato[n];

    //printf("\n%d", curr_lun);
    for(i=0; i<n; i++)
        trovato[i]=0;

    for(i=0; i<curr_lun; i++){
        for(j=0; j<n; j++){
            if(path[i]==kernel[j])
                trovato[i]=1;
        }
    }

    /*for(i=0; i<n; i++){
        printf("\n%d ", trovato[i]);
    }*/

    for(i=0; i<n; i++){
        if(trovato[i]==1)
            cont++;
    }

    //printf("\n");
    return cont;
}
