#define POP_DIM 1000
#define GEN_N (POP_DIM/2+(POP_DIM%2))//numero genitori è metà della popolazione
                                   //deve essere pari percui se è dispari somma 1
#define ELITE (POP_DIM/4)//numero di migliori tra i genitori (è pari)
//#define CASUALI (GEN_N-ELITE)//genitori da scegliere a caso (sol 0-elite)
#define RANGE_CAS (POP_DIM-ELITE)//numero di valori tra cui estrarre gli el casuali
                               //evitando di estrarre le sol tra 0 ed elite 

#include "popolation.h"

/*Funzione che crea la popolazione
 *  
 */

population_t *build_population(int **pieces,int *border,int npieces,int row,int col){
    int i,fit,nfeasible=0;
    population_t *popolazione_start;
    popolazione_start=(population_t *)malloc(sizeof(population_t));
    if(popolazione_start==NULL){
        fprintf(stderr,"build_population()-errore in malloc() di popolazione_start.\n");
        exit(2);
    }
    popolazione_start->soluzioni=(solution_t *)malloc(sizeof(solution_t)*POP_DIM);
    if(popolazione_start->soluzioni==NULL){
        fprintf(stderr,"build_population()-errore in malloc() di popolazione_start->soluzioni.\n");
        exit(2);
    }
    for(i=0;i<POP_DIM;i++){
        popolazione_start->soluzioni[i]=build_solution(row,col);
        //genera una popolazione di soluzioni casuali
        random_solution_generation(&popolazione_start->soluzioni[i],border,pieces,row*col,row,col);
        //test_solution(&popolazione_start->soluzioni[i],row,col);
        fit=fitness_solution_evaluation(pieces,&popolazione_start->soluzioni[i],npieces,row,col);
        if (popolazione_start->soluzioni[i].feasible==FEASIBLE)
            nfeasible++;
        popolazione_start->soluzioni[i].fitness=fit+1;
    }
    printf("N° soluzioni feasible: %d\n", nfeasible);
    return popolazione_start;
}

