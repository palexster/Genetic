
#define POP_DIM 100
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "colors.h"
#include "popolation.h"
#include "sort.h"


int **build_colors(int pieces){
    int **mat,i;
    mat=(int **)malloc(sizeof(int*)*pieces); // Allocazione numero di colori
    for(i=0;i<pieces;i++){
        mat[i]=(int *)malloc(sizeof(int)*4); // 4 colori per pezzo
        }
    return mat;
}

void test_colors(int **colors,int pieces){
    int i,j;
    for(i=0;i<pieces;i++){
        for(j=0;j<4;j++){
                printf("%d \t",colors[i][j]);
     }
        printf("\n");
    }
}

/*Ogni soluzione è composta da una matrice di stringhe da due in cui il primo char
 punta al pezzo nel vettore dei colori e il secondo indica la rotazione del pezzo*/

solution_t build_solution(int **colors, int row,int col){
    solution_t *solution;
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
void random_solution_generation(solution_t *solution,int **colors,int pieces, int row, int col){
    char *taken,n_pieces_taken,random_number,random_rotation;
    int i,j;
    taken=(char *)malloc(sizeof(char)*pieces);
    for(i=0;i<pieces;i++){
        taken[i]=0;
    }
    n_pieces_taken=0;
    /*Ciclo sulla matrice della soluzione e cerco un pezzo non preso casuale, usando una rotazion
     casuale. Se è preso, vado a quello dopo. Se arrivo al fondo ricomincio dall'inizio. Ci sono
     metodi molto migliori ma poi lo raffiniamo*/
    for(i=0;i<row;i++)
       for(j=0;j<col;j++){
        random_number = rand() % pieces;
        if (!taken[random_number]){
            ++taken[random_number];
            solution->matrice_pezzi[i][j][0]=(char)random_number;
            ++n_pieces_taken;
            random_rotation= rand() % 4;
            solution->matrice_pezzi[i][j][1]=random_rotation;
        }
        else {
            while (taken[random_number]){
                random_number++;
            if (random_number>=pieces)
                random_number=0;
            }
            solution->matrice_pezzi[i][j][0]=(char)random_number;
            random_rotation= rand() % 4;
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

int fitness_solution_evaluation(int **colors,solution_t *solution,int pieces,int row,int col){
    int i,j,rot_first,rot_sec,profit=0,bordo_inferiore,bordo_laterale;
    for(i=0;i<row;i++)
        for(j=0;j<col;j++){
            if (j!=(col-1)){
                rot_first=abs(DESTRA-solution->matrice_pezzi[i][j][1] % 4);
                rot_sec=abs(SINISTRA-solution->matrice_pezzi[i][j+1][1] % 4); 
                if (colors[solution->matrice_pezzi[i][j][0]][rot_first]== colors[solution->matrice_pezzi[i][j+1][0]][rot_sec])
                profit++;
            }
            if (i!=(row-1)){
                rot_first=abs(SOTTO-solution->matrice_pezzi[i][j][1] % 4);
                rot_sec=abs(SOPRA-solution->matrice_pezzi[i+1][j][1] % 4);
                if (colors[solution->matrice_pezzi[i][j][0]][rot_first]== colors[solution->matrice_pezzi[i+1][j][0]][rot_sec])
                        profit++;
            }
            
        }
    return profit;
}