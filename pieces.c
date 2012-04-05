#define COLN 4  //numero di colori per pezzo
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "pieces.h"
#include "popolation.h"
#define GRAY 0

    
/*funzione per l'allocazione del vettore e caricamento dei dati
 Riceve nome del file in ingresso
 Ritorna il vettore dei pezzi(vettore di vettori di 4 interi che rappresentano i colori)*/
int **build_pieces(char* filename, int **border,int* np, int* r, int* c){
    int **mat,nbordo;//matrice di interi per allocare la struttura dati dei pezzi
    FILE *fp;//puntatore al file che contiene i pezzi
    int row,col,//numero righe e colonne matrice dei pezzi
        npieces,//numero pezzi
        line[4],//vettore temporaneo per caricare i colori
            *b,
        i,j;//indici della matrice
    /*Lettura dimensioni della matrice e allocazione del vettore dei pezzi*/
    if((fp=fopen(filename,"r"))==NULL){
        fprintf(stderr,"impossibile aprire il file.\n");
        exit(2);
    }
    if ((fscanf(fp,"%d%d",&row,&col))==EOF)
        exit(2);
    npieces=row*col;
    b=(int *)malloc(sizeof(int)*npieces);
    if(b==NULL){
        fprintf(stderr,"build_pieces()-errore in malloc() di b.\n");
        exit(2);
    }
    for(i=0;i<npieces;i++){
        b[i]=0;
        }
    mat=(int **)malloc(sizeof(int*)*npieces); // Allocazione numero di colori
    if(mat==NULL){
        fprintf(stderr,"build_pieces()-errore in malloc() di mat.\n");
        exit(2);
    }
    for(i=0;i<npieces;i++){
        mat[i]=(int *)malloc(sizeof(int)*COLN); // 4 colori per pezzo
        if(mat[i]==NULL){
        fprintf(stderr,"build_pieces()-errore in malloc() di mat[%d].\n",i);
        exit(2);
    }
        }
    
    /*Riempimento matrice dei pezzi con i colori dei singoli pezzi*/
    for(i=0;i<npieces;i++){
        if ((fscanf(fp,"%d%d%d%d",&line[0],&line[1],&line[2],&line[3]))==EOF){
            fprintf(stderr,"file corrotto.\n");
            exit(2);
        }
                    nbordo=0;
        for(j=0;j<COLN;j++){
            if (line[j]==GRAY){
                nbordo++;
            }
                mat[i][j]=line[j];     
        }
        if (nbordo>0){
                b[i]=nbordo;
        }
        //printf("Il pezzo %d ha %d volte il grigio. Ripeto %d volte\n",i,nbordo,border[i]);
     }
     *border=b;
     *np=npieces;
     *r=row;
     *c=col;
     return mat;
    }
    
/*verifica caricamento corretto del vettore dei colori*/
void test_pieces(int **pieces,int npieces){
    int i,j;
    for(i=0;i<npieces;i++){
        for(j=0;j<COLN;j++){
                printf("%d \t",pieces[i][j]);
     }
        printf("\n");
    }
}

/*Ogni soluzione è composta da una matrice di stringhe da due elementi in cui il
 *primo char è l'indice del pezzo nel vettore dei pezzi e il secondo indica la 
 *rotazione del pezzo.
 Funzione che alloca la soluzione*/
