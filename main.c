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

/*
 * 
 */
int main(int argc, char** argv) {
    /* Dichiarazione variabili */
    int row,col,//numero righe e colonne matrice dei pezzi
        npieces;//numero pezzi    
    int **pieces;//vettore dei colori del pezzo
    solution_t solution,solution1;
    population_t *population,*pop;
    long max_iterations,i;
 
    srand(time(NULL));
    pieces=build_pieces("pieces_10x10.txt",&npieces,&row,&col);
    //test_pieces(pieces,npieces);
    population=build_population(pieces,npieces,row,col);
    test_fitness(population);
    scanf("%d",&max_iterations);
    for(i=0;pop_evolution(pieces,npieces,population,row,col)!=OPT_SOL&&(i<max_iterations);i++){
        //sorted_popolation(population,pieces);
        test_fitness(population);
        //DEBUG
        int a;
        scanf("%d",&a);
        //END DEBUG
    }
    dealloc_population(population,row);
    free(population);
    return (EXIT_SUCCESS);
}

