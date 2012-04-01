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
    
#define OPT_SOL 0
#define EVOLVI_ANCORA 1
    
typedef struct population_s {
    solution_t *soluzioni;
} population_t;

//boolean val
enum{FALSE,TRUE};

    
population_t *build_population(int **pieces,int *border,int npieces,int row,int col);
void test_fitness(population_t *pop);
void quick_sort(solution_t *array, int l, int r, int (*cmp)(solution_t lv, solution_t rv));
void dealloc_population(population_t *pop,int row);
void crossover(int **pieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2, int npieces, int row, int col);
int pop_evolution(int **pieces,int npieces,population_t *pop,int row, int col);
int get_best(population_t* pop);
int is_best(population_t* pop,int row,int col);

void write_best_solution(char *nomefile,population_t *pop,int row,int col);
#ifdef	__cplusplus
}
#endif

#endif	/* POPOLATION_H */

