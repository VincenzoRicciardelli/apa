
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#include "ts.h"
#include "graph.h"

typedef struct soluzione_s{
    int sol[2];
}soluzione_t;

void leggiVertici(char *argv, TS ts);
void valutaProposta(char *argv, TS ts, int k, int m);
int check(graph g, int k, int m);
void azzeraVisitato(int *visitato, int n);
void camminoSemplice(int v, int w, graph g, int *path, int *visitato, int currLen, int *min);
void costruzioneSoluzione(char *argv, TS ts, int k, int m);
void calcoloCombinazioni(soluzione_t *soluzioni, int n, int k, int pos, int *sol, int *dim, int start);
int combinazioniSemplici(soluzione_t *soluzioni, TS ts, int *bestSol, int k, int n, int pos, int *sol, int *lun, int start, int m, int kappa);
int check2(soluzione_t *soluzioni, TS ts, int *sol, int pos, int m, int k);

int main(int argc, char **argv){
    TS ts= TSinit(1);
    int k, m;
    if(argc!=6)
        exit(-1);

    k= atoi(argv[2]);
    m= atoi(argv[3]);

    printf("-------------------1-------------------\n");
    leggiVertici(argv[1], ts);
    printf("Lettura vertici avvenuta con successo!\n");

    printf("-------------------2-------------------\n");
    valutaProposta(argv[4], ts, k, m);

    printf("-------------------3-------------------\n");
    costruzioneSoluzione(argv[5], ts, k, m);
}

void leggiVertici(char *argv, TS ts){
    FILE *fp= fopen(argv, "r");
    char str[21];
    int i, id;

    while(fscanf(fp, "%s", str)==1){
        id= TScercaOaggiungi(ts, str);
    }

    fclose(fp);
}

void valutaProposta(char *argv, TS ts, int k, int m){
    FILE *fp= fopen(argv, "r");
    char str1[21], str2[21];
    graph g;
    g= GRAPHinit(TSnum(ts));
    int id1, id2;
    while(fscanf(fp, "%s %s", str1, str2)==2){
        id1= TScerca(ts, str1);
        id2= TScerca(ts, str2);
        //printf("%d,%d", id1, id2);
        GRAPHinsertE(g, id1, id2, 1);
    }

    printf("La soluzione proposta %se' valida!\n", check(g, k, m)? "" : "NON ");

    fclose(fp);
}

int check(graph g, int k, int m){
    int i, j;
    int cont, min;
    int *path= calloc(GRAPHgetNV(g), sizeof(int));
    int *visitato = calloc(GRAPHgetNV(g), sizeof(int));

    for(i=0; i< GRAPHgetNV(g)-1; i++){
        for(j=i+1; j< GRAPHgetNV(g); j++){
            min=INT_MAX;
            azzeraVisitato(visitato, GRAPHgetNV(g));
            camminoSemplice(i, j, g, path, visitato, 0, &min);

            if(min==INT_MAX || min>k)
                return 0;
        }
    }

    for(i=0;i< GRAPHgetNV(g);i++){
        cont=0;
        for(j=0; j< GRAPHgetNV(g); j++){
            if(GRAPHadiacenti(g, i, j)){
                cont++;
            }
        }

        if(cont>m)
            return 0;
    }

    return 1;
}

void azzeraVisitato(int *visitato, int n){
    int i;
    for(i=0; i<n; i++)
        visitato[i]=0;
}

void camminoSemplice(int v, int w, graph g, int *path, int *visitato, int currLen, int *min){
    int i;
    visitato[v]=1;
    path[currLen]=v;
    if(v==w){
        if(currLen<*min){
            *min=currLen;
        }
        return;
    }

    for(i=0; i< GRAPHgetNV(g); i++){
        if(GRAPHadiacenti(g, v, i)){
            if(!visitato[i]){
                camminoSemplice(i, w, g, path, visitato, currLen+1, min);
                visitato[i]=0;
            }
        }
    }

}

void costruzioneSoluzione(char *argv, TS ts, int k, int m){
    FILE *fp= fopen(argv, "w");
    soluzione_t *soluzioni= malloc(TSnum(ts) * sizeof(soluzione_t));
    int dim=0, lun=0, i, j;
    int *sol= malloc(2* sizeof(int));

    calcoloCombinazioni(soluzioni, TSnum(ts), 2, 0, sol, &dim, 0);

    /*for(i=0; i<dim; i++){
        for(j=0; j<2; j++){
            printf("%d ", soluzioni[i].sol[j]);
        }
        printf("\n");
    }*/

    int *bestSol= malloc(dim* sizeof(soluzione_t));

    for(i=1; i<=dim; i++){
        if(combinazioniSemplici(soluzioni, ts, bestSol, i, dim, 0, sol, &lun, 0, m, k)){
            printf("Soluzione trovata!\n");
            break;
        }
    }

    for(i=0; i<lun; i++){
        for(j=0; j<2; j++){
            fprintf(fp, "%s ", TSleggi(ts, soluzioni[bestSol[i]].sol[j]));
        }
        fprintf(fp, "\n");
    }
}

void calcoloCombinazioni(soluzione_t *soluzioni, int n, int k, int pos, int *sol, int *dim, int start){
    int i, j;

    if(pos>=k){
        j=*dim;
        for(i=0; i<k; i++){
            soluzioni[j].sol[i]= sol[i];
        }
        *dim+=1;
        return;
    }
    for(i=start; i<n; i++){
        sol[pos]=i;
        calcoloCombinazioni(soluzioni, n, k, pos+1, sol, dim, i+1);
    }

}

int combinazioniSemplici(soluzione_t *soluzioni, TS ts, int *bestSol, int k, int n, int pos, int *sol, int *lun, int start, int m, int kappa){
    int i;

    if(pos>=k){
        if(check2(soluzioni, ts, sol, pos, m, kappa)){
            for(i=0; i<k; i++){
                bestSol[i]=sol[i];
            }
            *lun= k;
            return 1;
        }
        return 0;
    }

    for(i=start; i<n; i++){
        sol[pos]= i;
        if(combinazioniSemplici(soluzioni, ts, bestSol, k, n, pos+1, sol, lun, i+1, m, kappa))
            return 1;
    }
    return 0;
}

int check2(soluzione_t *soluzioni, TS ts, int *sol, int pos, int m, int k){
    int i, v, w;
    graph tmp= GRAPHinit(TSnum(ts));
    for(i=0; i<pos; i++){
        v=soluzioni[sol[i]].sol[0];
        w=soluzioni[sol[i]].sol[1];
        GRAPHinsertE(tmp, v, w, 1);
    }

    if(!check(tmp, m, k))
        return 0;

    GRAPHfree(tmp);
    return 1;
}
