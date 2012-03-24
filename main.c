/* 
 * File:   main.c
 * Author: alex
 *
 * Created on 7 marzo 2012, 16.28
 */
#include <stdio.h>
#include <stdlib.h>
#include "pieces.h"
#include "popolation.h"
#include <omp.h>

#ifdef _OPENMP
   #define TRUE  1
   #define FALSE 0
#else
   #define omp_get_thread_num() 0
#endif

/*
 * 
 */
int main(int argc, char** argv) {
    #ifdef _OPENMP
   (void) omp_set_dynamic(TRUE);
   if (omp_get_dynamic()) {printf("Warning: dynamic adjustment of threads has been set\n");}
   (void) omp_set_num_threads(2);
    #endif
    /* Dichiarazione variabili */
    int row,col,//numero righe e colonne matrice dei pezzi
        npieces;//numero pezzi    
    int **pieces;//vettore dei colori del pezzo
    solution_t solution,solution1;
    population_t *population,*pop;
 
    srand(time(NULL));
    pieces=build_pieces("pieces_10x10.txt",&npieces,&row,&col);
    //test_pieces(pieces,npieces);
    population=build_population(pieces,npieces,row,col);
    test_fitness(population);
    sorted_popolation(population,pieces);
    test_fitness(population);
    dealloc_population(population,row);
    free(population);
    return (EXIT_SUCCESS);
}

