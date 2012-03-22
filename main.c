/* 
 * File:   main.c
 * Author: alex
 *
 * Created on 7 marzo 2012, 16.28
 */
#include <stdio.h>
#include <stdlib.h>
#include "pieces.h"
#include "sort.h"
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
 
    srand(time(NULL));
    pieces=build_pieces("pieces_10x10.txt",&npieces,&row,&col);
    test_pieces(pieces,npieces);
    population=build_population(pieces,npieces,row,col);
    //solution=build_solution(pieces,row,col);
    //random_solution_generation(solution,pieces,npieces,row,col);
    //test_solution(solution,row,col);
    //test_fitness(population);
    //solution1=build_solution(pieces,row,col);
    //random_solution_generation(solution1,pieces,npieces,row,col);
    //test_solution(solution1,row,col);
    //sorted_popolation(population,pieces);
    test_fitness(population);
    return (EXIT_SUCCESS);
}

