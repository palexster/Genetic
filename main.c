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

#ifdef _OPENMP
  #include <omp.h>
  #define TRUE  1
  #define FALSE 0
#else
  #define omp_get_thread_num() 0
  #define omp_get_num_threads() 1
#endif

int main(int argc, char** argv) {
    /* Dichiarazione variabili */
    #ifdef _OPENMP
   (void) omp_set_dynamic(FALSE);
   if (omp_get_dynamic()) {printf("Warning: dynamic adjustment of threads has been set\n");}
   (void) omp_set_num_threads(2);
    #endif
    int row,col;//numero righe e colonne matrice dei pezzi
    int npieces,//numero pezzi
        *border,// vettore dei pezzi di bordo di npieces el.per ogni pezzo dice se è di bordo.val è pari al num di trinagoli grigi(0=centro,1=bordo,2=angolo)
        i;
    int **pieces;//vettore dei colori del pezzo
    solution_t best;//contiene migliore soluzione trovata
    population_t *population; // puntatore a popolazione
    srand(time(NULL)); // randomizzazione del generatore di numeri pseudocasuali
    #pragma omp single
    if (argc != 4){
        fprintf(stderr,"Usage: %s input_file output_file stats_file",argv[0]);
        exit(2);
    } 
    pieces=build_pieces(argv[1],&border,&npieces,&row,&col);
    population=build_population(pieces,border,npieces,row,col);
    const int MAX_PT=(row-1)*col+(col-1)*row;//costante di punti max dipende 
    //printf("Punteggio Massimo: %d\n",MAX_PT);
    sorted_popolation(population,pieces);
    test_evolution(population,&best,MAX_PT);
    best.fitness=population->soluzioni[0].fitness;
    best.matrice_pezzi=matcp(population->soluzioni[0],row,col);
    if(!(is_best(population,row,col))){
        for(i=0;(i<MAX_ITERATIONS)&&(best.fitness!=MAX_PT);i++){
                if(pop_evolution(pieces,npieces,population,row,col,border)>best.fitness){
                    best.fitness=population->soluzioni[0].fitness;
                    best.matrice_pezzi=matcp(population->soluzioni[0],row,col);
                
                }
                test_evolution(population,&best,MAX_PT);
        }
    }
    write_best_solution(argv[2],best,row,col);
    write_evolution(population,argv[3]);
    //deallocazione memoria heap
    dealloc_population(population,row,col);
    free(population);
    dealloc_pieces(pieces,npieces);
    free(border);
    return (EXIT_SUCCESS);
}

