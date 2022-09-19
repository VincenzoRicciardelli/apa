
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts.h"
#include "graph.h"

graph leggiGrafo(char *argv, TS ts);
void calcolaSottocammini(char *argv, graph g, TS ts);
int *cercaIntersezione(int *camm1, int *camm2, int dim1, int dim2, int *dimsol);
void sottocammini(graph g, TS ts, int *intersezione, int dimsol, int *camm, int dim);
void camminoSemplice(int v, int w, int *path, int *camm, int dim, graph g, TS ts, int currL, int *visitato, int start);
void camminiOttimi(graph g, TS ts, int v, int w, int k, int p);
void camminoSempliceR(graph g, int v, int w, int k, int p, int contp, int currw, int *max, int currlen, int *path, int *bestPath, int *visitato);

int main(int argc, char **argv){
    graph g;
    int v, w, k, p;
    char str1[21], str2[21];
    TS ts= TSinit(1);
    if(argc!=3)
        exit(-1);

    printf("--------------------1--------------------\n");
    g= leggiGrafo(argv[1], ts);
    printf("Lettura del grafo effettuata!\n");

    printf("--------------------2--------------------\n");
    calcolaSottocammini(argv[2], g, ts);

    printf("--------------------3--------------------\n");
    printf("Inserisci il vertice di partenza: ");
    scanf("%s", str1);
    v= TScerca(ts, str1);
    printf("Inserisci il vertice di arrivo: ");
    scanf("%s", str2);
    w= TScerca(ts, str2);
    printf("Inserisci k: ");
    scanf("%d", &k);
    printf("Inserisci p: ");
    scanf("%d", &p);
    camminiOttimi(g, ts, v, w, k, p);
}

graph leggiGrafo(char *argv, TS ts){
    FILE *fp= fopen(argv, "r");
    int id1, id2, w;
    char str1[21], str2[21];
    graph tmp;

    while(fscanf(fp, "%s %*d %s", str1, str2)==2){
        id1= TScercaOaggiungi(ts, str1);
        id2= TScercaOaggiungi(ts, str2);
    }

    tmp= GRAPHinit(TSnum(ts));
    rewind(fp);

    while (fscanf(fp, "%s %d %s", str1, &w, str2)==3){
        id1= TScerca(ts, str1);
        id2= TScerca(ts, str2);
        GRAPHinsertE(tmp, id1, id2, w);
    }

    fclose(fp);
    return tmp;
}

void calcolaSottocammini(char *argv, graph g, TS ts){
    FILE *fp= fopen(argv, "r");
    int *camm1, *camm2, *intersezione;
    char str[21];
    int i, dim1, dim2, dimsol;

    fscanf(fp, "%d", &dim1);
    camm1= calloc((dim1+1), sizeof(int));
    for(i=0; i<=dim1; i++){
        fscanf(fp, "%s", str);
        camm1[i]= TScerca(ts, str);
    }
    fscanf(fp, "%d", &dim2);
    camm2= calloc((dim2+1), sizeof(int));
    for(i=0; i<=dim2; i++){
        fscanf(fp, "%s", str);
        camm2[i]= TScerca(ts, str);
    }

    fclose(fp);

    /*for(i=0; i<=dim1; i++)
        printf("%d ", camm1[i]);
    printf("\n");
    for(i=0; i<=dim2; i++)
        printf("%d ", camm2[i]);
    printf("\n");*/

    intersezione= cercaIntersezione(camm1, camm2, dim1, dim2, &dimsol);

    printf("L'intersezione dei due cammini e': ");
    for(i=0; i<dimsol; i++){
        printf("%s\n", TSleggi(ts, intersezione[i]));
    }
    printf("\n");

    printf("Per il cammino uno: \n");
    sottocammini(g, ts, intersezione, dimsol, camm1, dim1);
    printf("\nPer il cammino due: \n");
    sottocammini(g, ts, intersezione, dimsol, camm2, dim2);

}

