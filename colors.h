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
    
typedef struct solution_s {
    char ***matrice_pezzi;
    int fitness;
} solution_t;

typedef enum {
SOPRA,
DESTRA,
SOTTO,
SINISTRA
} faces_t;

int **build_colors(int pieces);
void test_colors(int **colors,int pieces);
void random_solution_generation(solution_t *solution,int **colors,int pieces, int row, int col);
void test_solution(solution_t *solution,int row,int col);
int fitness_solution_evaluation(int **colors,solution_t *sol,int pieces,int row,int col);
solution_t build_solution(int **colors, int row,int col);

#ifdef	__cplusplus
}
#endif

#endif	/* COLORS_H */