void test_fitness(population_t *pop){
    int i,max=0,idmax=0;
    float media,varianza,totale;
    //printf("Qual è la miglior soluzione?\n");
    for(i=0;i<POP_DIM;i++){
        if ((pop->soluzioni[i].fitness)>max){
            max=pop->soluzioni[i].fitness;
            idmax=i;
        }
        printf("Soluzione %d:\t %d \n",i,pop->soluzioni[i].fitness);
    }
    media =(float) totale / POP_DIM;
    for(i=0;i<POP_DIM;i++){
    varianza += pow((pop->soluzioni[i].fitness - media),2); 
    }
    varianza = varianza / (POP_DIM-1);
    printf("Dimensione popolazione: %d\n", POP_DIM);
    printf("Media Popolazione: %f \t Varianza Popolazione: %f \n",media,varianza);
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

/*Funzione che regola l'evoluzione di una popolazione, definendo quali individui 
 *  devono riprodursi e quali devono essere mantenuti in vita
 */

/*funzione che restituisce la soluzione della popolaz con fitness + alta
 siccome pop ordinata per ordine decrescente di fitness ritorna quella del 1°el*/
int get_best(population_t* pop){
    return(pop->soluzioni[0].fitness);
}
/*ritorna vero se la pop contienen una sol ottima altrimenti falso*/
int is_best(population_t* pop,int row,int col){
     const int MAX_PT=(row-1)*col+(col-1)*row;//costante di punti max dipende 
                                               //da dimensione matr del gioco
                                               //1° addendo collegamenti oriz.
                                               //2°verticali (colleg=coppia di
                                               //pezzi con colori uguali su giuntura)
     if(get_best(pop)==MAX_PT)return(TRUE);
     return(FALSE);
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
    long parents[GEN_N];//vettore dell soluzioni scelte come parenti tra cui
                             //estrarre le coppie di genitori per crossover
                             //la soluzione è memorizzata come indice nel vettore
                             //popolazione dichiarato come puntatore generico per
                             //avere di sicuro dimensione di 4 byte (e poter 
                             //rappresentare tutti gli indici senza overflow)
                             //E' usata nche per tenere conto dell'estarazione 
                             //per la scelta degli accoppiamenti se val positivo
                             //allora già estratto
    long i,tmp;//indice su pop e var temp per memorizzare il val estratto a caso
    solution_t offspring[GEN_N];//vettore dei figli
    int cnt,//per generare coppia e distinguere genitori
        curr_best;//per normalizzare probabilità in base a miglior sol
                  //evitando che nelle prime iteraz se fitnessmedia è bassa ci
                  //sia troppo facile venir sostituiti
    long gen[2];//vettore dei due indici dei genitori accoppiati da passare 
                   //alla funzione di crossover
    double  tmp_rnd,//tmp_rnd per estraz in sostituzione
            sum_inv_fit,//somma di 1/fitness per sol nella pop
            pi;// pi=(1/fitness)/sum_inv_fit prob con cui el è sostituito
   
    /*inizializzazione flag*/
    for(i=0;i<POP_DIM;i++)
        chosen[i]=FALSE;
    /*sceglie i migliori come genitori (sceglie il 25% della popolazione)*/
    for(i=0;i<ELITE;i++)
        parents[i]=-i-1;//salva (indice_negato+1) per controllare estraz gen
    /*estrae un altro 25% a caso*/
    for(;i<GEN_N;i++){
        tmp=rand()%(RANGE_CAS)+ELITE;//tira a caso tra il 75% rimanente della pop
                                    //(esclude il 25% migliore gia selezionato)
        //se l'el. estratto e già stato scelto come genitore
        //prova con il successivo finchè non trova un el. non ancora sel.
        while(chosen[tmp])
            tmp=((tmp+1)%POP_DIM)+(ELITE*(tmp/POP_DIM));//ultimo addendo per saltare
                                                    //quando ricomincia
        parents[i]=-(tmp+1);//segna come genitore in range [1-gen_n] (evita zero)
                            //perchè cambia segno in estraz gen se selez
        chosen[tmp]=TRUE;//segna come selezionato
    }
    /*DEBUG
    for(i=0;i<GEN_N;printf("parent%ld:%ld\n",i,parents[i++]));*/
    /*Accoppiamento:
     seleziona 2 genitori a caso e li passa all funz di crossover per generare
     2 figli*/
    for(i=0;i<GEN_N;){
        /*ciclo di selezione genitori*/
        for(cnt=0;cnt<2;cnt++){
                tmp=rand()%GEN_N;
                //se l'el. estratto e già stato accoppiato
                //prova con il successivo finchè non trova un el.da accoppiare.
                while(parents[tmp]>0)
                        tmp=((tmp+1)%GEN_N);
                gen[cnt]=tmp;
                parents[tmp]*=-1;
        }
        //DEBUG
        //printf("gen:%ld %ld\n",gen[0],gen[1]);
        offspring[i++].fitness=-1;
        offspring[i++].fitness=-1;
        //END DEBUG
        crossover(&pop->soluzioni[parents[gen[0]]-1],&pop->soluzioni[parents[gen[1]]-1],&offspring[i],&offspring[i+1],npieces,row,col);
        fitness_solution_evaluation(pieces,&offspring[i++],npieces,row,col);
        fitness_solution_evaluation(pieces,&offspring[i++],npieces,row,col);
    }
    /*inizializzazione flag e calcolo somma inversi della fitness*/
    sum_inv_fit=0;
    for(i=0;i<POP_DIM;i++){
        chosen[i]=FALSE;
        sum_inv_fit+=(double)1/(double)pop->soluzioni[i].fitness;
    }
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
    for(i=0;cnt<GEN_N;i=((i+1)%POP_DIM)){
        tmp_rnd=(double)rand()/(double)RAND_MAX/sum_inv_fit;
        pi=((double)1/(double)pop->soluzioni[i].fitness)/sum_inv_fit;
        if((!chosen[i])&&(tmp_rnd<pi))
            pop->soluzioni[i]=offspring[cnt++];
    }
    //DEBUG
    //test_fitness(pop);
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
    
    for(i=0;i<GEN_N;i++){
         tmp=rand()%POP_DIM;
         //usa il long per genitore anche se non ha nulla a che vedere
         //(evita nuova var tanto solo per prova)
         tmp_rnd=(double)rand()/(double)RAND_MAX;
         pi=((double)1/(double)pop->soluzioni[i].fitness)/sum_inv_fit;
         if(tmp_rnd<pi]){
                //se già sostituito prova col successivo finchè non trova uno vecchio el
                while(chosen[tmp])
                         tmp=(tmp+1)%POP_DIM;
                //sostituisce in ogni caso(non considera fitness el corrente)
                pop->soluzioni[tmp]=offspring[i];
         }     
    }*/
    //sorted_popolation(pop,pieces);
    if(is_best(pop,row,col)){
        return(OPT_SOL);
    }
    return(EVOLVI_ANCORA);
}

