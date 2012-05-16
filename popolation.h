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

    
#define POP_DIM 5000
#define GEN_N (POP_DIM/2+(POP_DIM/2)%2)//numero genitori è metà della popolazione
                                   //deve essere pari percui se è dispari somma 1
#define ELITE (POP_DIM/3)//numero di migliori tra i genitori (è pari)
//#define CASUALI (GEN_N-ELITE)//genitori da scegliere a caso (sol 0-elite)
#define RANGE_CAS (POP_DIM-ELITE)//numero di valori tra cui estrarre gli el casuali
                               //evitando di estrarre le sol tra 0 ed elite 
#define N_MISURE 3 // MAX,MEDIA,VARIANZA 
#define MAX_ITERATIONS  50000// MAX_NUMERO_DI_ITERAZIONI in realtà se continua a migliorare best potrebbe essere di
 
    typedef enum {
                MAX,
                MEDIA,
                VARIANZA,
    } misure_t;
    
typedef struct population_s {
    solution_t *soluzioni;
    double bests[2][N_MISURE]; // Vettore per registrare i valore dei massimi durante l'evoluzione
    long current_iteration; // numero dell'iterazione corrente
    long pop_dim;// dimensione popolazione
    long gen_n;//numero genitori è metà della popolazione
                                   //deve essere pari percui se è dispari somma 1
    long elite; //numero di migliori tra i genitori
    long mutation;
    long soglia_escaltion;
} population_t;

//boolean val
enum{FALSE,TRUE};

    
population_t *build_population(int **pieces,int *border,int npieces,int row,int col);
void test_fitness(population_t *pop);
void quick_sort(solution_t *array, int l, int r, int (*cmp)(solution_t lv, solution_t rv));
void dealloc_population(population_t *pop,int row,int col);
void crossover(solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2,int**pieces, int npieces, int row, int col);
void crossover_centro(short signed int **kernelPieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2, int npieces, int row, int col);
void crossover_bordo(short signed int **kernelPieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2,int**pieces, int npieces, int row, int col);
int pop_evolution(int **pieces,int npieces,population_t *pop,int row, int col,int *border);
int get_best(population_t* pop);
int is_best(population_t* pop,int row,int col);
void test_evolution(population_t *pop,solution_t *best,const int MAX_PT,int debug);
void write_best_solution(char *nomefile,solution_t sol,int row,int col);

#ifdef	__cplusplus
}
#endif

#endif	/* POPOLATION_H */

