#include <stdio.h>
#include <stdlib.h>
#include "colors.h"

int **build_colors(int row, int col){
    int **mat,pieces,i;
    pieces=row*col;
    mat=(int **)malloc(sizeof(int*)*pieces); // Allocazione numero di colori
    for(i=0;i<pieces;i++){
        mat[i]=(int *)malloc(sizeof(int)*4); // 4 colori per pezzo
        }
    return mat;
}