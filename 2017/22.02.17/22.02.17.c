
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct prodotto_s{
    char *nome;
    int prezzo;
    int qnt;
}prodotto_t;

typedef struct offerta_s{
    char *id;
    int prezzo;
    int nP;
    prodotto_t *prodotti;
}offerta_t;

typedef struct catalogo_s{
    prodotto_t *prodotti;
    offerta_t *offerte;
    int nP;
    int nO;
}catalogo_t;

//fare una struttura unica e passare quella

catalogo_t *leggiCatalogo(char *argv);
void trovaOfferte(catalogo_t *catalogo, char *nomeP);
void prodottiComuni(catalogo_t *catalogo, char *off1, char *off2);
void spesaOttima(char *argv, catalogo_t *catalogo);
void combinazioniSempliciR(int pos, int start, int k, catalogo_t *catalogo, prodotto_t *spesa, int nS, int currVal, int *min, int *sol, int *bestSol);
int check(int *sol, int k, catalogo_t *catalogo, prodotto_t *spesa, int nS, int *currVal);
prodotto_t trovaProdottoInCatalogo(catalogo_t *catalogo, char *str);

int main(int argc, char **argv){
    catalogo_t *catalogo;
    char nomeP[21], off1[21], off2[21];

    if(argc!=3)
        exit(-1);

    printf("\n------1------\n");
    catalogo= leggiCatalogo(argv[1]);
    printf("Catalogo letto con successo!\n");

    printf("\n------2------\n");
    printf("Inserire il nome del prodotto da cercare: ");
    scanf("%s", nomeP);
    printf("\n");
    trovaOfferte(catalogo, nomeP);

    printf("\n------3------\n");
    printf("Inserire il codice della prima offerta: ");
    scanf("%s", off1);
    printf("\n");
    printf("Inserire il codice della seconda offerta: ");
    scanf("%s", off2);
    printf("\n");
    prodottiComuni(catalogo, off1, off2);

    printf("\n------4------\n");
    spesaOttima(argv[2], catalogo);
}

catalogo_t *leggiCatalogo(char *argv){
    int i, j;
    catalogo_t *tmp;
    char str[21], str2[21];
    FILE *fp= fopen(argv, "r");

    tmp= malloc(sizeof(catalogo_t));

    fscanf(fp, "%d %d", &tmp->nP, &tmp->nO);

    tmp->prodotti= malloc(tmp->nP* sizeof(prodotto_t));
    tmp->offerte= malloc(tmp->nO* sizeof(offerta_t));

    for(i=0; i<tmp->nP; i++){
        fscanf(fp, "%s %d", str, &tmp->prodotti[i].prezzo);
        tmp->prodotti[i].nome= strdup(str);
    }

    for(i=0; i<tmp->nO; i++){
        fscanf(fp, "%s %d %d", str, &tmp->offerte[i].prezzo, &tmp->offerte[i].nP);
        tmp->offerte[i].id= strdup(str);
        tmp->offerte[i].prodotti= malloc(tmp->offerte[i].nP* sizeof(prodotto_t));
        for(j=0; j<tmp->offerte[i].nP; j++){
            fscanf(fp, "%s %d", str2, &tmp->offerte[i].prodotti[j].qnt);
            tmp->offerte[i].prodotti[j].nome= strdup(str2);
        }
    }

    fclose(fp);
    return tmp;
}

void trovaOfferte(catalogo_t *catalogo, char *nomeP){
    int i, j;

    printf("Il prodotto e' stato trovato nelle seguenti offerte: ");
    for(i=0; i<catalogo->nO; i++){
        for ( j=0; j<catalogo->offerte[i].nP ; j++) {
            //printf("%s", catalogo->offerte[i].prodotti[j].nome);
            if(strcmp(catalogo->offerte[i].prodotti[j].nome, nomeP)==0) {
                printf("%s ", catalogo->offerte[i].id);
            }
        }
    }
    printf("\n");

}

void prodottiComuni(catalogo_t *catalogo, char *off1, char *off2){
    int i, j;
    offerta_t *offerta1, *offerta2;

    for(i=0; i<catalogo->nO; i++){
        if(strcmp(catalogo->offerte[i].id, off1)==0)
            offerta1= &catalogo->offerte[i];
        if(strcmp(catalogo->offerte[i].id, off2)==0)
            offerta2= &catalogo->offerte[i];
    }

    printf("I prodotti in comune delle due offerte sono: ");
    for(i=0; i<offerta1->nP; i++){
        for(j=0; j<offerta2->nP; j++){
            if(strcmp(offerta1->prodotti[i].nome, offerta2->prodotti[j].nome)==0){
                printf("%s", offerta1->prodotti[i].nome);
            }
        }
    }
    printf("\n");
}

