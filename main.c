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
#define SOGLIA_ESCALATION 20
#define MAXLENGTH 30

#define TRUE  1
#define FALSE 0

#ifdef _OPENMP
  #include <omp.h>
#else
  #define omp_get_thread_num() 0
#endif

int main(int argc, char** argv) {
    /* Dichiarazione variabili */
    #ifdef _OPENMP
   (void) omp_set_dynamic(FALSE);
   if (omp_get_dynamic()) {printf("Warning: dynamic adjustment of threads has been set\n");}
   (void) omp_set_num_threads(2);
#endif
    int row,col;//numero righe e colonne matrice dei pezzi
    int new_best, 
        debug=FALSE, // flag per attivare le stampe nella funzione di test 
        npieces,//numero pezzi
        *border,// vettore dei pezzi di bordo di npieces el.per ogni pezzo dice se è di bordo.val è pari al num di trinagoli grigi(0=centro,1=bordo,2=angolo)
            temp, // massimo di una generazione
            escalation, // contiene il numero di iterazioni non miglioranti consecutive
            record,
            max_pop_dim,
            i;
    int **pieces;//vettore dei colori del pezzo
    char file[MAXLENGTH]; // stringa dove comporre il nome del nuovo file di record
    solution_t best;//contiene migliore soluzione trovata
    population_t *population; // puntatore a popolazione
    FILE*fp;//scrivere file utili per i test per test
    
    srand(time(NULL)); // randomizzazione del generatore di numeri pseudocasuali
    if (argc<6 && argc >5){
        fprintf(stderr,"Usage: %s input_file output_file record max_dim_pop [--debug]",argv[0]);
        exit(2);
    } 
    if (argv[6] != NULL){
        if (argv[6] == "--debug")
            debug=TRUE;
    }
    max_pop_dim=atoi(argv[4]);
    record=atoi(argv[3]);
    pieces=build_pieces(argv[1],&border,&npieces,&row,&col);
    population=build_population(pieces,border,npieces,row,col);
    const int MAX_PT=(row-1)*col+(col-1)*row;//costante di punti max dipende 
    escalation=0;
    //printf("Punteggio Massimo: %d\n",MAX_PT);
    sorted_popolation(population,pieces);
    best.fitness=population->soluzioni[0].fitness;
    best.matrice_pezzi=matalloc(row,col);
    solution_copy(population->soluzioni[0],&best,row,col);
    test_evolution(population,&best,MAX_PT,debug);
    if(!(is_best(population,row,col))){
        while(population->pop_dim<=max_pop_dim)
        for(i=0;(i<MAX_ITERATIONS)&&(best.fitness!=MAX_PT);i++){
            temp=pop_evolution(pieces,npieces,population,row,col,border);
                if(temp>best.fitness){
                    best.fitness=population->soluzioni[0].fitness;
                    solution_copy(population->soluzioni[0],&best,row,col);
                    new_best=TRUE;
                    escalation=0;
                    population->mutation=0;
                    if (best.fitness>record){
                        //printf("NEW RECORD!\n");
                        sprintf(file,"Solution_%dx%d_%d.txt",row,col,best.fitness);
                        write_best_solution(file,best,row,col);
                    }
                }
                else {
                    escalation++;
                    if (population->mutation>SOGLIA_ESCALATION){
                         sprintf(file,"Stats_%dx%d_%ld.txt",row,col,population->pop_dim);
                        write_evolution(population,file);
                        expand_population(pieces,npieces,population,row,col,border);
                        population->current_iteration=0;
                        population->mutation=0;
                        escalation=0;
                        break;
                    }
                }
                test_evolution(population,&best,MAX_PT,debug);
                if (new_best && !debug){
                    printf("--------\n");
                        printf(" New best solution found: %d\n Population Dimension: %ld\n Iteration %d\n Average Population %.2f \n", best.fitness,population->pop_dim,population->current_iteration,population->bests[1][MEDIA]);
                        new_best=FALSE;
                }
       }
    }
    write_best_solution(argv[2],best,row,col);
    //write_evolution(population,argv[3]);
    //deallocazione memoria heap
    dealloc_population(population,row,col);
    dealloc_soluzioni(&best,row,col);
    free(population);
    dealloc_pieces(pieces,npieces);
    free(border);
    return best.fitness ;
}