int *cercaIntersezione(int *camm1, int *camm2, int dim1, int dim2, int *dimsol){
    int *sol;
    int i, j, k;

    if(dim1<=dim2){
        sol= calloc(dim1, sizeof(int));
    } else{
        sol= calloc(dim2, sizeof(int));
    }

    k=0;
    for(i=0; i<dim1; i++){
        for(j=0; j<dim2; j++){
            if(camm1[i]==camm2[j]){
                sol[k++]= camm1[i];
            }
        }
    }

    *dimsol= k;

    return sol;
}

void sottocammini(graph g, TS ts, int *intersezione, int dimsol, int *camm, int dim){
    int v, maxL, start;
    int *path= calloc((dim+1), sizeof(int));
    int *visitato= calloc((dim+1), sizeof(int));

    for(v=0; v<dimsol; v++){
        for(int i=0; i<=dim; i++) {
            if (camm[i] == intersezione[v])
                start = i;
            visitato[camm[i]] = 0;
        }
        printf("\nIl sottocammino a partire da %s: ", TSleggi(ts,intersezione[v]));
        if(v+1==dimsol){
            camminoSemplice(intersezione[v], camm[dim], path, camm, dim, g, ts, 0, visitato, start);
        }
        else
            camminoSemplice(intersezione[v], intersezione[v+1], path, camm, dim, g, ts, 0, visitato, start);
        printf("\n");
    }
}

void camminoSemplice(int v, int w, int *path, int *camm, int dim, graph g, TS ts, int currL, int *visitato, int start){
    int i;
    visitato[v]=1;
    path[currL]=v;
    if(v==w){
        for(i=0; i<currL+1; i++)
            printf("%s ", TSleggi(ts, path[i]));
        return;
    }

    for(i=start; i<=dim; i++){
        if(GRAPHadiacenti(g, v, camm[i])){
            if(!visitato[camm[i]]){
                camminoSemplice(camm[i], w, path, camm, dim, g, ts, currL+1, visitato, start);
                visitato[camm[i]]=0;
                return;
            }
        }
    }
}

void camminiOttimi(graph g, TS ts, int v, int w, int k, int p){
    int i;
    int max=0;
    int *path= calloc(GRAPHgetNV(g), sizeof(int));
    int *bestPath= calloc(GRAPHgetNV(g), sizeof(int));
    int *visitato= calloc(GRAPHgetNV(g), sizeof(int));


    for(i=0; i< GRAPHgetNV(g); i++){
        bestPath[i]=-1;
        visitato[i]=0;
    }
    camminoSempliceR(g, v, w, k, p, 0, 0, &max, 0, path, bestPath, visitato);

    /*i=0;
    printf("Il percorso ottimo trovato ha peso %d ed e': ", max);
    while(bestPath[i]!=-1){
        printf("%s ", TSleggi(ts, bestPath[i]));
        i++;
    }*/


}

void camminoSempliceR(graph g, int v, int w, int k, int p, int contp, int currw, int *max, int currlen, int *path, int *bestPath, int *visitato){
    int i;

    visitato[v]++;
    path[currlen]=v;

    for (i = 0; i < currlen + 1; i++)
        printf("%d ", path[i]);
    printf("\n");
    if(v==w){
        if(currw>*max) {
            *max = currw;
            printf("%d\n", *max);
            memcpy(bestPath, path, (currlen + 1) * sizeof(int));

            for (i = 0; i < currlen + 1; i++)
                printf("%d ", bestPath[i]);
        }
        return;
    }

    for(i=0; i< GRAPHgetNV(g); i++){
        if(GRAPHadiacenti(g, v, i)){
            printf("ciao!!\n");
            if(visitato[i]<k){
                printf("ciao2!!\n");
                if(contp<p){
                    printf("ciao3!!\n");
                    contp++;
                    currw+= GRAPHgetPeso(g, v, i);
                    camminoSempliceR(g, i, w, k, p, contp, currw, max, currlen+1, path, bestPath, visitato);
                    contp--;
                    visitato[i]--;
                    currw-= GRAPHgetPeso(g, v, i);
                }
            }
        }
    }
}
