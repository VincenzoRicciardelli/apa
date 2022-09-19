
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cella_s{
    char car;
    int visitato;
}cella_t;

typedef struct grid_t{
    cella_t *celle;
}GRID;

typedef struct words_t{
    char *word;
    int val;
}WORDS;

typedef struct soluz{
    int r;
    int c;
    char *word;
    char dir[6];
}SOL;

GRID *leggiGriglia(char *filename, int *R, int *C);
WORDS *leggiParole(char *filename, int *nW);
SOL *leggiProposta(char *filename, int *nS);
void pulisciGriglia(GRID *g, int R, int C);
int propostaValida(SOL *sol, int nS, GRID *g, int R, int C, WORDS *w, int nW);
int checkOriz(GRID *g, int R, int C, char *word, int r, int c);
int checkVert(GRID *g, int R, int C, char *word, int r, int c);
int checkDiag(GRID *g, int R, int C, char *word, int r, int c);
void cercaSoluzioneOttima(GRID *g, int R, int C, WORDS *w, int nW);
void combSemplici(GRID *g, int pos, int start, int *sol, SOL *soluzione, SOL *bestSoluzione, int k, int *maxL, int R, int C, WORDS *w, int nW, int currVal, int *maxVal);
int check(int *sol, SOL *soluzione, int lun, GRID *g, int R, int C, WORDS *w);

int main(){
    GRID *g;
    WORDS *w;
    SOL *s;
    int R, C, nW=0, nS=0;

    printf("------------------------1------------------------\n");
    g= leggiGriglia("griglia.txt", &R, &C);
    w= leggiParole("parole.txt", &nW);
    s= leggiProposta("proposta.txt", &nS);
    printf("La lettura dei tre files e' avvenuta con successo!\n");

    printf("------------------------2------------------------\n");
    printf("La soluzione proposta %se' valida!\n", propostaValida(s, nS, g, R, C, w, nW)? "" : "NON ");

    printf("------------------------3------------------------\n");
    cercaSoluzioneOttima(g, R, C, w, nW);
}

GRID *leggiGriglia(char *filename, int *R, int *C){
    int i, j;
    FILE *fp= fopen(filename, "r");
    GRID *g;
    fscanf(fp, "%d %d", R, C);
    g= malloc(*R * sizeof(GRID));
    for(i=0; i<*R; i++){
        g[i].celle= malloc(*C * sizeof(cella_t));
        fscanf(fp, "%*c");
        for(j=0; j<*C; j++){
            fscanf(fp, "%c", &g[i].celle[j].car);
            g[i].celle[j].visitato=0;
        }
    }

    fclose(fp);
    return g;
}

WORDS *leggiParole(char *filename, int *nW){
    FILE *fp= fopen(filename, "r");
    WORDS *p;
    char str[16];
    int i;
    while(fscanf(fp, "%*s %d", &i)==1){
        *nW+=1;
    }

    rewind(fp);

    p= malloc(*nW* sizeof(WORDS));

    for(i=0; i<*nW; i++){
        fscanf(fp, "%s %d", str, &p[i].val);
        p[i].word= strdup(str);
    }

    fclose(fp);
    return p;
}

SOL *leggiProposta(char *filename, int *nS){
    FILE *fp= fopen(filename, "r");
    SOL *sol;
    int i;
    char str[16], str2[6];
    while(fscanf(fp, "%*s %*s %d %*d", &i)==1){
        *nS+=1;
    }

    rewind(fp);
    sol= malloc(*nS* sizeof(SOL));

    for(i=0; i<*nS; i++){
        fscanf(fp, "%s %s %d %d", str, str2, &sol[i].r, &sol[i].c);
        sol[i].word= strdup(str);
        strcpy(sol[i].dir, str2);
    }

    fclose(fp);
    return sol;
}

int propostaValida(SOL *sol, int nS, GRID *g, int R, int C, WORDS *w, int nW){
    int i, j, trovato;

    //controllo sull'esistenza delle parole
    for(i=0; i<nS; i++){
        trovato=0;
        for(j=0; j<nW; j++){
            if(strcmp(sol[i].word, w[j].word)==0)
                trovato=1;
        }
        if(trovato==0)
            return 0;

        //controllo sulle restanti condizioni
        if(strcmp(sol[i].dir,"Oriz.")==0){
            if(!checkOriz(g, R, C, sol[i].word, (sol[i].r)-1, (sol[i].c)-1))
                return 0;
        }

        if(strcmp(sol[i].dir, "Vert.")==0){
            if(!checkVert(g, R, C, sol[i].word, (sol[i].r)-1, (sol[i].c)-1))
                return 0;
        }

        if(strcmp(sol[i].dir, "Diag.")==0){
            if(!checkDiag(g, R, C, sol[i].word, (sol[i].r)-1, (sol[i].c)-1))
                return 0;
        }
    }

    //pulisco la griglia
    pulisciGriglia(g, R, C);

    return 1;
}

int checkOriz(GRID *g, int R, int C, char *word, int r, int c){
    int i, j;
    int in=c;
    if(r<0 || r>=R || c<0 || c+ strlen(word)>=C)
        return 0;
    for(i=0; i< strlen(word); i++){
        if(word[i]==g[r].celle[c].car){
            if(g[r].celle[c].visitato>1)
                return 0;
            g[r].celle[c].visitato++;
            c++;
        }

        else {
            for (j = in; j < c; j++) {
                g[r].celle[j].visitato--;
            }
            return 0;
        }
    }
    return 1;
}

