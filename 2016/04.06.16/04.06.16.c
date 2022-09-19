
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts.h"

typedef struct prodotto_s{
    char *id;
    char *nome;
    float prezzo;
    int disp;
    char *cat;
}prodotto_t;

typedef struct categoria_s{
    char *nome;
    prodotto_t *prodotti;
    int nP;
}categoria_t;

typedef struct catalogo_s{
    prodotto_t *prodotti;
    categoria_t *categorie;
    int nP;
    int nC;
}catalogo_t;

catalogo_t *leggiCatalogo(char *argv, TS ts);
void ordinaCatalogoPerCodice(catalogo_t *catalogo);
void ordinaCatalogoPerNome(catalogo_t *catalogo);
void inserimentoProdotto(catalogo_t *catalogo, TS ts);
void ricercaProdottoPerCodice(catalogo_t *catalogo);
void ricercaDicotomica(catalogo_t *catalogo, int p, int r, char *str);
void stampaProdottiCategoria(categoria_t categoria);
void ordinaCategoriaPerCodice(categoria_t categoria);
void ordinaCategoriaPerNome(categoria_t categoria);
void ricercaProdottoPerCodiceCAT(categoria_t catagoria);
void ricercaDicotomicaCAT(categoria_t categoria, int p, int r, char *str);
void cercaSottosequenza(catalogo_t *catalogo);
void faiSpesa(char *argv, catalogo_t *catalogo);

int main(int argc, char **argv){
    catalogo_t *catalogo;
    TS ts= TSinit(1);

    if(argc!=3)
        exit(-1);

    printf("------------------0------------------\n");
    catalogo= leggiCatalogo(argv[1], ts);
    printf("Catalogo letto con successo!\n");

    /*printf("------------------1------------------\n");
    inserimentoProdotto(catalogo, ts);
    printf("Inserimento riuscito!\n");*/
    /*for(int i=0; i<catalogo->nP; i++){
        if(strcmp(catalogo->prodotti[i].id, "6NUT4")==0)
            printf("%d\n", catalogo->prodotti[i].disp);
    }*/
    /*printf("------------------2------------------\n");
    ordinaCatalogoPerCodice(catalogo);
    ricercaProdottoPerCodice(catalogo);

    printf("------------------3------------------\n");
    char str[11];
    printf("Inserire categoria: ");
    scanf("%s", str);
    printf("\n");
    int id= TScerca(ts, str);
    ordinaCategoriaPerCodice(catalogo->categorie[id]);
    ricercaProdottoPerCodiceCAT(catalogo->categorie[id]);

    printf("------------------4------------------\n");
    char str2[11];
    printf("Inserire categoria: ");
    scanf("%s", str2);
    printf("\n");
    int id2= TScerca(ts, str);
    ordinaCategoriaPerNome(catalogo->categorie[id2]);
    stampaProdottiCategoria(catalogo->categorie[id2]);*/

    //printf("------------------5------------------\n");
    //cercaSottosequenza(catalogo);

    printf("------------------6------------------\n");
    faiSpesa(argv[2], catalogo);
}

catalogo_t *leggiCatalogo(char *argv, TS ts){
    FILE *fp= fopen(argv, "r");
    char str[11], str2[256], str3[11];
    int i, id, nP=0, nC=0;
    catalogo_t *tmp= malloc(1* sizeof(catalogo_t));


    while(fscanf(fp, "%*s %*s %*f %*d %s", str3)==1){
        if(TScerca(ts, str3)==-1){
            id= TSaggiungi(ts, str3);
            nC++;
        }
        nP++;
    }

    printf("%d %d\n", nP, nC);

    tmp->nP=nP;
    tmp->nC=nC;
    tmp->prodotti= malloc(nP * sizeof(prodotto_t));
    tmp->categorie= malloc(nC * sizeof(categoria_t));
    for(i=0; i<nC; i++){
        tmp->categorie[i].prodotti= malloc(nP* sizeof(prodotto_t));
        tmp->categorie[i].nP=0;
    }

    int nPperCat[nC];
    for(i=0; i<nC; i++){
        nPperCat[i]=0;
    }

    rewind(fp);

    for(i=0; i<nP; i++){
        fscanf(fp, "%s %s %f %d %s", str, str2, &tmp->prodotti[i].prezzo, &tmp->prodotti[i].disp, str3);
        tmp->prodotti[i].id= strdup(str);
        tmp->prodotti[i].nome= strdup(str2);
        tmp->prodotti[i].cat= strdup(str3);
        id= TScerca(ts, str3);
        tmp->categorie[id].nome= strdup(str3);
        tmp->categorie[id].prodotti[nPperCat[id]++] = tmp->prodotti[i];
        tmp->categorie[id].nP= nPperCat[id];
    }

    fclose(fp);

    return tmp;
}