solution_t build_solution(int **pieces, int row,int col){
    solution_t *solution; // Puntatore alla soluzione che deve essere allocata
    int i,j;
    solution=(solution_t *)malloc(sizeof(solution_t));
    if(solution==NULL){
        fprintf(stderr,"build_solution()-errore in malloc() di solution.\n");
        exit(2);
    }
    solution->matrice_pezzi=(char ***)malloc(sizeof(char**)*row);
    if(solution->matrice_pezzi==NULL){
        fprintf(stderr,"build_solution()-errore in malloc() di solution->matrice_pezzi.\n");
        exit(2);
    }
    for(i=0;i<row;i++){
        solution->matrice_pezzi[i]=(char **)malloc(sizeof(char *)*col);
        if(solution->matrice_pezzi[i]==NULL){
                fprintf(stderr,"build_solution()-errore in malloc() di solution->matrice_pezzi[%d].\n",i);
                exit(2);
        }
        for(j=0;j<col;j++){
            solution->matrice_pezzi[i][j]=(char *)malloc(sizeof(char)*2);
            if(solution->matrice_pezzi[i]==NULL){
                fprintf(stderr,"build_solution()-errore in malloc() di solution->matrice_pezzi[%d][%d].\n",i,j);
                exit(2);
            }
        }
    }
    solution->feasible=FEASIBLE;
    return *solution;
}
/*
 * Funziona che riempie una soluzione iniziale con i pezzi del vettore pieces in
 * maniera casuale
 */
void random_solution_generation(solution_t *solution,int *border,int **pieces,int npieces, int row, int col){
    char *taken //vettore dei pezzi già inseriti nella soluzione
    ,n_pieces_taken, // numero di pezzi già inseriti nella soluzione
     random_number, // numero pseudocasuale generato per decidere quale pezzo
            perimetro,//bordo della matrice che rappresenta la soluzione
            random_rotation,
            corners[4], // indice degli angoli
            *border_pieces, // indice dei bordi
            *border_taken,
            corner_taken[4];
    int i,j,k;
    taken=(char *)malloc(sizeof(char)*npieces);
    if(taken==NULL){
        fprintf(stderr,"random_solution_generation()-errore in malloc() di taken.\n");
        exit(2);
    }
    border_pieces=(char *)malloc(sizeof(char)*perimetro);
    if(border_pieces==NULL){
        fprintf(stderr,"random_solution_generation()-errore in malloc() di border_pieces.\n");
        exit(2);
    }
    border_taken=(char *)malloc(sizeof(char)*perimetro);
    if(border_taken==NULL){
        fprintf(stderr,"random_solution_generation()-errore in malloc() di border_taken.\n");
        exit(2);
    }
    for(i=0;i<npieces;i++){
        taken[i]=0;
        border_taken[i]=0;
    }
    n_pieces_taken=0;
    perimetro=(row-1+col-1)*2;

    i=0;
    j=0;
    n_pieces_taken=0;
    k=0;
    //cicli preliminari per riempire il vettore dei pezzi di bordo e di angolo
    while ((k+j) < (perimetro)){
        if (border[i] == 1){
            border_pieces[j]=i;
            n_pieces_taken++;
            j++;
        }
        if (border[i] == 2){
            corners[k]=i;
            n_pieces_taken++;
            k++;
        }
        i++;
        if (i==npieces)
            i=0;
    }
    // Riempimento angoli
    //Angolo alto-sinistra;
    solution->matrice_pezzi[0][0][0]=get_right_corner(pieces,corner_taken);
    solution->matrice_pezzi[0][0][1]=get_corner_fitting_rotation(pieces,solution->matrice_pezzi[0][0][0],SINISTRA,SOPRA);
    //Angolo alto-destra;
    solution->matrice_pezzi[0][col-1][0]=get_right_corner(pieces,corner_taken);
    solution->matrice_pezzi[0][col-1][1]=get_corner_fitting_rotation(pieces,solution->matrice_pezzi[0][col-1][0],SOPRA,DESTRA);
    //Angolo basso-destra;
    solution->matrice_pezzi[row-1][col-1][0]=get_right_corner(pieces,corner_taken);
    solution->matrice_pezzi[row-1][col-1][1]=get_corner_fitting_rotation(pieces,solution->matrice_pezzi[row-1][col-1][0],SOPRA,DESTRA);
    //Angolo basso-sinistra;
    solution->matrice_pezzi[row-1][0][0]=get_right_corner(pieces,corner_taken);
    solution->matrice_pezzi[row-1][0][1]=get_corner_fitting_rotation(pieces,solution->matrice_pezzi[row-1][col-1][0],SOPRA,DESTRA);
    for(i=0;i<row;i++){
        get_right_border(pieces,solution,border_taken,perimetro,i,0,SOPRA);
        get_right_border(pieces,solution,border_taken,perimetro,i,col-1,SOPRA);
    }
    for(j=0;j<col;j++){
        get_right_border(pieces,solution,border_taken,perimetro,0,j,SOPRA);
        get_right_border(pieces,solution,border_taken,perimetro,row-1,j,SOPRA);
    }
    /*Ciclo sulla matrice della soluzione e cerco un pezzo non preso casuale, usando una rotazion
     casuale. Se è preso, vado a quello dopo. Se arrivo al fondo ricomincio dall'inizio. Ci sono
     metodi molto migliori ma poi lo raffiniamo*/
    for(i=1;i<(row-1);i++)
       for(j=1;j<(col-1);j++){
           random_number = rand() % npieces;
        if (!taken[random_number]){
            ++taken[random_number];
            solution->matrice_pezzi[i][j][0]=(char)random_number;
            ++n_pieces_taken;
            random_rotation= rand() % COLN;
            solution->matrice_pezzi[i][j][1]=random_rotation;
        }
        else {
            while (taken[random_number]){
                random_number++;
            if (random_number>=npieces)
                random_number=0;
            }
            solution->matrice_pezzi[i][j][0]=(char)random_number;
            random_rotation= rand() % COLN;
            solution->matrice_pezzi[i][j][1]=random_rotation;
            ++taken[random_number];
        }
       }
    //per ora ti ho messo le free qua!
    free(border_taken);
    free(border_pieces);
    free(taken);
    return;
}

