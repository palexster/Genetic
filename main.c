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
#define MAX_ITERATIONS 1

#ifdef _OPENMP
   #define TRUE  1
   #define FALSE 0
#else
   #define omp_get_thread_num() 0
#endif


int main(int argc, char** argv) {
    /* Dichiarazione variabili */
    int row,col,a;//numero righe e colonne matrice dei pezzi
    int npieces,//numero pezzi
        stop=1,// flag di fine generazione
        *border,// vettore dei pezzi di bordo
        i;
    int **pieces;//vettore dei colori del pezzo
    solution_t solution,solution1;
    population_t *population;
    long max_iterations;
    srand(time(NULL)); // randomizzazione del generatore di numeri pseudocasuali
    pieces=build_pieces("pieces_05x05.txt",&border,&npieces,&row,&col);
    population=build_population(pieces,border,npieces,row,col);
    sorted_popolation(population,pieces);
    test_fitness(population);
    // comincia l'evoluzione... i criteri di stop
    // 1) Soluzione Ottima
    // 2) Troppe epoche
    //se la prima sol non itera neanche
/*
    if(!(is_best(population,row,col))){
        for(i=0;pop_evolution(pieces,npieces,population,row,col)!=OPT_SOL&&(i<MAX_ITERATIONS);i++){
       test_fitness(population);
       //DEBUG
       scanf("%d",&a);
        }
    }
*/
    write_best_solution("Output.txt",population,row,col);
    //deallocazione memoria heap
    dealloc_population(population,row);
    free(population);
    dealloc_pieces(pieces,npieces);
    return (EXIT_SUCCESS);
}

