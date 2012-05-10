#include"evolution.h"
#include"popolation.h"
#include <math.h>
/*funzione per selezionare genitori da cui generare nuove soluzioni
 riceve il vettore dei parenti (ogni el è indice della sol nel vettore popolazione)*/
void parent_selection(long *parents,population_t *pop){
    char *chosen=(char *)malloc(sizeof(char)*pop->pop_dim);
    long i,tmp;//indice su pop e var temp per memorizzare il val estratto a caso
            
    /*inizializzazione flag*/
    for(i=0;i<pop->pop_dim;i++)
        chosen[i]=FALSE;
    /*sceglie i migliori come genitori (sceglie il 25% della popolazione)*/
    for(i=0;i<pop->elite;i++)
        parents[i]=-i-1;//salva (indice_negato+1) per controllare estraz gen
    /*estrae un altro 25% a caso*/
    for(;i<pop->gen_n;i++){
        tmp=rand()%(pop->pop_dim-pop->elite)+pop->elite;//tira a caso tra il 75% rimanente della pop
                                    //(esclude il 25% migliore gia selezionato)
        //se l'el. estratto e già stato scelto come genitore
        //prova con il successivo finchè non trova un el. non ancora sel.
        while(chosen[tmp])
            tmp=((tmp+1)%pop->pop_dim)+(pop->elite*(tmp/pop->pop_dim));//ultimo addendo per saltare
                                                    //quando ricomincia
        parents[i]=-(tmp+1);//segna come genitore in range [1-gen_n] (evita zero)
                            //perchè cambia segno in estraz gen se selez
        chosen[tmp]=TRUE;//segna come selezionato
    }
    free(chosen);
    return;
}
/*funzione che genera le nuove soluzioni
 ricecve il vettore dei geniori e quello cui salvare i figli*/
void offspring_generation(int **pieces,int npieces,population_t *pop,long *parents,solution_t *offspring,int row, int col){
    long i,j,tmp;//indice su pop e var temp per memorizzare il val estratto a caso
    long cnt;
    long gen[2];//vettore dei due indici dei genitori accoppiati da passare 
                //alla funzione di crossover
    /*Accoppiamento:
     seleziona 2 genitori a caso e li passa all funz di crossover per generare
     2 figli*/
    #pragma omp parallel default(none) shared(pieces,npieces,pop,parents,offspring,row,col) firstprivate(i,j,cnt,tmp,gen)
{
   #pragma omp for
    for(i=0;i<pop->gen_n;i+=2){
        j=i+1;
        /*ciclo di selezione genitori*/
       #pragma omp critical (print_tid)
      {
        for(cnt=0;cnt<2;cnt++){
                tmp=rand()%pop->gen_n;
                //se l'el. estratto e già stato accoppiato
                //prova con il successivo finchè non trova un el.da accoppiare.
     
                while(parents[tmp]>0)
                        tmp=((tmp+1)%pop->gen_n);
                gen[cnt]=tmp;
                parents[tmp]*=-1;
        }
      }
        //DEBUG
        //printf("gen:%ld %ld\n",gen[0],gen[1]);
        //offspring[i++].fitness=-1;
        //offspring[i++].fitness=-1;
        //END DEBUG
        offspring[i]=build_solution(row,col);
        offspring[j]=build_solution(row,col);
        crossover(&pop->soluzioni[parents[gen[0]]-1],&pop->soluzioni[parents[gen[1]]-1],&offspring[i],&offspring[j],pieces,npieces,row,col);
        offspring[i].fitness=fitness_solution_evaluation(pieces,&offspring[i],npieces,row,col);
        //printf("Valore fitness figlio %d: %d\n",i,offspring[i].fitness);
        offspring[j].fitness=fitness_solution_evaluation(pieces,&offspring[j],npieces,row,col);
        //printf("Valore fitness figlio %d : %d\n",i,offspring[i].fitness);
    }
    }

    return;
}
/*metodo sostituz 1 O(pop_dim)*/
void sub1(population_t *pop,solution_t *offspring,int row, int col){
    char *chosen=(char *)malloc(sizeof(char)*pop->pop_dim);//vettore di flag per tenere conto che la sol i è già 
                         //estratta (già sostituita
    long i,cnt;//i indice vettore soluz cnt indice di offspring in sostituzione(max val GEN_N-1))
    double  tmp_rnd,//tmp_rnd per estraz in sostituzione
            sum_inv_fit,//somma di 1/fitness per sol nella pop
            pi;// pi=(1/fitness)/sum_inv_fit prob con cui el è sostituito
    
    /*inizializzazione flag e calcolo somma inversi della fitness*/
    sum_inv_fit=0;
    for(i=0;i<pop->pop_dim;i++){
        chosen[i]=FALSE;
        sum_inv_fit+=(double)1/(double)pop->soluzioni[i].fitness;
    }
    //SOSTITUZIONE PUÒ ESSERE DA RIPENSARE IN OGNI CASO SOLO 1 DELLE 2 DA USARE!
    /*Sostituzione: tutti i figli sono inseriti, scegliendo a caso gli elementi
     da eliminare (di fatto ogni el. da eliminare sostituito con un figlio)
     Scorre vettore popolazione e per ogni el sostituito con probabilità
     p(i)=(1/fitness(i)).
     Se sostituito incrementa cnt che è anche indice su vet figli(figli inseriti
     in ordine che hanno in vet tanto poi si ordina in base a fitness).
     continua finchè ci sono figli da inserire, se necessario scorre + volte
     la popolazione incrementando con modulo su DIM_POP(se molte estraz hanno esito negativo può capitare che a fine
     di una scansione ci siano ancora figli da inserire)*/
    cnt=0;
    for(i=0;cnt<pop->gen_n;i=((i+1)%pop->pop_dim)){
        //vedi ex knapsack di perboli(metaheuristics-ga)
        tmp_rnd=(double)rand()/(double)RAND_MAX;//=(n_random/rand_max)*(1/sum_inv_fit)
        pi=((double)1/(double)pop->soluzioni[i].fitness);//=(1/fitness)*(1/sum_inv_fit)
        if((!chosen[i])&&(tmp_rnd<pi)){
            //rilascia la mem per la matrice dei pezzi della sol da sostiutuire
            dealloc_soluzioni(&pop->soluzioni[i],row,col);
            pop->soluzioni[i]=offspring[cnt++];
            chosen[i]=TRUE;
        }
    }
    free(chosen);
}
/*metodo sostituz 2 O(pop_dim^2)*/
void sub2(population_t *pop,solution_t *offspring,int row, int col){
    char *chosen=(char *)malloc(sizeof(char)*pop->pop_dim);//vettore di flag per tenere conto che la sol i è già 
                         //estratta (già sostituita
    long i,cnt;//i indice vettore soluz cnt indice di offspring in sostituzione(max val GEN_N-1))
    double  tmp_rnd,//tmp_rnd per estraz in sostituzione
            sum_inv_fit,//somma di 1/fitness per sol nella pop
            pi;// pi=(1/fitness)/sum_inv_fit prob con cui el è sostituito
    double *distribution=(double *)malloc(sizeof(double)*pop->pop_dim);;//contiene distrib di probabilità della pop per decidere che el sostituire 
    
    /*inizializzazione flag e calcolo somma inversi della fitness*/
    sum_inv_fit=0;
    for(i=0;i<pop->pop_dim;i++){
        chosen[i]=FALSE;
        sum_inv_fit+=(double)1/(double)pop->soluzioni[i].fitness;
    }
    distribution[0]=(double)1/(double)pop->soluzioni[0].fitness/sum_inv_fit;
    for(i=1;i<pop->pop_dim;i++){
        distribution[i]=distribution[i-1]+((double)1/(double)pop->soluzioni[i].fitness)/sum_inv_fit;//=(1/fitness)*(1/sum_inv_fit)
    }
    //per ogni figlio estrae un numero casuale e trova l'indice per la min pi tale che numero estratto è minore
    //e sostituisce il corrispondente elemento(el con quell indice)
    for(cnt=0;cnt<pop->gen_n;cnt++){
        tmp_rnd=(double)rand()/(double)RAND_MAX;//estrae num casuale tra 0 e 1
        for(i=0;(distribution[i]<tmp_rnd)&&(i<pop->pop_dim);i++);//trova el da sostituire
        while(chosen[i])
            i=(i+1)%pop->pop_dim;
        //rilascia la mem per la matrice dei pezzi della sol da sostiutuire
        dealloc_soluzioni(&pop->soluzioni[i],row,col);
        pop->soluzioni[i]=offspring[cnt];
        chosen[i]=TRUE;
    }
    free(chosen);
    free(distribution);
}
/*funzione per sostituire vecchie sol con i figli
 riceve vet pop vet figli, row e col (dim matrice gioco)*/
