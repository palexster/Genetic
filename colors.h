/* 
 * File:   colors.h
 * Author: alex
 *
 * Created on 15 marzo 2012, 10.03
 */

#ifndef COLORS_H
#define	COLORS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
typedef struct population_s population_t;

typedef enum {
SOPRA,
DESTRA,
SOTTO,
SINISTRA
} faces_t;

int **build_colors(int pieces);
void test_colors(int **colors,int pieces);
void random_solution_generation(char ***solution,int **colors,int pieces, int row, int col);
void test_solution(char ***solution,int row,int col);
population_t *build_population(int **colors,int pieces,int row,int col);

#ifdef	__cplusplus
}
#endif

#endif	/* COLORS_H */