int checkVert(GRID *g, int R, int C, char *word, int r, int c){
    int i, j;
    int in=r;

    if(r<0 || r+ strlen(word)>=R || c<0 || c>=C)
        return 0;
    for(i=0; i< strlen(word); i++){
        if(word[i]==g[r].celle[c].car){
            if(g[r].celle[c].visitato>1)
                return 0;
            g[r].celle[c].visitato++;
            r++;
        } else {
            for (j = in; j < r; j++) {
                g[j].celle[c].visitato--;
            }
            return 0;
        }
    }
    return 1;
}

int checkDiag(GRID *g, int R, int C, char *word, int r, int c){
    int i, j, k, inr=r, inc=c, t;
    int cont=0;
    if(r<0 || r+ strlen(word)>=R || c<0 || c+ strlen(word)>=C)
        return 0;
    for(i=0; i< strlen(word); i++){
        if(word[i]==g[r].celle[c].car){
            if(g[r].celle[c].visitato>1)
                return 0;
            g[r].celle[c].visitato++;
            r++;
            c++;
            cont++;
        }
        else{
            for(j=inr, k=inc, t=0; t<cont; j++, k++, t++){
                g[j].celle[k].visitato--;
            }
            return 0;
        }
    }

    return 1;
}

void cercaSoluzioneOttima(GRID *g, int R, int C, WORDS *w, int nW){
    int i, maxVal=0, maxL=0;
    int *sol= malloc(nW* sizeof(int));
    SOL *soluzione= malloc(nW* sizeof(SOL));
    SOL *bestSoluzione= malloc(nW* sizeof(SOL));

    for(i=1; i<=nW; i++){
        combSemplici(g, 0, 0, sol, soluzione, bestSoluzione, i, &maxL, R, C, w, nW, 0, &maxVal);
    }

    //stampo soluzione
    printf("Soluzione ottima trovata a valore: %d\n", maxVal);
    for(i=0; i<maxL; i++){
        printf("%s %s %d %d\n", bestSoluzione[i].word, bestSoluzione[i].dir, bestSoluzione[i].r, bestSoluzione[i].c);
    }

    free(sol);
    free(soluzione);
    free(bestSoluzione);

}

void combSemplici(GRID *g, int pos, int start, int *sol, SOL *soluzione, SOL *bestSoluzione, int k, int *maxL, int R, int C, WORDS *w, int nW, int currVal, int *maxVal){
    int i;
    if(pos>=k){
        if(check(sol, soluzione, k, g, R, C, w)){
            if(currVal>*maxVal){
                *maxVal=currVal;
                *maxL= k;
                /*for(i=0; i<k; i++){
                    printf("%d ", sol[i]);
                    //printf("%s %s %d %d ", soluzione[i].word, soluzione[i].dir, soluzione[i].r, soluzione[i].c);
                }*/
                //printf("\n");

                memcpy(bestSoluzione, soluzione, k*sizeof(SOL));
            }
        }
        return;
    }
    for(i=start; i<nW; i++){
        sol[pos]=i;
        currVal+=w[i].val;
        combSemplici(g, pos+1, i+1, sol, soluzione, bestSoluzione, k, maxL, R, C, w, nW, currVal, maxVal);
        currVal-=w[i].val;
    }
}

int check(int *sol, SOL *soluzione, int lun, GRID *g, int R, int C, WORDS *w){
    int i, j, k;
    int ok;

    for(i=0; i<lun; i++){
        ok=0;
        for(j=0; j<R && ok==0; j++){
            for(k=0; k<C; k++){
                if(w[sol[i]].word[0]==g[j].celle[k].car){
                    if(checkOriz(g, R, C, w[sol[i]].word, j, k)){
                        soluzione[i].word= strdup(w[sol[i]].word);
                        strcpy(soluzione[i].dir, "Oriz.");
                        soluzione[i].r= j;
                        soluzione[i].c= k;
                        ok=1;
                        break;
                    }
                    if(checkVert(g, R, C, w[sol[i]].word, j, k)){
                        soluzione[i].word= strdup(w[sol[i]].word);
                        strcpy(soluzione[i].dir, "Vert.");
                        soluzione[i].r= j;
                        soluzione[i].c= k;
                        ok=1;
                        break;
                    }
                    if(checkDiag(g, R, C, w[sol[i]].word, j, k)){
                        soluzione[i].word= strdup(w[sol[i]].word);
                        strcpy(soluzione[i].dir, "Diag.");
                        soluzione[i].r= j;
                        soluzione[i].c= k;
                        ok=1;
                        break;
                    }
                }
            }
        }

        if(ok==0){
            pulisciGriglia(g, R, C);
            return 0;
        }
    }

    //pulisco la griglia
    pulisciGriglia(g, R, C);

    return 1;
}

void pulisciGriglia(GRID *g, int R, int C){
    int i, j;
    for(i=0; i<R; i++)
        for(j=0; j<C; j++)
            g[i].celle[j].visitato=0;
}