void ordinaCatalogoPerCodice(catalogo_t *catalogo){
    int i, j, idtmp;
    char minstr[11], tmp[11];

    for(i=0; i<catalogo->nP-1; i++){
        strcpy(minstr,"zzzzzzzzzz");
        for(j=0; j<catalogo->nP; j++){
            if(strcmp(catalogo->prodotti[i].id, catalogo->prodotti[j].id)>0 && strcmp(catalogo->prodotti[j].id, minstr)<0){
                strcpy(minstr, catalogo->prodotti[j].id);
                idtmp= j;
            }
        }
        strcpy(tmp, catalogo->prodotti[i].id);
        strcpy(catalogo->prodotti[i].id, minstr);
        strcpy(catalogo->prodotti[idtmp].id, tmp);
    }
}

void ordinaCatalogoPerNome(catalogo_t *catalogo){
    int i, j, idtmp;
    char minstr[11], tmp[11];

    for(i=0; i<catalogo->nP-1; i++){
        strcpy(minstr,"zzzzzzzzzz");
        for(j=0; j<catalogo->nP; j++){
            if(strcmp(catalogo->prodotti[i].nome, catalogo->prodotti[j].nome)>0 && strcmp(catalogo->prodotti[j].nome, minstr)<0){
                strcpy(minstr, catalogo->prodotti[j].nome);
                idtmp= j;
            }
        }
        strcpy(tmp, catalogo->prodotti[i].nome);
        strcpy(catalogo->prodotti[i].nome, minstr);
        strcpy(catalogo->prodotti[idtmp].nome, tmp);
    }
}

void inserimentoProdotto(catalogo_t *catalogo, TS ts){
    int i, trovato=0;
    char str[11], str2[256], str3[11];
    int id;

    printf("Inserisci il prodotto con il formato <id> <nome> <prezzo> <disponibilita> <categoria>\n");
    printf("Id: ");
    scanf("%s", str);
    printf("\n");
    for(i=0; i<catalogo->nP; i++){
        if(strcmp(catalogo->prodotti[i].id, str)==0){
            printf("Prodotto gia' esistente ne aumento le quantita'\n");
            trovato=1;
            catalogo->prodotti[i].disp++;
        }
    }
    if(!trovato){
        int cont= catalogo->nP+1;
        realloc(catalogo->prodotti, cont* sizeof(prodotto_t));
        catalogo->prodotti[catalogo->nP].id= strdup(str);
        catalogo->nP++;
        printf("nome: ");
        scanf("%s", str2);
        catalogo->prodotti[catalogo->nP].nome= strdup(str2);
        printf("\n");
        printf("prezzo: ");
        scanf("%f", &catalogo->prodotti[catalogo->nP].prezzo);
        printf("\n");
        printf("disponibilita': ");
        scanf("%d", &catalogo->prodotti[catalogo->nP].disp);
        printf("\n");
        printf("categoria: ");
        scanf("%s", str3);
        if(TScerca(ts, str3)==-1){
            id= TSaggiungi(ts, str3);
            catalogo->prodotti[catalogo->nP].cat= strdup(str3);
            int cont2= catalogo->nC;
            realloc(catalogo->categorie, (cont2+1) * sizeof(categoria_t));
            catalogo->categorie[id].nome= strdup(str3);
            catalogo->categorie[id].prodotti= malloc(1* sizeof(prodotto_t));
            catalogo->categorie[id].prodotti[0]=catalogo->prodotti[catalogo->nP];
            catalogo->nC++;
        }
        else{
            catalogo->prodotti[catalogo->nP].cat= strdup(str3);
            id= TScerca(ts, str3);
            int cont3= catalogo->categorie[id].nP;
            realloc(catalogo->categorie[id].prodotti, (cont3+1) * sizeof(prodotto_t));
            catalogo->categorie[id].prodotti[catalogo->categorie[id].nP]= catalogo->prodotti[catalogo->nP];
            catalogo->categorie[id].nP++;
        }
        catalogo->nP++;
    }

}

void ricercaProdottoPerCodice(catalogo_t *catalogo){
    char str[11];
    printf("Inserire il codice da ricercare: ");
    scanf("%s", str);
    printf("\n");
    ricercaDicotomica(catalogo, 0, catalogo->nP, str);
}

