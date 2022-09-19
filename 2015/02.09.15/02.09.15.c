
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct righe_s{
    int n;
    int *dim;
}righe_t;

typedef struct colonne_s{
    int n;
    int *dim;
}colonne_t;

typedef struct matrice_s{
    int R;
    int C;
    righe_t *righe;
    colonne_t *colonne;
}matrice_t;

matrice_t leggiMatrice(char *argv);
void leggiProposta(char *argv, matrice_t mat);
int check(matrice_t mat, int **prop);
void trovaSoluzione(char *argv, matrice_t mat);
int generaSoluzioneR(int pos, matrice_t mat, int **prop, FILE *fp);

int main(int argc, char **argv){
    matrice_t mat;

    if(argc!=4)
        exit(-1);

    printf("----------------1----------------\n");
    mat= leggiMatrice(argv[1]);
    printf("Matrice iniziale letta con successo!\n");

    printf("----------------2----------------\n");
    leggiProposta(argv[2], mat);

    printf("----------------3----------------\n");
    trovaSoluzione(argv[3], mat);
}

matrice_t leggiMatrice(char *argv){
    int i, j;
    matrice_t mat;
    FILE *fp= fopen(argv, "r");
    fscanf(fp, "%d", &mat.R);
    mat.righe= calloc(mat.R, sizeof(int));
    for(i=0; i<mat.R; i++){
        fscanf(fp, "%d", &mat.righe[i].n);
        mat.righe[i].dim= calloc(mat.righe[i].n, sizeof(int));
        for(j=0; j<mat.righe[i].n; j++)
            fscanf(fp, "%d", &mat.righe[i].dim[j]);
    }
    fscanf(fp, "%d", &mat.C);
    mat.colonne= calloc(mat.C, sizeof(int));
    for(i=0; i<mat.C; i++){
        fscanf(fp, "%d", &mat.colonne[i].n);
        mat.colonne[i].dim= calloc(mat.colonne[i].n, sizeof(int));
        for(j=0; j<mat.colonne[i].n; j++){
            fscanf(fp, "%d", &mat.colonne[i].dim[j]);
        }
    }

    fclose(fp);

    return mat;
}

void leggiProposta(char *argv, matrice_t mat){
    FILE *fp= fopen(argv, "r");
    int i, j;

    int **prop;
    prop= calloc(mat.R, sizeof(int*));
    for(i=0; i<mat.R; i++){
        prop[i]= calloc(mat.C, sizeof(int));
        for(j=0; j<mat.C; j++){
            fscanf(fp, "%d", &prop[i][j]);
        }
    }

    fclose(fp);

    printf("La proposta%se' valida!\n", check(mat, prop)? " " : " NON ");

    for(i=0; i<mat.R; i++)
        free(prop[i]);
    free(prop);
}

int check(matrice_t mat, int **prop){
    int i, j;
    int contR, contC;
    int dimR[(mat.R/2)+1], dimC[(mat.C/2)+1];

    for(i=0; i<mat.R; i++){
        contR=0;
        for(j=0; j<mat.C; j++){
            if(prop[i][j]==1){
                if(prop[i][j+1]!=1 && j+1<mat.C){
                    contR++;
                }
                if(j+1>=mat.C){

                    contR++;
                }
            }
        }

        if(contR != mat.righe[i].n){
            //printf("riga:%d, %d!=%d\n", i, contR, mat.righe[i].n);
            //printf("Proposta non valida!\n");
            return 0;
        }

        for(j=0; j<contR; j++){
            dimR[j]=0;
        }

        int k=0;
        for(j=0; j<mat.C; j++){
            if(prop[i][j]==1){ //&& (j=0 || prop[i][j-1]==1)){
                if(prop[i][j+1]==1 && j+1<mat.C){
                    dimR[k]++;
                }
                else{
                    dimR[k]++;
                    if(mat.righe[i].dim[k]!=dimR[k]){
                        //printf("Proposta non valida!\n");
                        return 0;
                    }

                    k++;
                }
            }
        }
    }

    for(j=0; j<mat.C; j++){
        contC=0;
        int prec=0;
        for(i=0; i<mat.R; i++){
            if(prop[i][j]==1){
                prec=1;

                if(i+1>=mat.R)
                    contC++;
            }
            if(prop[i][j]==0 && prec==1){
                contC++;
                prec=0;
            }
        }

        if(contC != mat.colonne[j].n){
            //printf("colonna:%d, %d!=%d\n", j, contC, mat.colonne[j].n);
            //printf("Proposta non valida!\n");
            return 0;
        }

        for(i=0; i<mat.R; i++){
            dimC[i]=0;
        }

        int k=0;
        int precdim=0;
        for(i=0; i<mat.R; i++){
            if(prop[i][j]==1){
                precdim=1;
                dimC[k]++;
            }
            else {
                if (precdim == 1) {
                    k++;
                    precdim = 0;
                }
            }

            //altrimenti sballa tutto
            if(j==0&&k==0&&dimC[k]==2){
                dimC[k]=1;
            }

            if(i+1==mat.R){
                for(int l=0; l<k+1; l++){
                    if(mat.colonne[j].dim[l]!=dimC[l]) {
                        //printf("colonna:%d,%d %d!=%d\n", j, l, dimC[l], mat.colonne[j].dim[l]);
                        //printf("Proposta non valida!\n");
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

void trovaSoluzione(char *argv, matrice_t mat){
    FILE *fp= fopen(argv, "w");
    int i;
    int **prop;
    prop= calloc(mat.R, sizeof(int*));
    for(i=0; i<mat.R; i++){
        prop[i]= calloc(mat.C, sizeof(int));
    }
    if(generaSoluzioneR(0, mat, prop, fp)){
        printf("Soluzione trovata!\n");
    } else{
        printf("Soluzione non trovata!\n");
    }

    for(i=0; i<mat.R; i++)
        free(prop[i]);
    free(prop);
}

int generaSoluzioneR(int pos, matrice_t mat, int **prop, FILE *fp){
    int i,j;
    if(pos == mat.R*mat.C){
        if(check(mat, prop)) {
            for(i=0; i<mat.R; i++){
                for(j=0; j<mat.C; j++){
                    fprintf(fp, "%d ", prop[i][j]);
                }
                fprintf(fp, "\n");
            }
            return 1;
        }
        return 0;
    }
    i = pos / mat.R;
    j = pos%mat.C;
    prop[i][j] = 1;
    if(generaSoluzioneR(pos+1, mat, prop, fp))
        return 1;
    prop[i][j] = 0;
    if(generaSoluzioneR(pos+1, mat, prop, fp))
        return 1;
    return 0;
}
