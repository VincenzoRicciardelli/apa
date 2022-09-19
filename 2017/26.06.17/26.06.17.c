
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pq.h"

typedef struct prenotazione_s{
    char *idcliente;
    int arrivo;
    int pernottamenti;
}prenotazione_t;

typedef struct camera_s{
    int occupata;
    int giorni;
    int arrivo;
    char *idcliente;
    PQ *pq;
}camera_t;

typedef struct albergo_s{
    camera_t *camere;
    int nC;
}albergo_t;

prenotazione_t *leggiFile(char *argv);
void checkProposta(char *argv, prenotazione_t *prenotazione, albergo_t *alberghi);

int main(int argc, char **argv){
    albergo_t *alberghi;
    prenotazione_t *prenotazione;
    int i, N, X, a, b;

    if(argc!=3)
        exit(-1);

    printf("Inserire N: ");
    scanf("%d", &N);
    printf("\nInserire X: ");
    scanf("%d", &X);
    printf("\nInserire a: ");
    scanf("%d", &a);
    printf("\nInserire b: ");
    scanf("%d", &b);
    //printf("%d", N);
    alberghi= malloc(N* sizeof(albergo_t));
    printf("\nInserire M: ");
    for(i=0; i<N; i++){
        scanf("%d", &alberghi[i].nC);
        alberghi[i].camere= malloc(alberghi[i].nC* sizeof(int));
        for (int j=0; j<alberghi[i].nC; j++) {
            alberghi[i].camere[j].occupata=0;
        }
        if(alberghi[i].camere==NULL)
            exit(-2);
    }

    prenotazione = leggiFile(argv[1]);
    if(prenotazione==NULL)
        exit(-4);

    printf("--------1--------");
    checkProposta(argv[2], prenotazione, alberghi);

}

prenotazione_t *leggiFile(char *argv){
    int i, cont=0, x, y;
    char str[11];
    prenotazione_t *prenotazione;
    FILE *fp= fopen(argv, "r");

    while(fscanf(fp, "%s %d %d", str, &x, &y)==3){
        cont++;
    }

    //printf("%d\n", cont);
    prenotazione= malloc(cont* sizeof(prenotazione_t));

    rewind(fp);

    while(fscanf(fp,"%s %d %d", str, &prenotazione[i].arrivo, &prenotazione[i].pernottamenti)==3){
        prenotazione[i].idcliente= strdup(str);
    }

    return prenotazione;
}

void checkProposta(char *argv, prenotazione_t *prenotazione, albergo_t *alberghi){
    FILE *fp= fopen(argv, "r");
    char str[11];
    int i, alb, valido=1;

    while(fscanf(fp, "%s %d", str, &alb)==2 && valido){
        for(i=0; i<alberghi[alb].nC; i++){
            if(!alberghi[alb].camere[i].occupata){
                alberghi[alb].camere[i].occupata=1;
                alberghi[alb].camere[i].idcliente= strdup(str);
                break;
            }
            else{
                if(i+1==alberghi[alb].nC){
                    if(checkPeriodo(alberghi, prenotazione)){
                        //coda
                    }
                }
            }
        }
    }
}
