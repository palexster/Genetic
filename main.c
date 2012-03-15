/* 
 * File:   main.c
 * Author: alex
 *
 * Created on 7 marzo 2012, 16.28
 */

#include <stdio.h>
#include <stdlib.h>
#include "colors.h"

/*
 * 
 */
int main(int argc, char** argv) {
    /* Dichiarazione variabili */
    
    FILE *fp;
    int row,col;
    int **colors;
    printf("Hello World!!!");
    
    /*Lettura dimensioni della matrice e allocazione del vettore dei pezzi*/
    
    fp=fopen("pieces_05x05.txt","r");
    fscanf(fp,"%d%d",&row,&col);
    colors=build_colors(row,col);
    
    /*Riempimento matrice dei pezzi con i colori dei singoli pezzi*/
    
    
    
    return (EXIT_SUCCESS);
}

