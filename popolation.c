
#define POP_DIM 100
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "colors.h"
#include "popolation.h"
#include "sort.h"

population_t *build_population(int **colors,int pieces,int row,int col){
    int i,fit;
    population_t *popolazione_start;
    popolazione_start=(population_t *)malloc(sizeof(population_t));
    popolazione_start->soluzioni=(solution_t *)malloc(sizeof(solution_t)*POP_DIM);
    for(i=0;i<POP_DIM;i++){
        popolazione_start->soluzioni[i]=build_solution(colors,row,col);
        random_solution_generation(&popolazione_start->soluzioni[i],colors,row*col,row,col);
        fit=fitness_solution_evaluation(colors,&popolazione_start->soluzioni[i],pieces,row,col);
        popolazione_start->soluzioni[i].fitness=fit;
    }
    return popolazione_start;
}



/*void fitness_popolation_evaluation(popolation_t *pop,char ***solution,int pieces,int row,int col){
    
}
*/

void test_fitness(population_t *pop){
    int i,max=0,idmax=0;
    printf("Chi è la miglior soluzione?\n");
    for(i=0;i<POP_DIM;i++){
        if ((pop->soluzioni[i].fitness)>max){
            max=pop->soluzioni[i].fitness;
            idmax=i;
        }
        printf("Soluzione %d:\t %d \n",i,pop->soluzioni[i].fitness);
    }
    printf("Miglior Soluzione n° %d , punti: %d",idmax,pop->soluzioni[idmax].fitness);
}

int cmp_fitness(void * s1,void *s2){
    solution_t s1cast,s2cast;
    s1cast=*(solution_t *)s1;
    s2cast=*(solution_t *)s2;
    if (s1cast.fitness>s2cast.fitness)
        return 1;
    if (s1cast.fitness<s2cast.fitness)
        return -1;
    return 0;
}

void sorted_popolation(population_t *pop,int **colors){
  quick_sort((void **)pop->soluzioni,POP_DIM,0,(int)cmp_fitness);  
}