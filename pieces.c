#define COLN 4  //numero di colori per pezzo
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "pieces.h"
#include "popolation.h"
#define GRAY 0

    
/*funzione per l'allocazione del vettore e caricamento dei dati
 Riceve nome del file in ingresso
 Ritorna il vettore dei pezzi(vettore di vettori di 4 interi che rappresentano i colori)*/
int **build_pieces(char* filename, int *border,int* np, int* r, int* c){
    int **mat,perimetro,nbordo=0;//matrice di interi per allocare la struttura dati dei pezzi
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
    if ((fscanf(fp,"%d%d",&row,&col))==EOF)
        return 1;
    npieces=row*col;
    border=(int *)malloc(sizeof(int)*npieces);
    for(i=0;i<npieces;i++){
        border[i]=0; // 4 colori per pezzo
        }
    mat=(int **)malloc(sizeof(int*)*npieces); // Allocazione numero di colori
    for(i=0;i<npieces;i++){
        mat[i]=(int *)malloc(sizeof(int)*COLN); // 4 colori per pezzo
        }
    
    /*Riempimento matrice dei pezzi con i colori dei singoli pezzi*/
    for(i=0;i<npieces;i++){
        if ((fscanf(fp,"%d%d%d%d",&line[0],&line[1],&line[2],&line[3]))==EOF)
            return 1;
        nbordo=0;
        for(j=0;j<COLN;j++){
            if (line[j]==0){
                nbordo++;
            }
                mat[i][j]=line[j];     
        }
        if (nbordo>0)
                border[i]=nbordo;
    }
     *np=npieces;
     *r=row;
     *c=col;
     return mat;
    }
    
/*verifica caricamento corretto del vettore dei colori*/
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
    solution->feasible=FEASIBLE;
    return *solution;
}
/*
 * Funziona che riempie una soluzione iniziale con i pezzi del vettore pieces in
 * maniera casuale
 */
void random_solution_generation(solution_t *solution,int *border,int **pieces,int npieces, int row, int col){
    char *taken //vettore dei pezzi già inseriti nella soluzione
    ,n_pieces_taken, // numero di pezzi già inseriti nella soluzione
     random_number,perimetro, // numero pseudocasuale generato per decidere quale pezzo
            random_rotation;
    int i,j;
    taken=(char *)malloc(sizeof(char)*npieces);
    for(i=0;i<npieces;i++){
        taken[i]=0;
    }
    n_pieces_taken=0;
    perimetro=(row+col)*2;
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
    free(taken);
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

    // a e b sono utilizzate per memorizzare i colori da confrontare dei due pezzi
    // rot_first e rot_sec per memorizzare la rotazione dei due pezzi considerando 
    // la rotazione del pezzo nella soluzione
    int a,b,i,j,rot_first,rot_sec,profit=0,test;

    for(i=0;i<row;i++)
        for(j=0;j<col;j++){
            // se è l'ultima colonna non controlla il profit laterale
            if (j<(col-1)){
                // l'ultimo indice 1 indica la rotazione, 0 il numero del pezzo
                test=DESTRA;
                rot_first=DESTRA-solution->matrice_pezzi[i][j][1];
                if (rot_first<0)
                    rot_first=4+rot_first;
                // Controllo bounding del pezzo, utile solo in fase di backup
                //if (rot_first<0 || rot_first>4)
                //    printf("Out of bounds reading primo pezzo orizzontale");
                rot_sec=SINISTRA-solution->matrice_pezzi[i][j+1][1];
                if (rot_sec<0)
                    rot_sec=4+rot_sec;
                // Controllo bounding del pezzo, utile solo in fase di backup
                //if (rot_sec<0 || rot_sec>4)
                //    printf("Out of bounds reading secondo pezzo orizzontale");
                a = pieces[solution->matrice_pezzi[i][j][0]][rot_first];
                b = pieces[solution->matrice_pezzi[i][j+1][0]][rot_sec];
                if (a == b && a != GRAY)
                        profit++;
                if (a == GRAY || b == GRAY)
                    solution->feasible=UNFEASIBLE;
            }
            // se è l'ultima riga non controlla il profit orizzontale
            if (i<(row-1)){
                rot_first=SOTTO-solution->matrice_pezzi[i][j][1];
                if (rot_first<0)
                    rot_first=4+rot_first;
/*
                if (rot_first<0 || rot_first>4)
                    printf("Out of bounds reading primo pezzo verticale");
*/
                rot_sec=SOPRA-solution->matrice_pezzi[i+1][j][1];
                if (rot_sec<0)
                    rot_sec=4+rot_sec;
/*
                if (rot_sec<0 || rot_sec>4)
                    printf("Out of bounds reading secondo pezzo orizzontale");
*/
                a=pieces[solution->matrice_pezzi[i][j][0]][rot_first];
                b=pieces[solution->matrice_pezzi[i+1][j][0]][rot_sec];
                if (a==b &&  a != GRAY )
                        profit++;
                if (a == GRAY || b == GRAY)
                    solution->feasible=UNFEASIBLE;
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

void dealloc_pieces(int **pieces, int npieces){
    int i;
    for(i=0;i<npieces;i++){
        free(pieces[i]);
        }
    free(pieces);
}


