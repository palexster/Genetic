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

/*struttura dati per la singola soluzione:
 *è composta da una matrice di stringhe (in cui l'elemento i j è il pezzo in 
 * posizione i j nella griglia) da due elementi in cui il primo char è l'indice 
 * del pezzo nel vettore dei pezzi e il secondo indica la rotazione del pezzo
 */    
typedef struct solution_s {
    char ***matrice_pezzi;
    int fitness;
    int feasible;
} solution_t;

typedef enum {
SOTTO,
SINISTRA,
SOPRA,
DESTRA,
} faces_t;

typedef enum {
    FEASIBLE,
    UNFEASIBLE,
} feasible_t;

int **build_pieces(char* filename,int* np,int *border, int* r, int* c);
void test_pieces(int **pieces,int npieces);
void random_solution_generation(solution_t *solution,int *border,int **pieces,int npieces, int row, int col);
void test_solution(solution_t *solution,int row,int col);
int fitness_solution_evaluation(int **pieces,solution_t *sol,int npieces,int row,int col);
solution_t build_solution(int **pieces,int row,int col);
void dealloc_soluzioni(solution_t *sol,int row);
void dealloc_pieces(int **pieces, int npieces);

#ifdef	__cplusplus
}
#endif

#endif	/* pieces_H */