void crossover(solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2, int npieces, int row, int col){
    int i;
    *fig1=build_solution(row,col);
    *fig2=build_solution(row,col);
    // confronto pezzi dentro il kernel, kernelPieces serve a tenere traccia di quali pezzi
    // sono presenti dentro il kernel dei figli e devono essere rimpiazzati.
    // L'allocazione dei due figli è parallelizzata
    char **kernelPieces;
    //char pezzoDaControllare;->dovrebbe essere inutile
    //allocazione vettori per il confronto ottimizzato del kernel
    //pezzi di bordo in realtà mai usati quindi alcuni el del vet mai usati
    kernelPieces=(char **)malloc(sizeof(char*)*npieces);
    if(kernelPieces==NULL){
        fprintf(stderr,"build_population()-errore in malloc() di kernelPieces.\n");
        exit(2);
    }
   for(i=0;i<npieces;i++){
        kernelPieces[i]=(char *)malloc(sizeof(char)*2);
        if(kernelPieces[i]==NULL){
                fprintf(stderr,"crossover_centro()-errore in malloc() di kernelPieces[%d].\n",i);
                exit(2);
        }
        kernelPieces[i][0]=-1;
        kernelPieces[i][1]=-1;
    }
    crossover_centro(kernelPieces,sol1,sol2,fig1,fig2,npieces,row,col);
    
    
    
    /* // generazione tagli, contatori e indice righe/colonne
    int taglio1,taglio2,i,r,c,c1,r1,j,flag;
    // confronto pezzi dentro il kernel, kernelPieces serve a tenere traccia di quali pezzi
    // sono presenti dentro il kernel dei figli e devono essere rimpiazzati.
    // L'allocazione dei due figli è parallelizzata
    char pezzoDaControllare,**kernelPieces;
    //allocazione vettori per il confronto ottimizzato del kernel
    kernelPieces=(char **)malloc(sizeof(char*)*npieces);
    for(i=0;i<npieces;i++){
        kernelPieces[i]=(char *)malloc(sizeof(char)*2);
        kernelPieces[i][0]=-1;
        kernelPieces[i][1]=-1;
    }
    taglio1=rand() % npieces/2 +1 ;
    taglio2=rand() % npieces/2 + npieces/2;
    if (taglio2=taglio1)
        taglio2=taglio2 + npieces/10;
    *fig1=build_solution(pieces,row,col);
    *fig2=build_solution(pieces,row,col);
    // Generazione kernel della prole
    for(i=taglio1;i<taglio2;i++){
        r = i/col;
        c = i % col;
        // figlio 1
        fig1->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        fig1->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r][c][1];
        kernelPieces[sol1->matrice_pezzi[r][c][0]][0]=i;
        // figlio 2
        fig2->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        fig2->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r][c][1];
        kernelPieces[sol2->matrice_pezzi[r][c][0]][1]=i;
    }
    // Generazione lato sinistro della prole
    for(i=0;i<taglio1;i++){
        r = i/col;
        c = i % col;
        flag=0;
        j=taglio1;
        // se il pezzo non è già presente nel kernel
        if (kernelPieces[sol1->matrice_pezzi[r][c][0]][0]<0)
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol1->matrice_pezzi[r][c][0]][0];
            r1 = j/col;
            c1 = j % col;
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
        if (kernelPieces[sol2->matrice_pezzi[r][c][0]][1]<0)
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol2->matrice_pezzi[r][c][0]][1];
            r1 = j/col;
            c1 = j % col;
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
        }
        }
    }
    //Generazione lato destro della prole
    for(i=taglio2;i<npieces;i++){
        r = i/col;
        c = i % col;
        flag=0;
        j=taglio1;
        // se il pezzo non è già presente nel kernel
        if (kernelPieces[sol1->matrice_pezzi[r][c][0]][0]<0)
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol1->matrice_pezzi[r][c][0]][0];
            r1 = j/col;
            c1 = j % col;
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
        if (kernelPieces[sol2->matrice_pezzi[r][c][0]][1]<0)
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol2->matrice_pezzi[r][c][0]][1];
            r1 = j/col;
            c1 = j % col;
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
        }
        }
    }
 }controllare parentesi(in particolare ultime graffe) se si rimette sto codice!*/
}

