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
        line[COLN],//vettore temporaneo per caricare i colori
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
                //printf("Grigio Trovato in %d facce nel pezzo %d\n",nbordo,i);
        }
        //printf("Il pezzo %d ha %d volte il grigio. Ripeto %d volte\n",i,nbordo,border[i]);
     }
     const int MAX_PT=(row-1)*col+(col-1)*row;//costante di punti max dipende 
     printf("Punteggio Massimo: %d\n",MAX_PT);
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
solution_t build_solution(int row,int col){
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
    char *taken, //vettore dei pezzi già inseriti nella soluzione
    n_pieces_taken, // numero di pezzi già inseriti nella soluzione
     random_number, // numero pseudocasuale generato per decidere quale pezzo
            perimetro,//bordo della matrice che rappresenta la soluzione
            random_rotation,
            corners[4], // indice degli angoli
            *border_pieces, // indice dei bordi
            *border_taken,
            corner_taken[4];
    int i,j,k;
    perimetro=(row-1+col-1)*2;
    taken=(char *)malloc(sizeof(char)*npieces);
    if(taken==NULL){
        fprintf(stderr,"random_solution_generation()-errore in malloc() di taken.\n");
        exit(2);
    }
    // Perimetro -4 per non considerare gli angoli!
    border_pieces=(char *)malloc(sizeof(char)*(perimetro-4));
    if(border_pieces==NULL){
        fprintf(stderr,"random_solution_generation()-errore in malloc() di border_pieces.\n");
        exit(2);
    }
    border_taken=(char *)malloc(sizeof(char)*(perimetro-4));
    if(border_taken==NULL){
        fprintf(stderr,"random_solution_generation()-errore in malloc() di border_taken.\n");
        exit(2);
    }
    // azzeramento delle matrice allocate per contare i pezzi
    for(i=0;i<4;i++){
        corner_taken[i]=0;
    }
    for(i=0;i<npieces;i++){
        taken[i]=0;
    }
    for(i=0;i<perimetro-4;i++){
        border_taken[i]=0;
    }
    n_pieces_taken=0;
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
            //printf("Pezzo di bordo in %d\n",i);
        }
        if (border[i] == 2){
            corners[k]=i;
            n_pieces_taken++;
            k++;
            //printf("Pezzo di angolo in %d\n",i);
        }
        i++;
        if (i==npieces)
            i=0;
    }
    // Controllo Che il vettore taken sia a 0
    // Bordi e angoli finiti, quali sono stati presi?
/*
    printf("--Tutti a 0 dovrebbero essere---\n");
     for(j=1;j<npieces;j++){    
         printf("Pezzo %d: %d\n",j,taken[j]);
    }
*/
    // Riempimento angoli
    //Angolo alto-sinistra;
    solution->matrice_pezzi[0][0][0]=get_right_corner(pieces,corner_taken,corners);
    solution->matrice_pezzi[0][0][1]=get_corner_fitting_rotation(pieces,solution->matrice_pezzi[0][0][0],SINISTRA,SOPRA);
    ++taken[solution->matrice_pezzi[0][0][0]];
    //Angolo alto-destra;
    solution->matrice_pezzi[0][col-1][0]=get_right_corner(pieces,corner_taken,corners);
    solution->matrice_pezzi[0][col-1][1]=get_corner_fitting_rotation(pieces,solution->matrice_pezzi[0][col-1][0],SOPRA,DESTRA);
    ++taken[solution->matrice_pezzi[0][col-1][0]];
    //Angolo basso-destra;
    solution->matrice_pezzi[row-1][col-1][0]=get_right_corner(pieces,corner_taken,corners);
    solution->matrice_pezzi[row-1][col-1][1]=get_corner_fitting_rotation(pieces,solution->matrice_pezzi[row-1][col-1][0],SOTTO,DESTRA);
    ++taken[solution->matrice_pezzi[row-1][col-1][0]];
    //Angolo basso-sinistra;
    solution->matrice_pezzi[row-1][0][0]=get_right_corner(pieces,corner_taken,corners);
    solution->matrice_pezzi[row-1][0][1]=get_corner_fitting_rotation(pieces,solution->matrice_pezzi[row-1][col-1][0],SOTTO,SINISTRA);
    ++taken[solution->matrice_pezzi[row-1][0][0]];
    // Bordi e angoli finiti, quali sono stati presi?
    /* printf("--Tutti a 0 dovrebbero essere tranne angoli a 1---\n");
       //Codice utilizzato per il il debugging
     for(j=0;j<npieces;j++){
         if (taken[j]>1)
             fprintf(stderr,"Pezzo %d DUPLICATO!!!\n",j);
         //printf("Pezzo %d: %d\n",j,taken[j]);
    }
*/
    //Riempimento pezzi di bordo
    for(i=1;i<(row-1);i++){
        get_right_border(pieces,solution,taken,border_taken,border_pieces,perimetro-4,i,0,SINISTRA);
        get_right_border(pieces,solution,taken,border_taken,border_pieces,perimetro-4,i,col-1,DESTRA);
    }
    for(j=1;j<(col-1);j++){
        get_right_border(pieces,solution,taken,border_taken,border_pieces,perimetro-4,0,j,SOPRA);
        get_right_border(pieces,solution,taken,border_taken,border_pieces,perimetro-4,row-1,j,SOTTO);
    }
    // Bordi e angoli finiti, quali sono stati presi?
