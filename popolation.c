#define POP_DIM 100
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pieces.h"
#include "popolation.h"

population_t *build_population(int **pieces,int npieces,int row,int col){
    int i,fit;
    population_t *popolazione_start;
    popolazione_start=(population_t *)malloc(sizeof(population_t));
    popolazione_start->soluzioni=(solution_t *)malloc(sizeof(solution_t)*POP_DIM);
    for(i=0;i<POP_DIM;i++){
        popolazione_start->soluzioni[i]=build_solution(pieces,row,col);
        random_solution_generation(&popolazione_start->soluzioni[i],pieces,row*col,row,col);
        //test_solution(&popolazione_start->soluzioni[i],row,col);
        fit=fitness_solution_evaluation(pieces,&popolazione_start->soluzioni[i],npieces,row,col);
        popolazione_start->soluzioni[i].fitness=fit;
    }
    return popolazione_start;
}

void test_fitness(population_t *pop){
    int i,max=0,idmax=0;
    //printf("Qual è la miglior soluzione?\n");
    for(i=0;i<POP_DIM;i++){
        if ((pop->soluzioni[i].fitness)>max){
            max=pop->soluzioni[i].fitness;
            idmax=i;
        }
        //printf("Soluzione %d:\t %d \n",i,pop->soluzioni[i].fitness);
    }
    printf("Miglior Soluzione n° %d , punti: %d\n",idmax,pop->soluzioni[idmax].fitness);
}

int cmp_fitness(solution_t s1cast,solution_t s2cast){
    if (s1cast.fitness>s2cast.fitness)
        return -1;
    if (s1cast.fitness<s2cast.fitness)
        return 1;
    return 0;
}

void sorted_popolation(population_t *pop,int **pieces){
  quick_sort(pop->soluzioni,0,POP_DIM-1,cmp_fitness);  
}

void quick_sort(solution_t *array, int l, int r, int (*cmp)(solution_t lv, solution_t rv))
{
   int i, j;
   solution_t pivot, tmp;

   if (l >= r) return;

   pivot = array[l];
   i = l - 1; j = r + 1;
   while (i < j) {


      while (cmp(array[++i], pivot) < 0);
      while (cmp(array[--j], pivot) > 0);

      if (i < j) {
         tmp = array[i];
         array[i] = array[j];
         array[j] = tmp;
      }
   }
   quick_sort(array, l, j, cmp);
   quick_sort(array, j+1, r, cmp);
}

void dealloc_population(population_t *pop,int row){
    dealloc_soluzioni(pop->soluzioni,row);
    free(pop->soluzioni);
    //free(pop);
    return;
}
void crossover(int **pieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2, int npieces, int row, int col){
    // generazione tagli, contatori e indice righe/colonne
    int taglio1,taglio2,i,r,c,c1,r1,j,flag;
    // confronto pezzi dentro il kernel
    char pezzoDaControllare,**kernelPieces;
    //allocazione vettori per il confronto ottimizzato del kernel
    kernelPieces=(char **)malloc(sizeof(char*)*npieces);
    for(i=0;i<npieces;i++){
        kernelPieces[i]=(char *)malloc(sizeof(char)*2);
        kernelPieces[i][0]=-1;
        kernelPieces[i][1]=-1;
    }
    taglio1=rand() % npieces/2 +1 ;
    taglio2=rand() % npieces/2 + npieces/2;
    if (taglio2=taglio1)
        taglio2=taglio2 + npieces/10;
    fig1=build_solution(int** pieces, int row, int col);
    fig2=build_solution(int** pieces, int row, int col);
    // Generazione kernel della prole
    for(i=taglio1;i<taglio2;i++){
        r = i/col;
        c = i % col;
        // figlio 1
        fig1->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        fig1->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r][c][1];
        kernelPieces[sol1->matrice_pezzi[r][c][0]][0]=i;
        // figlio 2
        fig2->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        fig2->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r][c][1];
        kernelPieces[sol2->matrice_pezzi[r][c][0]][1]=i;
    }
    // Generazione lato sinistro della prole
    for(i=0;i<taglio1;i++){
        r = i/col;
        c = i % col;
        flag=0;
        j=taglio1;
        // se il pezzo non è già presente nel kernel
        if (kernelPieces[sol1->matrice_pezzi[r][c][0]][0]<0)
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol1->matrice_pezzi[r][c][0]][0];
            r1 = j/col;
            c1 = j % col;
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
        if (kernelPieces[sol2->matrice_pezzi[r][c][0]][1]<0)
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol2->matrice_pezzi[r][c][0]][1];
            r1 = j/col;
            c1 = j % col;
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
        }
        }
    }
    //Generazione lato destro della prole
    for(i=taglio2;i<npieces;i++){
        r = i/col;
        c = i % col;
        flag=0;
        j=taglio1;
        // se il pezzo non è già presente nel kernel
        if (kernelPieces[sol1->matrice_pezzi[r][c][0]][0]<0)
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol1->matrice_pezzi[r][c][0]][0];
            r1 = j/col;
            c1 = j % col;
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
        if (kernelPieces[sol2->matrice_pezzi[r][c][0]][1]<0)
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol2->matrice_pezzi[r][c][0]][1];
            r1 = j/col;
            c1 = j % col;
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
        }
        }
    }
}