void test_solution(solution_t *solution,int row,int col){
    int i,j;
    printf("-----------------------------------\n");
    for(i=0;i<row;i++){
        for(j=0;j<col;j++){
                printf("%d %d \t",solution->matrice_pezzi[i][j][0],solution->matrice_pezzi[i][j][1]);
     }
        printf("\n");
    }
}

int fitness_solution_evaluation(int **pieces,solution_t *solution,int npieces,int row,int col){
    //,bordo_inferiore,bordo_laterale;

    // a e b sono utilizzate per memorizzare i colori da confrontare dei due pezzi
    // rot_first e rot_sec per memorizzare la rotazione dei due pezzi considerando 
    // la rotazione del pezzo nella soluzione
    int a,b,i,j,rot_first,rot_sec,profit=0,test;

    for(i=0;i<row;i++)
        for(j=0;j<col;j++){
            // se è l'ultima colonna non controlla il profit laterale
            if (j<(col-1)){
                // l'ultimo indice 1 indica la rotazione, 0 il numero del pezzo
                test=DESTRA;
                rot_first=DESTRA-solution->matrice_pezzi[i][j][1];
                if (rot_first<0)
                    rot_first=4+rot_first;
                // Controllo bounding del pezzo, utile solo in fase di backup
                //if (rot_first<0 || rot_first>4)
                //    printf("Out of bounds reading primo pezzo orizzontale");
                rot_sec=SINISTRA-solution->matrice_pezzi[i][j+1][1];
                if (rot_sec<0)
                    rot_sec=4+rot_sec;
                // Controllo bounding del pezzo, utile solo in fase di backup
                //if (rot_sec<0 || rot_sec>4)
                //    printf("Out of bounds reading secondo pezzo orizzontale");
                a = pieces[solution->matrice_pezzi[i][j][0]][rot_first];
                b = pieces[solution->matrice_pezzi[i][j+1][0]][rot_sec];
                if (a == b && a != GRAY)
                        profit++;
                if (a == GRAY || b == GRAY)
                    solution->feasible=UNFEASIBLE;
            }
            // se è l'ultima riga non controlla il profit orizzontale
            if (i<(row-1)){
                rot_first=SOTTO-solution->matrice_pezzi[i][j][1];
                if (rot_first<0)
                    rot_first=4+rot_first;
/*
                if (rot_first<0 || rot_first>4)
                    printf("Out of bounds reading primo pezzo verticale");
*/
                rot_sec=SOPRA-solution->matrice_pezzi[i+1][j][1];
                if (rot_sec<0)
                    rot_sec=4+rot_sec;
/*
                if (rot_sec<0 || rot_sec>4)
                    printf("Out of bounds reading secondo pezzo orizzontale");
*/
                a=pieces[solution->matrice_pezzi[i][j][0]][rot_first];
                b=pieces[solution->matrice_pezzi[i+1][j][0]][rot_sec];
                if (a==b &&  a != GRAY )
                        profit++;
                if (a == GRAY || b == GRAY)
                    solution->feasible=UNFEASIBLE;
            }            
        }
    return profit;
}

