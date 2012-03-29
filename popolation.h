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

typedef struct population_s {
    solution_t *soluzioni;
} population_t;
    
population_t *build_population(int **pieces,int npieces,int row,int col);
void test_fitness(population_t *pop);
void quick_sort(solution_t *array, int l, int r, int (*cmp)(solution_t lv, solution_t rv));
void dealloc_population(population_t *pop,int row);
void crossover(int **pieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2, int npieces, int row, int col);

#ifdef	__cplusplus
}
#endif

#endif	/* POPOLATION_H */

