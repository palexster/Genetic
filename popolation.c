#define POP_DIM 1000
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "pieces.h"
#include "popolation.h"
//boolean val
enum{FALSE,TRUE};

/*Funzione che crea la popolazione*/
population_t *build_population(int **pieces,int npieces,int row,int col){
    int i,fit;
    population_t *popolazione_start;
    popolazione_start=(population_t *)malloc(sizeof(population_t));
    popolazione_start->soluzioni=(solution_t *)malloc(sizeof(solution_t)*POP_DIM);
    for(i=0;i<POP_DIM;i++){
        popolazione_start->soluzioni[i]=build_solution(pieces,row,col);
        //genera una popolazione di soluzioni casuali
        random_solution_generation(&popolazione_start->soluzioni[i],pieces,row*col,row,col);
        //test_solution(&popolazione_start->soluzioni[i],row,col);
        fit=fitness_solution_evaluation(pieces,&popolazione_start->soluzioni[i],npieces,row,col);
        popolazione_start->soluzioni[i].fitness=fit;
    }
    return popolazione_start;
}

void test_fitness(population_t *pop){
    int i,max=0,idmax=0;
    //printf("Qual è la miglior soluzione?\n");
    for(i=0;i<POP_DIM;i++){
        if ((pop->soluzioni[i].fitness)>max){
            max=pop->soluzioni[i].fitness;
            idmax=i;
        }
        printf("Soluzione %d:\t %d \n",i,pop->soluzioni[i].fitness);
    }
    printf("Miglior Soluzione n° %d , punti: %d\n",idmax,pop->soluzioni[idmax].fitness);
}

int cmp_fitness(solution_t s1cast,solution_t s2cast){
    if (s1cast.fitness>s2cast.fitness)
        return -1;
    if (s1cast.fitness<s2cast.fitness)
        return 1;
    return 0;
}

void sorted_popolation(population_t *pop,int **pieces){
  quick_sort(pop->soluzioni,0,POP_DIM-1,cmp_fitness);  
}

void quick_sort(solution_t *array, int l, int r, int (*cmp)(solution_t lv, solution_t rv))
{
   int i, j;
   solution_t pivot, tmp;

   if (l >= r) return;

   pivot = array[l];
   i = l - 1; j = r + 1;
   while (i < j) {


      while (cmp(array[++i], pivot) < 0);
      while (cmp(array[--j], pivot) > 0);

      if (i < j) {
         tmp = array[i];
         array[i] = array[j];
         array[j] = tmp;
      }
   }
   quick_sort(array, l, j, cmp);
   quick_sort(array, j+1, r, cmp);
}

void dealloc_population(population_t *pop,int row){
    dealloc_soluzioni(pop->soluzioni,row);
    free(pop->soluzioni);
    //free(pop);
    return;
}
/*funzione che restituisce la soluzione della popolaz con fitness + alta
 siccome pop ordinata per ordine decrescente di fitness ritorna quella del 1°el*/
int get_best(population_t* pop){
    return(pop->soluzioni[0].fitness);
}
/*funzione che genera nuova popolazione a partire dalla corrente
 * input:popolazione corrente, e parametri per per funz crossover(vet pezzi,
 * npezzi,row,col).
 3 fasi:
 1)    Selezione genitori:n_el1=25% pop sono i migliori el (ie con fitness 
        + alta ovvero il primo 25% del vettore) n_el2=25% pop el scelti a caso
        in tot 50% pop scelto come genitore=> 50% pop di figli(ogni coppia di
        genitori fa 2 figli).
 2)    Accoppiamento & Crossover:estrae 2 el a caso tra i genitori (così 
        ammesse coppie migliore-migliore, caso-caso, e miste) e passa alla
        funz di crossover le 2 var per i figli(per referenza) e i 2 genitori
 3)    Sostituzione:la dim della pop rimane costante e tutti i figli vengono
        inseriti, quindi ogni generazione un 50% degli elementi muore ed è
        sostituito dai figli*/
