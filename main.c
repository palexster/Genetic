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

int main(int argc, char** argv) {
    /* Dichiarazione variabili */
    int row,col;//numero righe e colonne matrice dei pezzi
    int npieces,//numero pezzi
        *border,// vettore dei pezzi di bordo di npieces el.per ogni pezzo dice se è di bordo.val è pari al num di trinagoli grigi(0=centro,1=bordo,2=angolo)
        i;
    int **pieces;//vettore dei colori del pezzo
    solution_t best;//contiene migliore soluzione trovata
    population_t *population; // puntatore a popolazione
 
    srand(time(NULL)); // randomizzazione del generatore di numeri pseudocasuali
    pieces=build_pieces("pieces_10x10.txt",&border,&npieces,&row,&col);
    population=build_population(pieces,border,npieces,row,col);
    const int MAX_PT=(row-1)*col+(col-1)*row;//costante di punti max dipende 
    printf("Punteggio Massimo: %d\n",MAX_PT);
    
/*
    i=rand()%POP_DIM;
    a=rand()%POP_DIM;
    printf("%d %d\n",i,a);
*/
/*
    test_solution(&population->soluzioni[i],row,col);
    test_solution(&population->soluzioni[a],row,col);
    crossover(&population->soluzioni[i],&population->soluzioni[a],&solution,&solution1,npieces,row,col);
    test_solution(&solution,row,col);
    test_solution(&solution1,row,col);
*/
/*
*/
    sorted_popolation(population,pieces);
    test_evolution(population);
    best.fitness=population->soluzioni[0].fitness;
    best.matrice_pezzi=matcp(population->soluzioni[0],row,col);
    if(!(is_best(population,row,col))){
        for(i=0;(i<MAX_ITERATIONS)&&(best.fitness!=MAX_PT);i++){
                if(pop_evolution(pieces,npieces,population,row,col,border)>best.fitness){
                    best.fitness=population->soluzioni[0].fitness;
                    best.matrice_pezzi=matcp(population->soluzioni[0],row,col);
                
                }
                test_evolution(population);
        }
    }
    printf("-----------------------------------\n");
    printf("Miglior soluzione trovata punti:%d\n",best.fitness);
    write_best_solution("Output.txt",best,row,col);
    write_evolution(population,"Statistiche.csv");
    //deallocazione memoria heap
    dealloc_population(population,row,col);
    free(population);
    dealloc_pieces(pieces,npieces);
    free(border);
    return (EXIT_SUCCESS);
}

