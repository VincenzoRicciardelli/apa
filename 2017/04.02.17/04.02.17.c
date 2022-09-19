
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts.h"
#include "list.h"
#include "graph.h"

typedef struct {
    lista_t *clArray;
    int n;
    int maxN;
    int maxCl;
}cliques;

graph leggi_file(FILE *in, TS ts);
int check(graph g, int *sol);
void risolvi(graph g, TS ts);
void generaCliquesR(graph g, int *sol, cliques *clTot, int pos);
void nuovaClique(graph g, int *sol, cliques *clTot);
void stampaCliques(cliques *clTot, TS ts);
void calcolaPermSemplR(graph g, int nN, int *tmpSol, int *val, int *mark, int pos, int *bestSol, int *bestW);
void nuovaPermSempl(graph g, int nN, int *tmpSol, int *bestSol, int *bestW);
void calcolaStampaCammini(graph g, cliques *clTot, TS ts);

int main(int argc, char **argv){
    FILE *in;
    graph g= NULL;
    TS ts = TSinit(1);

    if(ts==NULL)
        exit(-1);

    if(argc!=2){
        printf("\n Errore nel numero di argomenti");
        exit(-1);
    }

    in= fopen(argv[1], "r");
    if(in==NULL){
        printf("\n Errore nell'apertura del file!");
        exit(-1);
    }

    g= leggi_file(in, ts);
    if(g==NULL)
        exit(-1);

    risolvi(g, ts);

    TSlibera(ts);
    GRAPHfree(g);

    return 0;
}

graph leggi_file(FILE *in, TS ts){
    int id1, id2, w;
    char c[MAXL], r[MAXL];
    graph g;

    while (fscanf(in, "%s %s %d", c, r, &w)==3){
        id1 = TScercaOaggiungi(ts, c);
        id2 = TScercaOaggiungi(ts, r);
        if(id1<0 || id2<0)
            printf("\n Errore di lettura");
    }

    rewind(in);

    while(fscanf(in, "%s %s %d", c, r, &w)==3){
        id1= TScerca(ts, c);
        id2= TScerca(ts, r);
        if(id1>=0 && id2>=0)
            GRAPHinsertE(g, id1, id2, w);
    }
    fclose(in);
    return g;
}

int check(graph g, int *sol){
    int i, j;
    int v= GRAPHgetNV(g);
    int c=1;

    //check connettivita'
    for(i=0; i<v; i++){
        if(sol[i]){//ignora nodi fuori dalla clique
            for(j=i+1; j<v; j++){
                if(sol[j] && !GRAPHadiacenti(g, i, j))
                    return 0; //se anche UN solo arco manca, il grafo non e' completo
            }
        }
    }

    //check massimalita'
    for(i=0; i<v; i++){
        if(!sol[i]){//cerco nodi fuori dalla clique in esame
            c=1; //flag di connettivita'
            for(j=0; j<v; j++){
                if(sol[j] && !GRAPHadiacenti(g,i,j)){
                    c=0;//nodo nella clique non connesso al nodo i
                    break;
                }
            }
            if(c==1) return 0; //clique non massimale in quanto non puo' essere estesa
        }
    }

    //superato il test di completezza e massimalita'
    return 1;
}

void risolvi(graph g, TS ts){
    int i;
    int *sol = calloc(GRAPHgetNV(g), sizeof(int));
    cliques *clTot = calloc(1, sizeof(cliques));

    clTot->maxN = GRAPHgetNV(g);
    clTot->clArray = calloc(clTot->maxN, sizeof(lista_t));
    clTot->n = 0;
    clTot->maxCl = -1;

    generaCliquesR(g, sol, clTot, 0);

    stampaCliques(clTot, ts);

    printf("\n Indice della clique a cardinalita' massima -> %d [dimensione= %d]", clTot->maxCl, LISTAnum(clTot->clArray[clTot->maxCl]));

    calcolaStampaCammini(g, clTot, ts);

    for(i=0; i<clTot->n; i++) LISTAfree(clTot->clArray[i]);
    free(clTot->clArray);
    free(clTot);
    free(sol);
}

void generaCliquesR(graph g, int *sol, cliques *clTot, int pos){

    if(pos>= GRAPHgetNV(g)){
        nuovaClique(g, sol, clTot);
        return;
    }

    sol[pos]=0;
    generaCliquesR(g, sol, clTot, pos+1);
    sol[pos]=1;
    generaCliquesR(g, sol, clTot, pos+1);
}

void nuovaClique(graph g, int *sol, cliques *clTot){
    int i, nNodi, v= GRAPHgetNV(g);
    if(check(g, sol)){ //se clique massimale
        //memorizza la nuova clique
        lista_t newClique = LISTAinit();
        for(i=0; i<v; i++){
            if(sol[i]){
                LISTAinsert(newClique, i);
            }
        }
        clTot->clArray[clTot->n] = newClique;
        nNodi = LISTAnum(newClique);

        if(clTot->maxCl<0 || nNodi> LISTAnum(clTot->clArray[clTot->maxCl]))
            clTot->maxCl = clTot->n;
        clTot->n++;
    }
}

void stampaCliques(cliques *clTot, TS ts){
    int i;
    for(i=0; i<clTot->n; i++){
        printf("Clique #%d: \n", i);
        LISTAshowConNomi(clTot->clArray[i], ts);
        printf("\n\n");
    }
}

void calcolaStampaCammini(graph g, cliques *clTot, TS ts){
    int i, bestW;

    for(i=0; i<clTot->n; i++){
        int nN = LISTAnum(clTot->clArray[i]);
        if(nN>2){//salta le coppie di nodi collegate da un arco
            int *tmpSol = calloc(nN, sizeof(int)); //permutazione temporanea in fase di costruzione
            int *bestSol = calloc(nN, sizeof(int)); //permutazione migliore
            int *mark = calloc(nN, sizeof(int));
            int *val = calloc(nN, sizeof(int));
            LISTA2vett(clTot->clArray[i], val);
            bestW = 0;
            calcolaPermSemplR(g, nN, tmpSol, val, mark, 0, bestSol, &bestW);
            if(bestW>0){
                int j;
                printf("Ciclo di valore %d -> per clique %d\n", bestW, i);
                for (j= 0; j<nN; j++) printf("%s ", TSleggi(ts, bestSol[j]));
                printf("\n");
            }
            //libera tutto prima del prossimo
            free(tmpSol);
            free(bestSol);
            free(mark);
            free(val);
        }
    }
}

void calcolaPermSemplR(graph g, int nN, int *tmpSol, int *val, int *mark, int pos, int *bestSol, int *bestW){
    int i;
    if(pos>=nN){
        nuovaPermSempl(g, nN, tmpSol, bestSol, bestW);
        return;
    }

    for(i=0; i<nN; i++){
        if(mark==0){
            tmpSol[pos]=val[i];
            mark[i]=1;
            calcolaPermSemplR(g, nN, tmpSol, val, mark, pos+1, bestSol, bestW);
            mark[i]=0;
        }
    }
}

void nuovaPermSempl(graph g, int nN, int *tmpSol, int *bestSol, int *bestW){
    int i, w=0;
    for(i=0; i<nN; i++) {
        w += GRAPHgetPeso(g, tmpSol[i],
                          tmpSol[(i + 1) % nN]); //somma dei pesi con modulo per gestire l'arco che ritorna all'inizio
    }
    if(w>*bestW){
        *bestSol = w;
        for (i= 0;  i<nN ; i++) {
            bestSol[i]= tmpSol[i];
        }
    }
}
