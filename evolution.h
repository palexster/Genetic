/* 
 * File:   evolution.h
 * Author: nasuto
 *
 * Created on April 27, 2012, 3:14 PM
 */
#include"popolation.h"

#ifndef EVOLUTION_H
#define	EVOLUTION_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* EVOLUTION_H */

void parent_selection(long parents[]);
void offspring_generation(int **pieces,int npieces,population_t *pop,long parents[],solution_t offspring[],int row, int col);
void substitution(population_t *pop,solution_t offspring[],int row, int col);
void mutation(int **pieces,int npieces,population_t *pop,int row, int col,int *border);
