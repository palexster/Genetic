#define COLN 4  //numero di colori per pezzo
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pieces.h"
#include "popolation.h"

/*funzione per l'allocazione del vettore e caricamento dei dati
 Riceve nome del file in ingresso
 Ritorna il vettore dei pezzi(vettore di vettori di 4 interi che rappresentano i colori)*/
int **build_pieces(char* filename,int* np, int* r, int* c){
    int **mat;//matrice di interi per allocare la struttura dati dei pezzi
    FILE *fp;//puntatore al file che contiene i pezzi
    int row,col,//numero righe e colonne matrice dei pezzi
        npieces,//numero pezzi
        line[4],//vettore temporaneo per caricare i colori
        i,j;//indici della matrice
    /*Lettura dimensioni della matrice e allocazione del vettore dei pezzi*/
    if((fp=fopen(filename,"r"))==NULL){
        fprintf(stderr,"impossibile aprire il file.");
        exit(2);
    }
    fscanf(fp,"%d%d",&row,&col);
    npieces=row*col;
    
    mat=(int **)malloc(sizeof(int*)*npieces); // Allocazione numero di colori
    for(i=0;i<npieces;i++){
        mat[i]=(int *)malloc(sizeof(int)*COLN); // 4 colori per pezzo
        }
  
    /*Riempimento matrice dei pezzi con i colori dei singoli pezzi*/
    for(i=0;i<100;i++){
        fscanf(fp,"%d%d%d%d",&line[0],&line[1],&line[2],&line[3]);
        for(j=0;j<COLN;j++){
                mat[i][j]=line[j];
        }
    }
     *np=npieces;
     *r=row;
     *c=col;
     return mat;
    }
    
/*verifica caricamento colori*/
void test_pieces(int **pieces,int npieces){
    int i,j;
    for(i=0;i<npieces;i++){
        for(j=0;j<COLN;j++){
                printf("%d \t",pieces[i][j]);
     }
        printf("\n");
    }
}

/*Ogni soluzione è composta da una matrice di stringhe da due elementi in cui il
 *primo char è l'indice del pezzo nel vettore dei pezzi e il secondo indica la 
 *rotazione del pezzo.
 Funzione che alloca la soluzione*/
solution_t build_solution(int **pieces, int row,int col){
    solution_t *solution; // Puntatore alla soluzione che deve essere allocata
    int i,j;
    solution=(solution_t *)malloc(sizeof(solution_t));
    solution->matrice_pezzi=(char ***)malloc(sizeof(char**)*row);
    for(i=0;i<row;i++){
        solution->matrice_pezzi[i]=(char **)malloc(sizeof(char *)*col);
        for(j=0;j<col;j++)
            solution->matrice_pezzi[i][j]=(char *)malloc(sizeof(char)*2);
    }
    return *solution;
}
void random_solution_generation(solution_t *solution,int **pieces,int npieces, int row, int col){
    char *taken //vettore dei pezzi già inseriti nella soluzione
    ,n_pieces_taken, // numero di pezzi già inseriti nella soluzione
     random_number, // numero pseudocasuale generato per decidere quale pezzo
            random_rotation;
    int i,j;
    taken=(char *)malloc(sizeof(char)*npieces);
    for(i=0;i<npieces;i++){
        taken[i]=0;
    }
    n_pieces_taken=0;
    /*Ciclo sulla matrice della soluzione e cerco un pezzo non preso casuale, usando una rotazion
     casuale. Se è preso, vado a quello dopo. Se arrivo al fondo ricomincio dall'inizio. Ci sono
     metodi molto migliori ma poi lo raffiniamo*/
    for(i=0;i<row;i++)
       for(j=0;j<col;j++){
        random_number = rand() % npieces;
        if (!taken[random_number]){
            ++taken[random_number];
            solution->matrice_pezzi[i][j][0]=(char)random_number;
            ++n_pieces_taken;
            random_rotation= rand() % COLN;
            solution->matrice_pezzi[i][j][1]=random_rotation;
        }
        else {
            while (taken[random_number]){
                random_number++;
            if (random_number>=npieces)
                random_number=0;
            }
            solution->matrice_pezzi[i][j][0]=(char)random_number;
            random_rotation= rand() % COLN;
            solution->matrice_pezzi[i][j][1]=random_rotation;
            ++taken[random_number];
        }
       }
    return;
}

void test_solution(solution_t *solution,int row,int col){
    int i,j;
    printf("-----------------------------------\n");
    for(i=0;i<row;i++){
        for(j=0;j<col;j++){
                printf("%d %d \t",solution->matrice_pezzi[i][j][0],solution->matrice_pezzi[i][j][1]);
     }
        printf("\n");
    }
}

int fitness_solution_evaluation(int **pieces,solution_t *solution,int npieces,int row,int col){
    int a,b,i,j,rot_first,rot_sec,profit=0;//,bordo_inferiore,bordo_laterale;
    for(i=0;i<row;i++)
        for(j=0;j<col;j++){
            if (j!=(col-1)){
                rot_first=abs(DESTRA-solution->matrice_pezzi[i][j][1] % COLN);
                rot_sec=abs(SINISTRA-solution->matrice_pezzi[i][j+1][1] % COLN);
                a = pieces[solution->matrice_pezzi[i][j][0]][rot_first];
                b = pieces[solution->matrice_pezzi[i][j+1][0]][rot_sec];
                if (a == b)
                        profit++;
            }
            if (i!=(row-1)){
                rot_first=abs(SOTTO-solution->matrice_pezzi[i][j][1] % COLN);
                rot_sec=abs(SOPRA-solution->matrice_pezzi[i+1][j][1] % COLN);
                if (pieces[solution->matrice_pezzi[i][j][0]][rot_first]== pieces[solution->matrice_pezzi[i+1][j][0]][rot_sec])
                        profit++;
            }
            
        }
    return profit;
}

void dealloc_soluzioni(solution_t *sol,int row){
    int count;
    for (count = 0; count < row; count++)
                free(sol->matrice_pezzi[count]);
    free(sol->matrice_pezzi);
    return;
} 