void substitution(population_t *pop,solution_t *offspring,int row, int col){
    sub2(pop,offspring,row,col);
}

/*funzione mutazione
 */
void mutation(int **pieces,int npieces,population_t *pop,int row, int col,int *border){
    long l;//indice per scorrere la matrice di soluzioni nella mutazione
    int caso;
    int ordine,starting;
    if (rand()%2){
    ordine=(int)log10(pop->pop_dim);
    starting=pow(10,ordine);
    }
    else
    {
       starting=100;
    }
    for(l=pop->pop_dim/starting;l<pop->pop_dim-1;l++){ // POP
                    random_solution_generation(&(pop->soluzioni[l]),border,pieces,npieces,row,col);   
                    pop->soluzioni[l].fitness=fitness_solution_evaluation(pieces,&(pop->soluzioni[l]),npieces,row,col);          
                }
    ++pop->mutation;
    //fprintf(stderr,"C'è stata una mutazione");
}

/*
void light_mutation(int **pieces,int npieces,population_t *pop,int row, int col,int *border){
    long l;//indice per scorrere la matrice di soluzioni nella mutazione 
    for(l=pop->pop_dim/10;l<pop->pop_dim-1;l++){ // POP
        random_rotate(&(pop->soluzioni[l]),row,col);
                }
}
*/

void light_mutation(int **pieces,int npieces,population_t *pop,int row, int col,int *border){
    long i;//i indice vettore soluz cnt indice di offspring in sostituzione(max val GEN_N-1))
    double  tmp_rnd,//tmp_rnd per estraz in sostituzione
            sum_inv_fit,//somma di 1/fitness per sol nella pop
            pi;
    
    //elite=POP_DIM/6;
/*
    sum_inv_fit=0;
    for(i=0;i<pop->pop_dim;i++){
        sum_inv_fit+=(double)1/(double)pop->soluzioni[i].fitness;
    }
*/
    for(i=0;i<pop->pop_dim;i++){
        //vedi ex knapsack di perboli(metaheuristics-ga)
        tmp_rnd=(double)rand()/(double)RAND_MAX;//=(n_random/rand_max)*(1/sum_inv_fit)
        pi=((double)1/(double)pop->soluzioni[i].fitness);//sum_inv_fit;//=(1/fitness)*(1/sum_inv_fit)
        if(tmp_rnd<pi){
            //rilascia la mem per la matrice dei pezzi della sol da sostiutuire
            random_rotate(&(pop->soluzioni[i]),row,col);
        }
    }
}
