/* 
 * File:   pieces.h
 * Author: alex
 *
 * Created on 15 marzo 2012, 10.03
 */

#ifndef pieces_H
#define	pieces_H

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

int **build_pieces(char* filename,int* np, int* r, int* c);
void test_pieces(int **pieces,int npieces);
void random_solution_generation(solution_t *solution,int **pieces,int npieces, int row, int col);
void test_solution(solution_t *solution,int row,int col);
int fitness_solution_evaluation(int **pieces,solution_t *sol,int npieces,int row,int col);
solution_t build_solution(int **pieces,int row,int col);

#ifdef	__cplusplus
}
#endif

#endif	/* pieces_H */