/*funzione per il crossover tr i pezzi della matrice che non sono di bordo 
 * ie indice_riga € [1,row-2] e indice_col€[1,col-2]*/
void crossover_centro(char **kernelPieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2, int npieces, int row, int col){
    // generazione tagli, contatori e indice righe/colonne
    int taglio1,taglio2,i,r,c,c1,r1,j;
    char ker_len_min;//lunghezza minima kernel
    
    ker_len_min=(char)npieces/10;//10% num pezzi(approx. all'intero inferiore) conta anche bordo anche se lavora su centro
    //col+2 fa si che min(taglio1) è 3° el 2^riga per evitare bordo
    //(evita 1^riga e almeno un el prima del taglio) e avere almeno un el prima del taglio
    taglio1=rand()%npieces+(col+2);
    //col+1=numero di pezzi nell'ultima riga + l'ultimo pezzo della penultima
    //cioè il bordo destro della penultima riga e il bordo inferiore
    //npieces-taglio1+1=num el rimanenti
    //taglio1+1 per essere>taglio1
    //kerlen min=min num el in kernel
    taglio2=rand()%(npieces-(taglio1+1)-(col+1))+taglio1+ker_len_min;
    //così vincoli il kernel sempre a cavallo della metà.
    /*taglio1=rand() % npieces/2 +(col+1);
    taglio2=rand() % (npieces/2-(col+1)) + npieces/2;
    if (taglio2=taglio1)
        taglio2=taglio2 + npieces/10;*/
    
    /*Generazione kernel della prole
     i tagli operano su matrice linearizzata:i indice della mat linearizzata
     r e c sono gli indici della mat delle sol in realtà scorrono solo sulla parte
     interna, evitando bordi:r€[1,col-2] e c€[1,row-2]*/
    //prima riga su cui operare (che può essere non completa,cioè c>1) è gestita a parte
    r=taglio1/col;
    i=r*col;
    for(c=taglio1%col;(c<(col-1))&&(i<taglio2);c++,i++){
        fig1->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        fig1->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r][c][1];
        kernelPieces[sol1->matrice_pezzi[r][c][0]][0]=i;
        // figlio 2
        fig2->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        fig2->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r][c][1];
        kernelPieces[sol2->matrice_pezzi[r][c][0]][1]=i;
    }
    //se kernel su + righe continua a scorrere matrice interna fino a taglio2
    i+=3;//considera su matrice linearizzata le posizioni saltate quando finisce
         //di scorrere el interni
    r++;
    for(;(r<(row-1))&&(i<taglio2);r++,i+=3){
        for(c=1;(c<(col-1))&&(i<taglio2);c++,i++){
            // figlio 1
            fig1->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
            fig1->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r][c][1];
            kernelPieces[sol1->matrice_pezzi[r][c][0]][0]=i;
            // figlio 2
            fig2->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
            fig2->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r][c][1];
            kernelPieces[sol2->matrice_pezzi[r][c][0]][1]=i;
        }
    }
    
    /*Generazione lato sinistro della prole*/
    for(i=r=1;(r<(row-1))&&(i<taglio1);r++,i+=3){
        for(c=1;(c<(col-1))&&(i<taglio1);c++,i++){
                // se il pezzo non è già presente nel kernel
            if (kernelPieces[sol1->matrice_pezzi[r][c][0]][0]<0)
                fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
            // se il pezzo è già presente nel kernel
            else {
                j=kernelPieces[sol1->matrice_pezzi[r][c][0]][0];
                r1 = j/col;
                c1 = j % col;
                fig1->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
            }
            if (kernelPieces[sol2->matrice_pezzi[r][c][0]][1]<0)
                fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
            // se il pezzo è già presente nel kernel
            else {
                j=kernelPieces[sol2->matrice_pezzi[r][c][0]][1];
                r1 = j/col;
                c1 = j % col;
                fig2->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
            }
        }
    }
    /*Generazione lato detro della prole*/
    r=taglio2/col;
    i=r*col;
    //prima riga su cui operare a parte come per kernel
    //npieces-col limita ciclo al penultimo el della peunltima riga (evita bordo
    //inferiore e el di bordo destro sull'ultima riga interna)
    for(c=taglio2%col;(c<(col-1))&&(i<(npieces-col));c++,i++){
       // se il pezzo non è già presente nel kernel
        if (kernelPieces[sol1->matrice_pezzi[r][c][0]][0]<0)
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol1->matrice_pezzi[r][c][0]][0];
            r1 = j/col;
            c1 = j % col;
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
        }
        if (kernelPieces[sol2->matrice_pezzi[r][c][0]][1]<0)
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        // se il pezzo è già presente nel kernel
        else {
            j=kernelPieces[sol2->matrice_pezzi[r][c][0]][1];
            r1 = j/col;
            c1 = j % col;
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
        }
    }
    r++;
    i+=3;
    //npieces-col limita ciclo al penultimo el della peunltima riga (evita bordo
    //inferiore e el di bordo destro sull'ultima riga interna)
    for(;(r<(row-1))&&(i<(npieces-col));r++,i+=3){
        for(c=1;(c<(col-1))&&(i<taglio1);c++,i++){
                // se il pezzo non è già presente nel kernel
            if (kernelPieces[sol1->matrice_pezzi[r][c][0]][0]<0)
                fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
            // se il pezzo è già presente nel kernel
            else {
                j=kernelPieces[sol1->matrice_pezzi[r][c][0]][0];
                r1 = j/col;
                c1 = j % col;
                fig1->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
            }
            if (kernelPieces[sol2->matrice_pezzi[r][c][0]][1]<0)
                fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
            // se il pezzo è già presente nel kernel
            else {
                j=kernelPieces[sol2->matrice_pezzi[r][c][0]][1];
                r1 = j/col;
                c1 = j % col;
                fig2->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
            }
        }
    }
}

void write_best_solution(char *nomefile,population_t *pop,int row,int col) {
    int i,j; // contatori nel ciclo 
    FILE *fp;//puntatore al file  di pezzi
     fp=fopen(nomefile,"w");
     for(i=0;i<row;i++)
         for(j=0;j<col;j++)
                fprintf(fp,"%d %d \n",pop->soluzioni[0].matrice_pezzi[i][j][0],pop->soluzioni[0].matrice_pezzi[i][j][1]);

}