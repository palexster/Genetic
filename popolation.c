#define POP_DIM 100
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pieces.h"
#include "popolation.h"
//boolean val
enum{FALSE,TRUE};

/*Funzione che crea la popolazione*/
population_t *build_population(int **pieces,int npieces,int row,int col){
    int i,fit;
    population_t *popolazione_start;
    popolazione_start=(population_t *)malloc(sizeof(population_t));
    popolazione_start->soluzioni=(solution_t *)malloc(sizeof(solution_t)*POP_DIM);
    for(i=0;i<POP_DIM;i++){
        popolazione_start->soluzioni[i]=build_solution(pieces,row,col);
        //genera una popolazione di soluzioni casuali
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

void pop_evolution(int *pieces,int npieces,population_t *pop,int row, int col){
    char chosen[POP_DIM];//vettore di flag per tenere conto che la sol i è già 
                         //estratta (o scelta come genitore o scelta per essere
                         //come genitore)
    long parents[POP_DIM/2];//vettore dell soluzioni scelte come parenti tra cui
                             //estrarre le coppie di genitori per crossover
                             //la soluzione è memorizzata come indice nel vettore
                             //popolazione dichiarato come puntatore generico per
                             //avere di sicuro dimensione di 4 byte (e poter 
                             //rappresentare tutti gli indici senza overflow)
                             //E' usata nche per tenere conto dell'estarazione 
                             //per la scelta degli accoppiamenti se val negativo
                             //allora già estratto
    long i,tmp;//indice su pop e var temp per memorizzare il val estratto a caso
    solution_t offspring[POP_DIM/2];//vettore dei figli
    int cntgen;//per generare coppia e distinguere genitori
    long gen[2];//vettore dei due indici dei genitori accoppiati da passare 
                   //alla funzione di crossover
    /*sceglie i migliori come genitori (sceglie il 25% della popolazione)*/
    for(i=0;i<POP_DIM/4;i++)
        parents[i]=i;
    /*estrae un altro 25% a caso*/
    for(;i<POP_DIM/2;i++){
        tmp=rand()%(3*POP_DIM/4)+25;//tira a caso tra il 75% rimanente della pop
                                    //(esclude il 25% migliore gia selezionato)
        //se l'el. estratto e già stato scelto come genitore
        //prova con il successivo finchè non trova un el. non ancora sel.
        while(!chosen[tmp])
            tmp++;
        parents[i]=tmp;//segna come genitore
        chosen[tmp]=TRUE;//segna come selezionato
    }
    for(i=0;i<POP_DIM/2;){
        for(cntgen=0;cntgen<2;cntgen++){
                tmp=rand()%(POP_DIM/2);
                //se l'el. estratto e già stato accoppiato
                //prova con il successivo finchè non trova un el.da accoppiare.
                while(parents[tmp]<0)
                        tmp++;
                gen[cntgen]=tmp;
        }
        crossover(pop[gen[1]],pop[gen[2]]],&offspring[i++],&offspring[i++]);
    }
}