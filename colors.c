
#define POP_DIM 100
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "colors.h"
#include "sort.h"

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
    printf("-----------------------------------\n");
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
    popolazione_start->vettore_popolazioni=(char ****)malloc(sizeof(char ***)*POP_DIM);
    popolazione_start->fitness=(int *)malloc(sizeof(int)*POP_DIM);
    for(i=0;i<POP_DIM;i++){
        popolazione_start->vettore_popolazioni[i]=build_solution(colors,row,col);
        random_solution_generation(popolazione_start->vettore_popolazioni[i],colors,row*col,row,col);
        //test_solution(popolazione_start->vettore_popolazioni[i],row,col);
        popolazione_start->fitness[i]=fitness_solution_evaluation(colors,popolazione_start->vettore_popolazioni[i],pieces,row,col);
    }
    return popolazione_start;
}

int fitness_solution_evaluation(int **colors,char ***solution,int pieces,int row,int col){
    int i,j,rot_first,rot_sec,profit=0,bordo_inferiore,bordo_laterale;
    for(i=0;i<row;i++)
        for(j=0;j<col;j++){
            if (j!=(col-1)){
                rot_first=abs(DESTRA-solution[i][j][1] % 4);
                rot_sec=abs(SINISTRA-solution[i][j+1][1] % 4); 
                if (colors[solution[i][j][0]][rot_first]== colors[solution[i][j+1][0]][rot_sec])
                profit++;
            }
            if (i!=(row-1)){
                rot_first=abs(SOTTO-solution[i][j][1] % 4);
                rot_sec=abs(SOPRA-solution[i+1][j][1] % 4);
                if (colors[solution[i][j][0]][rot_first]== colors[solution[i+1][j][0]][rot_sec])
                        profit++;
            }
            
        }
    return profit;
}

/*void fitness_popolation_evaluation(popolation_t *pop,char ***solution,int pieces,int row,int col){
    
}
*/

void test_fitness(population_t *pop){
    int i,max=0,idmax=0;
    printf("Chi è la miglior soluzione?\n");
    for(i=0;i<POP_DIM;i++){
        if ((pop->fitness[i])>max){
            max=pop->fitness[i];
            idmax=i;
        }
        printf("Soluzione %d:\t %d \n",i,pop->fitness[i]);
    }
    printf("Miglior Soluzione n° %d , punti: %d",idmax,pop->fitness[idmax]);
}

int cmp_fitness(int col1,int col2){
    if (col1>col2)
        return 1;
    if (col1<col2)
        return -1;
    return 0;
}

void sort_popolation(population_t *pop,int **colors){

}