int pop_evolution(int **pieces,int npieces,population_t *pop,int row, int col){
    char chosen[POP_DIM];//vettore di flag per tenere conto che la sol i è già 
                         //estratta (o scelta come genitore o scelta per essere
                         //come genitore)
    long parents[POP_DIM/2];//vettore dell soluzioni scelte come parenti tra cui
                             //estrarre le coppie di genitori per crossover
                             //la soluzione è memorizzata come indice nel vettore
                             //popolazione dichiarato come puntatore generico per
                             //avere di sicuro dimensione di 4 byte (e poter 
                             //rappresentare tutti gli indici senza overflow)
                             //E' usata nche per tenere conto dell'estarazione 
                             //per la scelta degli accoppiamenti se val negativo
                             //allora già estratto
    long i,tmp;//indice su pop e var temp per memorizzare il val estratto a caso
    solution_t offspring[POP_DIM/2];//vettore dei figli
    int cnt,//per generare coppia e distinguere genitori
        curr_best;//per normalizzare probabilità in base a miglior sol
                  //evitando che nelle prime iteraz se fitnessmedia è bassa ci
                  //sia troppo facile venir sostituiti
    long gen[2];//vettore dei due indici dei genitori accoppiati da passare 
                   //alla funzione di crossover
    float       tmp_rnd,//tmp_rnd per estraz in sostituzione
                pi;// pi=1/fitness prob con cui el è sostituito
    /*static*/int MAX_PT;//costante di punti max dipende da dimensione matr del
                         //gioco          
    MAX_PT=(row-1)*col+(col-1)*row;//1° addendo collegamenti oriz.
                                   //2°verticali (colleg=coppia di
                                   //pezzi con colori uguali su giuntura)
    /*inizializzazione flag*/
    for(i=0;i<POP_DIM;i++)
        chosen[i]=FALSE;
    /*sceglie i migliori come genitori (sceglie il 25% della popolazione)*/
    for(i=0;i<POP_DIM/4;i++)
        parents[i]=i;
    /*estrae un altro 25% a caso*/
    for(;i<POP_DIM/2;i++){
        tmp=rand()%(3*POP_DIM/4)+25;//tira a caso tra il 75% rimanente della pop
                                    //(esclude il 25% migliore gia selezionato)
        //se l'el. estratto e già stato scelto come genitore
        //prova con il successivo finchè non trova un el. non ancora sel.
        while(!chosen[tmp])
            tmp++;
        parents[i]=tmp;//segna come genitore
        chosen[tmp]=TRUE;//segna come selezionato
    }
    //DEBUG
    for(i=0;i<POP_DIM/2;i++,printf("parent%d:%d\n",i,parents[i]));
    /*Accoppiamento:
     seleziona 2 genitori a caso e li passa all funz di crossover per generare
     2 figli*/
    for(i=0;i<POP_DIM/2;){
        /*ciclo di selezione genitori*/
        for(cnt=0;cnt<2;cnt++){
                tmp=rand()%(POP_DIM/2);
                //se l'el. estratto e già stato accoppiato
                //prova con il successivo finchè non trova un el.da accoppiare.
                while(parents[tmp]<0)
                        tmp++;
                gen[cnt]=tmp;
        }
        //DEBUG
        printf("gen:%d %d\n",gen[0],gen[1]);
        offspring[i++].fitness=-1;
        offspring[i++].fitness=-1;
        //END DEBUG
        
        //crossover(pieces,&pop[gen[1]],&pop[gen[2]],&offspring[i++],&offspring[i++],npieces,row,col);
        //perchè passare a puntatore a sol se ritorni fitness?
        //da fare solo se non calcolata in crossover in caso migliorare gest indici offspring
        //offspring[i-2]fitness_solution_evaluation(pieces,&offspring[i-2],npieces,row,col);
        //offspring[i-1]fitness_solution_evaluation(pieces,&offspring[i-1],npieces,row,col);
    }
    /*inizializzazione flag*/
    for(i=0;i<POP_DIM;i++)
        chosen[i]=FALSE;
    /*Sostituzione: tutti i figli sono inseriti, scegliendo a caso gli elementi
     da eliminare (di fatto ogni el. da eliminare sostituito con un figlio)
     Scorre vettore popolazione e per ogni el sostituito con probabilità
     p(i)=(1/fitness(i)).
     Se sostituito incrementa cnt che è anche indice su vet figli(figli inseriti
     in ordine che hanno in vet tanto poi si ordina in base a fitness).
     continua finchè ci sono figli da inserire, se necessario scorre + volte
     la popolazione incrementando con modulo su DIM_POP(se molte estraz hanno esito negativo può capitare che a fine
     di una scansione ci siano ancora figli da inserire)*/
    curr_best=get_best(pop);//work in progress!->da vedere come usare
    cnt=0;
    for(i=0;cnt<POP_DIM/2;i=((i+1)%POP_DIM)){
        tmp_rnd=rand()%MAX_PT;
        pi=MAX_PT-pop->soluzioni[i].fitness;
        if((!chosen[i])&&(tmp_rnd<pi))
            pop->soluzioni[i]=offspring[cnt++];
    }
    //DEBUG
    test_fitness(pop);
    /*Varinte
     Anziché scorrere vecchia pop e sostituire el con p(i)=1/fitness(i)
     estrae un el a caso lo sostituisce con probabilità p(i)(se non risultato 
     estraz è non sostituire riestrai un altro el da sost con prob p(i)).
     Se già scelto muove al successivo finchè non trova uno da sostituire
     (chosen[i]/=false).
     Da qui 2 opzioni:sostituirlo in ogni caso al posto del precedente(in questo
     caso scelta dipendente da fitness di un altro el non di quella dell'el 
     corrente),oppure ritirare a sorte(rispetta criterio iniziale di scelta 
     dipendente da fitness dell'el ma + overhead perchè se estraz fallisce
     probing è stato inutile)
    
    for(i=0;i<POP_DIM/2;i++){
         tmp=rand()%POP_DIM;
         //usa il long per genitore anche se non ha nulla a che vedere
         //(evita nuova var tanto solo per prova)
         gen[0]=rand()%(float)(1/MAX_PT);
         if(gen[0]<pop->soluzioni[tmp]){
                //se già sostituito prova col sucessivo finchè non trova uno vecchio el
                while(!chosen[tmp])
                        tmp++;
                //sostituisce in ogni caso(non considera fitness el corrente)
                pop->soluzioni[tmp]=offspring[i];
         }     
    }*/
    //sorted_popolation(pop,pieces);
    if(get_best(pop)==MAX_PT){
        return(OPT_SOL);
    }
    return(EVOLVI_ANCORA);
}