void dealloc_soluzioni(solution_t *sol,int row){
    int count;
    for (count = 0; count < row; count++)
                free(sol->matrice_pezzi[count]);
    free(sol->matrice_pezzi);
    return;
} 

void dealloc_pieces(int **pieces, int npieces){
    int i;
    for(i=0;i<npieces;i++){
        free(pieces[i]);
        }
    free(pieces);
}


int get_border_fitting_rotation(int **pieces,int border_index, int bordo){
    int rotation=0,i;
    for(i=0;i<4;i++)
        if (!(pieces[border_index][i]))
            break;
    // bisogna che il pezzo border index sia ruotato affinchè il grigio sia esterno
    // bordo mi dà la posizione del grigio
    // in i memorizzo l'indice del colore grigio nel pezzo
    while (i != bordo){
        rotation++;
        i++;
        if(i==4)
            i=0;
    }
    return rotation;
}       

void get_right_border(int **pieces,solution_t *solution,char *border_taken,int perimetro,int i,int j,int posizione)
{
    int random_rotation,random_number,fine; 
    random_number = rand() % perimetro;
    fine=random_number;
     if (!border_taken[random_number]){
                   ++border_taken[random_number];
                   solution->matrice_pezzi[i][j][0]=(char)random_number;
                   random_rotation=get_border_fitting_rotation(pieces,random_number,posizione);
                   solution->matrice_pezzi[i][j][1]=random_rotation;
                   ++random_number;
     }
    else {
                while (border_taken[random_number] ){
                random_number++;
                if (random_number>=perimetro)
                        random_number=0;
/*
                if (fine == random_number){
                    printf("Errore random Number = %d\n",random_number);
                    return;
                }
*/
                //++border_taken[random_number];
               }
                ++border_taken[random_number];
                solution->matrice_pezzi[i][j][0]=random_number;
                random_rotation=get_border_fitting_rotation(pieces,random_number,posizione);
                solution->matrice_pezzi[i][j][1]=random_rotation;
                ++random_number;
    }
    return;
}


char get_right_corner(int **pieces,char *corner_taken){
    char random_number;
    random_number= (char)(rand() % 4);
    while (corner_taken[random_number]){
        random_number= (char)((random_number+1)%4);
    }
    ++corner_taken[random_number];
    return random_number;
}

/*Seleziona la rotazione giusta per un pezzo d'angolo
 * Corner index è il pezzo selezionato all'interno del vettore pieces per essere
 * messo nell'angolo, i e j caratterizzano la posizione dell'angolo (alto-destra...)
 */

char get_corner_fitting_rotation(int **pieces,char corner_index,int i,int j){
    int k,l;
    char rotation;
    rotation=0;
    for(k=0;k<3;k++){
        if (pieces[corner_index][k] == GRAY){
            if (pieces[corner_index][k+1] == GRAY){
            l=k+1;
            break;
            }
            else {
                l=k;
                --k;
                if (k<0)
                    k=4-k;
                break;
            }
        }
    }
    // bisogna che il pezzo corner sia ruotato affinchè il grigio sia esterno
    // bordo mi dà la posizione del grigio
    // in i memorizzo l'indice del colore grigio nel pezzo
    while (((((k+rotation)%4) != i) || (((l+rotation)%4) != j)) && ((((k+rotation)%4) !=j) || (((l+rotation)%4) != i) )){
        ++rotation;
    }
    return rotation;
}