/*
    printf("--Tutti a 0 dovrebbero essere tranne angoli a 1---\n");
     for(j=0;j<npieces;j++){
         if (taken[j]>1)
             fprintf(stderr,"Pezzo %d DUPLICATO!!!\n",j);
        // printf("Pezzo %d: %d\n",j,taken[j]);
    }
*/
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
            //printf("Pezzo preso  numero %d in posizione %d %d\n",random_number,i,j);
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
/*Codice per il debugging e la rilevazione di estrazioni duplicate!
    for(j=0;j<npieces;j++){
         if (taken[j]>1)
             fprintf(stderr,"Pezzo %d DUPLICATO!!!\n",j);
         //printf("Pezzo %d: %d\n",j,taken[j]);
    }
*/
    //per ora ti ho messo le free qua!
    free(border_pieces);
    free(taken);
    free(border_taken);
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
    int a,b,i,j,rot_first,rot_sec,profit=0;
    //test_solution(solution,row,col);
    for(i=0;i<row;i++)
        for(j=0;j<col;j++){
            ///printf("Coordinate %d\t %d\n",i,j);
            // se è l'ultima colonna non controlla il profit laterale
            if (j<(col-1)){
                // l'ultimo indice 1 indica la rotazione, 0 il numero del pezzo
                 //printf("A --> Pezzo Numero: %d,ruotato di %d\n",solution->matrice_pezzi[i][j][0],solution->matrice_pezzi[i][j][1]);
                rot_first=DESTRA-(solution->matrice_pezzi[i][j][1]);
                if (rot_first<0)
                    rot_first=4+rot_first;
                rot_sec=SINISTRA-(solution->matrice_pezzi[i][j+1][1]);
               // printf("A --> Pezzo Numero: %d,ruotato di %d\n",solution->matrice_pezzi[i][j+1][0],solution->matrice_pezzi[i][j+1][1]);
                if (rot_sec<0)
                    rot_sec=4+rot_sec;
                a = pieces[solution->matrice_pezzi[i][j][0]][rot_first];
                b = pieces[solution->matrice_pezzi[i][j+1][0]][rot_sec];
/*
                printf("A --> Pezzo Numero: %d,ruotato di %d, A vale %d\n",solution->matrice_pezzi[i][j][0],rot_first,a);
                printf("B --> Pezzo Numero: %d,ruotato di %d, B vale %d\n",solution->matrice_pezzi[i][j+1][0],rot_sec,b);
*/
                if (a == b)
                        profit++;
/*
               if (a == GRAY || b == GRAY)
                    printf("Sto confrontando colori di cui uno è GRAY: A--> %d\t B --> %d\n",a,b);
*/
            }
            // se è l'ultima riga non controlla il profit orizzontale
            if (i<(row-1)){
                rot_first=SOTTO-(solution->matrice_pezzi[i][j][1]);
                if (rot_first<0)
                    rot_first=4+rot_first;
                rot_sec=SOPRA-(solution->matrice_pezzi[i+1][j][1]);
                if (rot_sec<0)
                    rot_sec=4+rot_sec;
                a=pieces[solution->matrice_pezzi[i][j][0]][rot_first];
                b=pieces[solution->matrice_pezzi[i+1][j][0]][rot_sec];
                //printf("A --> Pezzo Numero: %d,ruotato di %d,A vale %d\n",solution->matrice_pezzi[i][j][0],rot_first,a);
                //printf("B --> Pezzo Numero: %d,ruotato di %d,B vale %d\n",solution->matrice_pezzi[i+1][j][0],rot_sec,b);
                if (a==b )
                        profit++;
/*
                if (a == GRAY || b == GRAY)
                    printf("Sto confrontando colori di cui uno è GRAY: A--> %d\t B --> %d\n",a,b);
*/
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

void get_right_border(int **pieces,solution_t *solution,char *taken,char *border_taken,char *border_pieces,int perimetro,int i,int j,int posizione){
    int random_rotation,pezzo,random_number,fine; 
    random_number = rand() % perimetro;
    fine=random_number;
     if (!border_taken[random_number]){
                   ++border_taken[random_number];
                   pezzo=border_pieces[random_number];
                   ++taken[pezzo];
                   //printf("Preso il pezzo numero %d come bordo %d\n",pezzo,posizione);
                   solution->matrice_pezzi[i][j][0]=(char)pezzo;;
                   random_rotation=get_border_fitting_rotation(pieces,pezzo,posizione);
                   solution->matrice_pezzi[i][j][1]=random_rotation;
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
                   pezzo=border_pieces[random_number];
                   ++taken[pezzo];
                   //printf("Preso il pezzo numero %d come bordo %d\n",pezzo,posizione);
                   solution->matrice_pezzi[i][j][0]=(char)pezzo;;
                   random_rotation=get_border_fitting_rotation(pieces,pezzo,posizione);
                   solution->matrice_pezzi[i][j][1]=random_rotation;
    }
    return;
}


char get_right_corner(int **pieces,char *corner_taken,char *corners){
    char random_number;
    random_number= rand() % 4;
    while (corner_taken[random_number]){
        random_number= (random_number+1);
        if (random_number >= 4)
            random_number=0;
    }
    ++corner_taken[random_number];
    return corners[random_number];
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
