
#define POP_DIM 100
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "colors.h"

struct population_s{
    char ****vettore_popolazioni;
    int *fitness;
};




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

char ***build_solution(int **colors, int row,int col){
    char ***solution;
    int i,j;
    solution=(char ***)malloc(sizeof(char **)*row);
    for(i=0;i<row;i++){
        solution[i]=(char **)malloc(sizeof(char *)*col);
        for(j=0;j<col;j++)
            solution[i][j]=(char *)malloc(sizeof(char)*2);
    }
    return solution;
}
void random_solution_generation(char ***solution,int **colors,int pieces, int row, int col){
    char *taken,n_pieces_taken,random_number,random_rotation;
    int i,j;
    taken=(char *)malloc(sizeof(char)*pieces);
    for(i=0;i<pieces;i++){
        taken[i]=0;
    }
    n_pieces_taken=0;
    srand(time(NULL));
    /*Ciclo sulla matrice della soluzione e cerco un pezzo non preso casuale, usando una rotazion
     casuale. Se è preso, vado a quello dopo. Se arrivo al fondo ricomincio dall'inizio. Ci sono
     metodi molto migliori ma poi lo raffiniamo*/
    for(i=0;i<row;i++)
       for(j=0;j<col;j++){
        random_number = rand() % pieces;
        if (!taken[random_number]){
            ++taken[random_number];
            solution[i][j][0]=(char)random_number;
            ++n_pieces_taken;
            random_rotation= rand() % 4;
            solution[i][j][1]=random_rotation;
        }
        else {
            while (taken[random_number]){
                random_number++;
            if (random_number>=pieces)
                random_number=0;
            }
            solution[i][j][0]=(char)random_number;
            random_rotation= rand() % 4;
            solution[i][j][1]=random_rotation;
            ++taken[random_number];
        }
       }
    return;
}

void test_solution(char ***solution,int row,int col){
    int i,j;
    for(i=0;i<row;i++){
        for(j=0;j<col;j++){
                printf("%d %d \t",solution[i][j][0],solution[i][j][1]);
     }
        printf("\n");
    }
}

population_t *build_population(int **colors,int pieces,int row,int col){
    int i;
    population_t *popolazione_start;
    popolazione_start=(population_t *)malloc(sizeof(population_t));
    char ****vettore_popolazione;
    popolazione_start->vettore_popolazioni=(char ****)malloc(sizeof(char ***)*POP_DIM);
    popolazione_start->fitness=(int *)malloc(sizeof(int)*POP_DIM);
    for(i=0;i<POP_DIM;i++){
        popolazione_start->vettore_popolazioni[i]=build_solution(colors,row,col);
        random_solution_generation(popolazione_start->vettore_popolazioni[i],colors,row*col,row,col);
        popolazione_start->fitness[i]=0;
    }
    return popolazione_start;
}

int fitness_solution_evaluation(int **colors,char ***solution,int pieces,int row,int col){
    int i,j,rot_first,rot_sec,profitto=0,bordo_inferiore,bordo_laterale;
    for(i=0;i<row;i++)
        for(j=0;j<col;j++){
            rot_first=(DESTRA-solution[i][j][1] % 4);
            rot_sec=(SINISTRA-solution[i][j][1] % 4); 
            if (i!=(col-1)){
                if (colors[solution[i][j][0]][rot_first]== colors[solution[i][j+1][0]][rot_sec])
                profitto++;
            }
            rot_first=(SOTTO-solution[i][j][1] % 4);
            rot_sec=(SOPRA-solution[i][j][1] % 4);
            if (j!=(row-1)){
                if (colors[solution[i][j][0]][rot_first]== colors[solution[i][j+1][0]][rot_sec])
                        profitto++;
            }
            return profitto;
        }
}

void fitness_popolation_evaluation(popolation_t *pop,char ***solution,int pieces,int row,int col){
    
}
