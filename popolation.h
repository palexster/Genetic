/* 
 * File:   popolation.h
 * Author: alex
 *
 * Created on 16 marzo 2012, 20.49
 */

#ifndef POPOLATION_H
#define	POPOLATION_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pieces.h"
#include <math.h>
    
#define POP_DIM 10000
#define GEN_N (POP_DIM/2+(POP_DIM%2))//numero genitori è metà della popolazione
                                   //deve essere pari percui se è dispari somma 1
#define ELITE (POP_DIM/3)//numero di migliori tra i genitori (è pari)
//#define CASUALI (GEN_N-ELITE)//genitori da scegliere a caso (sol 0-elite)
#define RANGE_CAS (POP_DIM-ELITE)//numero di valori tra cui estrarre gli el casuali
                               //evitando di estrarre le sol tra 0 ed elite 
#define MAX_ITERATIONS 1000 // MAX_NUMERO_DI_ITERAZIONI
#define N_MISURE 3 // MAX,MEDIA,VARIANZA 
    
    typedef enum {
                MAX,
                MEDIA,
                VARIANZA,
    } misure_t;
    
typedef struct population_s {
    solution_t *soluzioni;
    float bests[MAX_ITERATIONS+1][N_MISURE]; // Vettore per registrare i valore dei massimi durante l'evoluzione
    int current_iteration; // numero dell'iterazione corrente
} population_t;

//boolean val
enum{FALSE,TRUE};

    
population_t *build_population(int **pieces,int *border,int npieces,int row,int col);
void test_fitness(population_t *pop);
void quick_sort(solution_t *array, int l, int r, int (*cmp)(solution_t lv, solution_t rv));
void dealloc_population(population_t *pop,int row,int col);
void crossover(solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2,int**pieces, int npieces, int row, int col);
void crossover_centro(int **kernelPieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2, int npieces, int row, int col);
void crossover_bordo(int **kernelPieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2,int**pieces, int npieces, int row, int col);
int pop_evolution(int **pieces,int npieces,population_t *pop,int row, int col,int *border);
int get_best(population_t* pop);
int is_best(population_t* pop,int row,int col);
void test_evolution(population_t *pop,solution_t *best,const int MAX_PT);
void write_best_solution(char *nomefile,solution_t sol,int row,int col);
#ifdef	__cplusplus
}
#endif

#endif	/* POPOLATION_H */

