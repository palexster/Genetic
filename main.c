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
    int row,col,//numero righe e colonne matrice dei pezzi
        npieces,//numero pezzi
        stop=1;// flag di fine generazione
    int **pieces;//vettore dei colori del pezzo
    solution_t solution,solution1;
    population_t *population,*pop;
    srand(time(NULL));
    pieces=build_pieces("pieces_10x10.txt",&npieces,&row,&col);
    //test_pieces(pieces,npieces);
    population=build_population(pieces,npieces,row,col);
    sorted_popolation(population,pieces);
    // comincia l'evoluzione... i criteri di stop
    // 1) Soluzione Ottima
    // 2) Troppe epoche
    while (!stop){
        // pop evolution dovrebbe dire al main se vuole ancora andare avanti
        // ad ogni generazione va avanti e incrementa il valore di stop
        // o può settare a false la flag se ha trovato la soluzione ottima
        //stop=pop_evolution(pieces,npieces,pop,row,col);
    }
    dealloc_population(population,row);
    free(population);
    return (EXIT_SUCCESS);
}