void ricercaDicotomica(catalogo_t *catalogo, int p, int r, char *str){
    int q;
    while (p<=r){
        q=(p+r)/2;
        if(strcmp(catalogo->prodotti[q].id, str)==0) {
            printf("Trovato: %s", catalogo->prodotti[q].nome);
        }
        if(strcmp(catalogo->prodotti[q].id, str)>0)
            r= q-1;
        else
            p= q+1;
    }
}

void stampaProdottiCategoria(categoria_t categoria){
    int i;

    for(i=0; i<categoria.nP; i++)
        printf("%s ", categoria.prodotti[i].nome);
}

void ordinaCategoriaPerCodice(categoria_t categoria){
    int i, j, idtmp;
    char minstr[11], tmp[11];

    for(i=0; i<categoria.nP-1; i++){
        strcpy(minstr,"zzzzzzzzzz");
        for(j=0; j<categoria.nP; j++){
            if(strcmp(categoria.prodotti[i].id, categoria.prodotti[j].id)>0 && strcmp(categoria.prodotti[j].id, minstr)<0){
                strcpy(minstr, categoria.prodotti[j].id);
                idtmp= j;
            }
        }
        strcpy(tmp, categoria.prodotti[i].id);
        strcpy(categoria.prodotti[i].id, minstr);
        strcpy(categoria.prodotti[idtmp].id, tmp);
    }
}

void ricercaProdottoPerCodiceCAT(categoria_t categoria){
    char str[11];
    printf("Inserire il codice da ricercare: ");
    scanf("%s", str);
    printf("\n");
    ricercaDicotomicaCAT(categoria, 0, categoria.nP, str);
}

void ricercaDicotomicaCAT(categoria_t categoria, int p, int r, char *str){
    int q;
    while (p<=r){
        q=(p+r)/2;
        if(strcmp(categoria.prodotti[q].id, str)==0)
            printf("Trovato: %s", categoria.prodotti[q].nome);
        if(strcmp(categoria.prodotti[q].id, str)>0)
            r= q-1;
        else
            p= q+1;
    }
}

void ordinaCategoriaPerNome(categoria_t categoria){
    int i, j, idtmp;
    char minstr[11], tmp[11];

    for(i=0; i<categoria.nP-1; i++){
        strcpy(minstr,"zzzzzzzzzz");
        for(j=0; j<categoria.nP; j++){
            if(strcmp(categoria.prodotti[i].nome, categoria.prodotti[j].nome)>0 && strcmp(categoria.prodotti[j].nome, minstr)<0){
                strcpy(minstr, categoria.prodotti[j].nome);
                idtmp= j;
            }
        }
        strcpy(tmp, categoria.prodotti[i].nome);
        strcpy(categoria.prodotti[i].nome, minstr);
        strcpy(categoria.prodotti[idtmp].nome, tmp);
    }
}

void cercaSottosequenza(catalogo_t *catalogo) {
    char str[256], Str[256];
    int i;
    printf("Cerca nome: ");
    scanf("%s", str);
    for(i=0; i< strlen(str)-1; i++)
        Str[i]=str[i];
    for (i = 0; i < catalogo->nP; i++)
        if (strstr(catalogo->prodotti[i].nome, Str) != NULL) {
            printf("%s\n", catalogo->prodotti[i].nome);
        }
}

void faiSpesa(char *argv, catalogo_t *catalogo){
    FILE *fp= fopen(argv, "r");
    if(fp==NULL)
        exit(-1);
    int n, i, j, qnt;
    char code[11];
    int trovato=0;
    float spesa=0.0;

    fscanf(fp, "%d", &n);
    for(i=0; i<n; i++){
        fscanf(fp, "%s %d", code, &qnt);
        for(j=0; j<catalogo->nP; j++){
            if(strcmp(catalogo->prodotti[j].id, code)==0) {
                if (catalogo->prodotti[j].disp >= qnt) {
                    trovato++;
                }
            }
        }
    }

    rewind(fp);

    fscanf(fp, "%d", &n);
    if(trovato==n){
        for(i=0; i<n; i++){
            fscanf(fp, "%s %d", code, &qnt);
            for(j=0; j<catalogo->nP; j++){
                if(strcmp(catalogo->prodotti[j].id, code)==0) {
                        catalogo->prodotti[j].disp-=qnt;
                        spesa+=qnt*catalogo->prodotti[j].prezzo;
                }
            }
        }
        printf("Ordine svolto e si e' speso: %f\n", spesa);
    }
    else
        printf("Non e' stato possibile fare l'ordine!\n");


    fclose(fp);
}