void spesaOttima(char *argv, catalogo_t *catalogo){
    int nS, i, k, *sol= calloc(catalogo->nO, sizeof(int)), *bestSol= calloc(catalogo->nO, sizeof(int));
    int min=INT_MAX;
    char str[21];
    prodotto_t *spesa;
    FILE *fp= fopen(argv, "r");
    fscanf(fp, "%d", &nS);
    spesa= malloc(nS* sizeof(prodotto_t));

    for(i=0; i<catalogo->nO; i++){
        sol[i]=-1;
        bestSol[i]=-1;
    }

    for(i=0; i<nS; i++){
        fscanf(fp, "%s %d", str, &spesa[i].qnt);
        spesa[i].nome= strdup(str);
    }
    fclose(fp);



    for(k=1; k<=catalogo->nO; k++){
        combinazioniSempliciR(0, 0, k, catalogo, spesa, nS, 0, &min, sol, bestSol);
    }
    printf("Il prezzo minimo ottimale e': %d\n", min);
    printf("I prodotti acquistati sono: ");
    for(k=0; k<nS; k++) {
        for (i = 0; i < catalogo->nO && bestSol[i] != -1; i++) {
            for(int j=0; j<catalogo->offerte[bestSol[i]].nP; j++){
                if(strcmp(spesa[k].nome, catalogo->offerte[bestSol[i]].prodotti[j].nome)==0){
                    spesa[k].qnt-=catalogo->offerte[bestSol[i]].prodotti[j].qnt;
                }
            }
        }
    }

    for (i = 0; i < catalogo->nO && bestSol[i] != -1; i++) {
        printf("%s ", catalogo->offerte[bestSol[i]].id);
    }
    for(i=0; i<nS; i++){
        if(spesa[i].qnt>0){
            printf("%d di %s ", spesa[i].qnt, spesa[i].nome);
        }
    }

    free(bestSol);
    free(sol);
}

void combinazioniSempliciR(int pos, int start, int k, catalogo_t *catalogo, prodotto_t *spesa, int nS, int currVal, int *min, int *sol, int *bestSol){
    int i;

    if(pos>=k){
        if(check(sol, k, catalogo, spesa, nS, &currVal)) {
            if (currVal < *min) {
                *min = currVal;
                memcpy(bestSol, sol, k* sizeof(int));
            }
        }
        return;
    }

    for(i=start; i<catalogo->nO; i++){
        sol[pos]=i;
        combinazioniSempliciR(pos+1, i+1, k, catalogo, spesa, nS, currVal, min, sol, bestSol);
    }

}

int check(int *sol, int k, catalogo_t *catalogo, prodotto_t *spesa, int nS, int *currVal){
    int i, j, l;
    prodotto_t tmp;
    int *mark= calloc(k, sizeof(int));
    int *qnt= calloc(nS, sizeof(int));
    for(i=0; i<nS; i++){
        qnt[i]=spesa[i].qnt;
    }

    for(i=0; i<nS; i++) {
        for (j=0; j<k; j++) {
            for(l=0; l<catalogo->offerte[sol[j]].nP; l++){
                if(strcmp(spesa[i].nome, catalogo->offerte[sol[j]].prodotti[l].nome)==0){
                    qnt[i]-=catalogo->offerte[sol[j]].prodotti[l].qnt;
                    if(mark[j]==0) {
                        *currVal += catalogo->offerte[sol[j]].prezzo;
                        mark[j]=1;
                    }
                }
            }
            tmp= trovaProdottoInCatalogo(catalogo, spesa[i].nome);
        }

        if(qnt[i]>0){
            *currVal+=(qnt[i]*tmp.prezzo);
        }
    }

    free(mark);
    free(qnt);
}

prodotto_t trovaProdottoInCatalogo(catalogo_t *catalogo, char *str){
    int i;
    prodotto_t tmp;
    for(i=0; i<catalogo->nP; i++){
        if(strcmp(catalogo->prodotti[i].nome, str)==0)
            tmp= catalogo->prodotti[i];
    }
    return tmp;
}
