
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts.h"
#include "graph.h"

typedef struct amico_s{
    char nome[16];
    int tipo;
    int qnt;
}amico_t;

graph leggiGrafo(char *argv, TS ts);
int leggiProposta(char *argv, TS ts, graph g, amico_t *amici);
void partizioneMinima(graph g, TS ts, amico_t *amici);
void partizionamentoER(graph g, TS ts, int m, int pos, int *sol, int *val, amico_t *amici, int *occ);
int check(int m, graph g, int *sol);

int main(int argc, char **argv){
    TS ts= TSinit(1);
    amico_t *amici;
    int i;
    graph g;
    if(argc!=3)
        exit(-1);

    printf("--------------------------1--------------------------\n");
    g= leggiGrafo(argv[1], ts);
    //printf("%d\n", GRAPHgetNV(g));

    printf("Gli amici sono: \n");
    amici= malloc(GRAPHgetNV(g)*sizeof(amico_t));
    for(i=0; i< GRAPHgetNV(g); i++){
        strcpy(amici[i].nome, TSleggi(ts, i));
        printf("%s\n", amici[i].nome);
        amici[i].tipo=0;
        amici[i].qnt=0;
    }

    printf("--------------------------2--------------------------\n");
    if(!leggiProposta(argv[2], ts, g, amici))
        return 0;

    printf("--------------------------3--------------------------\n");
    partizioneMinima(g, ts, amici);
}

graph leggiGrafo(char *argv, TS ts){
    graph tmp;
    FILE *fp= fopen(argv, "r");
    char str1[16], str2[16];
    int id1, id2;

    while(fscanf(fp, "%s %s", str1, str2)==2){
        id1= TScercaOaggiungi(ts, str1);
        id2= TScercaOaggiungi(ts, str2);
    }

    tmp= GRAPHinit(TSnum(ts));
    rewind(fp);

    while(fscanf(fp, "%s %s", str1, str2)==2){
        id1= TScerca(ts, str1);
        id2= TScerca(ts, str2);

        GRAPHinsertE(tmp, id1, id2, 1);
    }

    fclose(fp);
    return tmp;
}

int leggiProposta(char *argv, TS ts, graph g, amico_t *amici){
    FILE *fp= fopen(argv, "r");
    int i, j, n, p, id, tipo, qnt;
    char str[16];

    tipo=0;
    fscanf(fp, "%d", &n);
    for(i=0; i<n; i++){
        fscanf(fp, "%d", &p);
        for(j=0; j<p; j++){
            fscanf(fp, "%s", str);
            id= TScerca(ts, str);
            amici[id].tipo = tipo;
            amici[id].qnt++;
        }
        tipo++;
    }

    fclose(fp);

    for(i=0; i<GRAPHgetNV(g); i++){
        if(amici[i].qnt>1){
            printf("Proposta errata!\n");
            return 0;
        }
    }

    for(i=0; i< GRAPHgetNV(g)-1; i++){
        for(j=0; j< GRAPHgetNV(g); j++){
            if(GRAPHadiacenti(g, i, j)){
                if(amici[i].tipo==amici[j].tipo){
                    printf("Proposta errata!\n");
                    return 0;
                }
            }
        }
    }

    printf("Proposta corretta!\n");
    return 1;
}

void partizioneMinima(graph g, TS ts, amico_t *amici){
    int occ=0;
    int *sol= calloc(GRAPHgetNV(g), sizeof(int));
    int *val= calloc(GRAPHgetNV(g), sizeof(int));

    partizionamentoER(g, ts, 0, 0, sol, val, amici, &occ);

    free(sol);
}

void partizionamentoER(graph g, TS ts, int m, int pos, int *sol, int *val, amico_t *amici, int *occ){
    int i, j, k;
    if(pos>= GRAPHgetNV(g)){
        if(check(m, g, sol) && !*occ) {
            *occ=1;
            printf("Partizione in %d blocchi: ", m);
            for (i = 0; i < m; i++) {
                printf("{");
                k=0;
                for (j = 0; j < GRAPHgetNV(g); j++) {
                    if (sol[j] == i) printf("%s ", amici[j].nome);
                }
                printf("}");
            }
            printf("\n");
        }
        return;
    }
    for(i=0; i<m; i++){
        sol[pos]=i;
        partizionamentoER(g, ts, m, pos+1, sol, val, amici, occ);
    }
    sol[pos]=m;
    partizionamentoER(g, ts, m+1, pos+1, sol, val, amici, occ);
}

int check(int m, graph g, int *sol){
    int i, j;

    for(i=0; i< GRAPHgetNV(g)-1; i++){
        for(j=i+1; j< GRAPHgetNV(g); j++){
            if(sol[i]==sol[j]){
                if(GRAPHadiacenti(g, i, j)){
                    return 0;
                }
            }
        }
    }

    return 1;
}
