
#include "popolation.h"
#include"evolution.h"

/*Funzione che crea la popolazione
 *  
 */

population_t *build_population(int **pieces,int *border,int npieces,int row,int col){
    int i,fit;
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
        //printf("Allocazione soluzione numero %d\n",i);
        random_solution_generation(&popolazione_start->soluzioni[i],border,pieces,row*col,row,col);
        //test_solution(&popolazione_start->soluzioni[i],row,col);
        popolazione_start->soluzioni[i].fitness=fitness_solution_evaluation(pieces,&popolazione_start->soluzioni[i],npieces,row,col);
    }
    popolazione_start->current_iteration=0;
    //printf("N° soluzioni feasible: %d\n", nfeasible);
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
        totale+=pop->soluzioni[i].fitness;
       // printf("Soluzione %d --> %d\n",i,pop->soluzioni[i].fitness);
    }
    media =(float) totale / POP_DIM;
    for(i=0;i<POP_DIM;i++){
        varianza += pow((pop->soluzioni[i].fitness - media),2); 
    }
    varianza = varianza / (POP_DIM-1);
/*
    printf("Media %f\n",media);
    printf("Varianza %f\n",varianza);
    printf("MAX  --> %d\n", pop->soluzioni[idmax].fitness);
*/
    pop->bests[pop->current_iteration][MAX]=max;
    pop->bests[pop->current_iteration][MEDIA]=media;
    pop->bests[pop->current_iteration][VARIANZA]=varianza;  
    //printf("MAX  --> %f\n", pop->bests[pop->current_iteration][MAX]);
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

void dealloc_population(population_t *pop,int row,int col){
    dealloc_soluzioni(pop->soluzioni,row,col);
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
     if(get_best(pop)>=MAX_PT) {
         fprintf(stderr,"ERRORE: La fitness della soluzione massima è maggiore di quella MASSIMA ammissibile\n");
         exit(2);
     }
     if(get_best(pop)==MAX_PT) return(TRUE);
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
int pop_evolution(int **pieces,int npieces,population_t *pop,int row, int col,int *border){
    long parents[GEN_N];//vettore dell soluzioni scelte come parenti tra cui
                             //estrarre le coppie di genitori per crossover
                             //la soluzione è memorizzata come indice nel vettore
                             //popolazione dichiarato come puntatore generico per
                             //avere di sicuro dimensione di 4 byte (e poter 
                             //rappresentare tutti gli indici senza overflow)
                             //E' usata nche per tenere conto dell'estarazione 
                             //per la scelta degli accoppiamenti se val positivo
                             //allora già estratto
    solution_t offspring[GEN_N];//vettore dei figli
    
   //printf("Entro in Pop Evolution\n");
    parent_selection(parents);
    /*DEBUG
    for(i=0;i<GEN_N;printf("parent%ld:%ld\n",i,parents[i++]));*/
    accoppiamento(pieces,npieces,pop,parents,offspring,row,col);
    substitution(pop,offspring,row,col);
    //DEBUG
    //test_fitness(pop);
    sorted_popolation(pop,pieces);
    //MUTATION triggerata solo quando per tre generazioni la varianza è nulla
    if (pop->current_iteration>GEN_N/5){
        //if(abs(pop->bests[pop->current_iteration-10][MAX]-pop->bests[pop->current_iteration][MAX])<10){
        if (pop->bests[pop->current_iteration][VARIANZA] < 1 && pop->bests[(pop->current_iteration)-1][VARIANZA] < 1 && pop->bests[(pop->current_iteration)-2][VARIANZA] < 1 ){
            mutation(pieces,npieces,pop,row,col,border);
            sorted_popolation(pop,pieces);
            fprintf(stderr,"mutazione\n");
        }
    }
    return(get_best(pop));
}
/*realizza crossover. riceve coppia genitori i puntatori ai figli e le dimensioni della soluz(cioè num pezzi) e della matrice(num righe e num col) 
 *si limita a preparare le strutture di appoggio usate e a chiamare le funzioni di crossover_bordo e crossover_centro*/
void crossover(solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2,int**pieces, int npieces, int row, int col){
    int i;
    // confronto pezzi dentro il kernel, kernelPieces serve a tenere traccia di quali pezzi
    // sono presenti dentro il kernel dei figli e devono essere rimpiazzati.
    // L'allocazione dei due figli è parallelizzata
    int **kernelPieces;
    //char pezzoDaControllare;->dovrebbe essere inutile
    //allocazione vettori per il confronto ottimizzato del kernel
    //pezzi di bordo in realtà mai usati quindi alcuni el del vet mai usati
    kernelPieces=(int  **)malloc(sizeof(int*)*npieces);
    if(kernelPieces==NULL){
        fprintf(stderr,"crossover()-errore in malloc() di kernelPieces.\n");
        exit(2);
    }
   for(i=0;i<npieces;i++){
        kernelPieces[i]=(int *)malloc(sizeof(int)*2);
        if(kernelPieces[i]==NULL){
                fprintf(stderr,"crossover()-errore in malloc() di kernelPieces[%d].\n",i);
                exit(2);
        }
        kernelPieces[i][0]=-1;
        kernelPieces[i][1]=-1;
    }
    crossover_centro(kernelPieces,sol1,sol2,fig1,fig2,npieces,row,col);
    for(i=0;i<npieces;i++){
        kernelPieces[i][0]=-1;
        kernelPieces[i][1]=-1;
    }
    crossover_bordo(kernelPieces,sol1,sol2,fig1,fig2,pieces,npieces,row,col);
    //test_solution(fig1,row,col);
    //test_solution(fig2,row,col);
    
    for(i=0;i<npieces;i++){
        free(kernelPieces[i]);
    }
    free(kernelPieces);
    //printf("Esco da Crossover\n");
    return;
}

/*funzione per il crossover tra i pezzi di bordo.
 * riceve 2 soluzioni(genitori) e genera 2 figli
 * bordo linearizzato in senso orario a partire da pos 0,0, cioè:
 * riga0,coln,rigan,col0 conisiderando una matrice n*n.*/
void crossover_bordo(int **kernelPieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2,int**pieces, int npieces, int row, int col){
    // generazione tagli, contatori e indice righe/colonne
    int taglio1,taglio2,i,j,c1,r1,perimetro,nval,tmp;
    int ker_len_min;//lunghezza minima kernel
    const int BOTTOM=row-1;
    const int RIGHT=col-1;
   //printf("Entro in Crossover bordo\n");
    perimetro=(row-1+col-1)*2;
    ker_len_min=(char)perimetro/10;//10% num pezzi(approx. all'intero inferiore)
    taglio1=rand()%(perimetro-ker_len_min-1)+1;
    if((nval=perimetro-taglio1-ker_len_min))
        taglio2=rand()%nval+taglio1+ker_len_min;//se taglio1<max val possibile taglio2 èrandom,cioè lunghezza kernel è casuale
    else
        taglio2=taglio1+ker_len_min;//se taglio1 è val max possibile taglio2 è vincolato da kerlenmin
    //DEBUG
    //printf("t1=%d,t2=%d,kerlenmin=%d\n",taglio1,taglio2,ker_len_min);
    /*generazione kernel
     *i indice del bordo linearizzato
     *j indice degli elementi di bordo nella matrice*/
    //trova su che lato del bordo inizia il kernel
    
    //NB sfrutta il fatto che la matrice è quadrata col=row => BOTTOM=RIGHT
    //1^riga
    j=taglio1;
    for(i=taglio1;(i<taglio2)&&(j<RIGHT);i++,j++){
        //figlio1
        fig1->matrice_pezzi[0][j][0]=sol1->matrice_pezzi[0][j][0];
        fig1->matrice_pezzi[0][j][1]=sol1->matrice_pezzi[0][j][1];
        kernelPieces[sol1->matrice_pezzi[0][j][0]][0]=j;//linearizzo indici row col poslin=0*row+j=j
        // figlio 2
        fig2->matrice_pezzi[0][j][0]=sol2->matrice_pezzi[0][j][0];
        fig2->matrice_pezzi[0][j][1]=sol2->matrice_pezzi[0][j][1];
        kernelPieces[sol2->matrice_pezzi[0][j][0]][1]=j;//linearizzo indici row col poslin=0*col+j=j
    }
    //ultima col a dx
    if(i>taglio1)
        j=0;//col dx appartiene al kernel(si è a dx di taglio1 considerando bordo lin)
    else
        j-=RIGHT;//altrimenti taglio1 ancora da trovare quindi se in col dx pos di taglio1-row-1=taglio1-BOTTOM per saltare el non del kernel
    for(;(i<taglio2)&&(j<BOTTOM);i++,j++){
        //figlio1
        fig1->matrice_pezzi[j][RIGHT][0]=sol1->matrice_pezzi[j][RIGHT][0];
        fig1->matrice_pezzi[j][RIGHT][1]=sol1->matrice_pezzi[j][RIGHT][1];
        kernelPieces[sol1->matrice_pezzi[j][RIGHT][0]][0]=j*col+RIGHT;//linearizzo indici row col
        // figlio 2
        fig2->matrice_pezzi[j][RIGHT][0]=sol2->matrice_pezzi[j][RIGHT][0];
        fig2->matrice_pezzi[j][RIGHT][1]=sol2->matrice_pezzi[j][RIGHT][1];
        kernelPieces[sol2->matrice_pezzi[j][RIGHT][0]][1]=j*col+RIGHT;//linearizzo indici row col
    }
    //ultima riga
    //se taglio1 era in col dx e continua qua j già settato da ciclo prima j=BOTTOM=RIGHT
    if(i==taglio1){
        //se invece ancora da trovare taglio1(cicli precedenti non eseguiti)
        j-=BOTTOM;//j punta a taglio1 in lato corrente considerando pos RIGHT come partenza(cioè come pos0)
        j=RIGHT-j;//per trovare pos reale fare il complemento di j su right.
    }
    for(;(i<taglio2)&&(j>0);i++,j--){
        //figlio1
        fig1->matrice_pezzi[BOTTOM][j][0]=sol1->matrice_pezzi[BOTTOM][j][0];
        fig1->matrice_pezzi[BOTTOM][j][1]=sol1->matrice_pezzi[BOTTOM][j][1];
        kernelPieces[sol1->matrice_pezzi[BOTTOM][j][0]][0]=BOTTOM*col+j;//linearizzo indici row col
        // figlio 2
        fig2->matrice_pezzi[BOTTOM][j][0]=sol2->matrice_pezzi[BOTTOM][j][0];
        fig2->matrice_pezzi[BOTTOM][j][1]=sol2->matrice_pezzi[BOTTOM][j][1];
        kernelPieces[sol2->matrice_pezzi[BOTTOM][j][0]][1]=BOTTOM*col+j;//linearizzo indici row col
    }
    //1^col sx
    if(i>taglio1)
        j=BOTTOM;//taglio1 superato e ultima riga appartiene al kernel(si è a dx di taglio1 considerando bordo lin)
    else{
        j=taglio1-3*RIGHT;//trova pos di taglio1 nel lato corrente considerando pos RIGHT come partenza(cioè come pos0)
        j=BOTTOM-j;//complemento per trovare pos reale
    }
    for(;(i<taglio2)&&(j>0);i++,j--){
        //figlio1
        fig1->matrice_pezzi[j][0][0]=sol1->matrice_pezzi[j][0][0];
        fig1->matrice_pezzi[j][0][1]=sol1->matrice_pezzi[j][0][1];
        kernelPieces[sol1->matrice_pezzi[j][0][0]][0]=j*col;//linearizzo indici row col
        // figlio 2
        fig2->matrice_pezzi[j][0][0]=sol2->matrice_pezzi[j][0][0];
        fig2->matrice_pezzi[j][0][1]=sol2->matrice_pezzi[j][0][1];
        kernelPieces[sol2->matrice_pezzi[j][0][0]][1]=j*col;//linearizzo indici row col
    }
    //DEBUG
    //test_solution(fig1,row,col);
    //test_solution(fig2,row,col);
    /*lato sinistro prole*/
    for(i=j=0;(i<taglio1)&&(j<RIGHT);i++,j++){
        if (kernelPieces[sol2->matrice_pezzi[0][j][0]][0]<0){
                fig1->matrice_pezzi[0][j][0]=sol2->matrice_pezzi[0][j][0];
                fig1->matrice_pezzi[0][j][1]=sol2->matrice_pezzi[0][j][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=0;
                c1=j;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[0][j][0]=sol2->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso.
                if(j==0)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig1->matrice_pezzi[0][j][1]=get_corner_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SINISTRA,SOPRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig1->matrice_pezzi[0][j][1]=get_border_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SOPRA);
            }
            if (kernelPieces[sol1->matrice_pezzi[0][j][0]][1]<0){
                fig2->matrice_pezzi[0][j][0]=sol1->matrice_pezzi[0][j][0];
                fig2->matrice_pezzi[0][j][1]=sol1->matrice_pezzi[0][j][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=0;
                c1=j;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[0][j][0]=sol1->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==0)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig2->matrice_pezzi[0][j][1]=get_corner_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SINISTRA,SOPRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig2->matrice_pezzi[0][j][1]=get_border_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SOPRA);
            }
    }
    //ultima col dx
    for(j=0;(i<taglio1)&&(j<BOTTOM);i++,j++){
        if (kernelPieces[sol2->matrice_pezzi[j][RIGHT][0]][0]<0){
                fig1->matrice_pezzi[j][RIGHT][0]=sol2->matrice_pezzi[j][RIGHT][0];
                fig1->matrice_pezzi[j][RIGHT][1]=sol2->matrice_pezzi[j][RIGHT][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=j;
                c1=RIGHT;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[j][RIGHT][0]=sol2->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==0)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig1->matrice_pezzi[j][RIGHT][1]=get_corner_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SOPRA,DESTRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig1->matrice_pezzi[j][RIGHT][1]=get_border_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],DESTRA);
            }
            if (kernelPieces[sol1->matrice_pezzi[j][RIGHT][0]][1]<0){
                fig2->matrice_pezzi[j][RIGHT][0]=sol1->matrice_pezzi[j][RIGHT][0];
                fig2->matrice_pezzi[j][RIGHT][1]=sol1->matrice_pezzi[j][RIGHT][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=j;
                c1=RIGHT;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[j][RIGHT][0]=sol1->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==0)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig2->matrice_pezzi[j][RIGHT][1]=get_corner_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SOPRA,DESTRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig2->matrice_pezzi[j][RIGHT][1]=get_border_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],DESTRA);
            }
    }
    //ultima riga in basso
    for(;(i<taglio1)&&(j>0);i++,j--){
        if (kernelPieces[sol2->matrice_pezzi[BOTTOM][j][0]][0]<0){
                fig1->matrice_pezzi[BOTTOM][j][0]=sol2->matrice_pezzi[BOTTOM][j][0];
                fig1->matrice_pezzi[BOTTOM][j][1]=sol2->matrice_pezzi[BOTTOM][j][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=BOTTOM;
                c1=j;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[BOTTOM][j][0]=sol2->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==RIGHT)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig1->matrice_pezzi[BOTTOM][j][1]=get_corner_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],DESTRA,SOTTO);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig1->matrice_pezzi[BOTTOM][j][1]=get_border_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SOTTO);
            }
            if (kernelPieces[sol1->matrice_pezzi[BOTTOM][j][0]][1]<0){
                fig2->matrice_pezzi[BOTTOM][j][0]=sol1->matrice_pezzi[BOTTOM][j][0];
                fig2->matrice_pezzi[BOTTOM][j][1]=sol1->matrice_pezzi[BOTTOM][j][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=BOTTOM;
                c1=j;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[BOTTOM][j][0]=sol1->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==RIGHT)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig2->matrice_pezzi[BOTTOM][j][1]=get_corner_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],DESTRA,SOTTO);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig2->matrice_pezzi[BOTTOM][j][1]=get_border_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SOTTO);
            }
    }
    //1^col sx
    for(j=BOTTOM;(i<taglio1)&&(j>0);i++,j--){
        if (kernelPieces[sol2->matrice_pezzi[j][0][0]][0]<0){
                fig1->matrice_pezzi[j][0][0]=sol2->matrice_pezzi[j][0][0];
                fig1->matrice_pezzi[j][0][1]=sol2->matrice_pezzi[j][0][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=j;
                c1=0;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[j][0][0]=sol2->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==BOTTOM)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig1->matrice_pezzi[j][0][1]=get_corner_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SOTTO,SINISTRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig1->matrice_pezzi[j][0][1]=get_border_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SINISTRA);
            }
            if (kernelPieces[sol1->matrice_pezzi[j][0][0]][1]<0){
                fig2->matrice_pezzi[j][0][0]=sol1->matrice_pezzi[j][0][0];
                fig2->matrice_pezzi[j][0][1]=sol1->matrice_pezzi[j][0][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //figlio1
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=j;
                c1=0;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[j][0][0]=sol1->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==BOTTOM)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig2->matrice_pezzi[j][0][1]=get_corner_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SOTTO,SINISTRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig2->matrice_pezzi[j][0][1]=get_border_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SINISTRA);
            }
    }
    //DEBUG
    //test_solution(fig1,row,col);
    //test_solution(fig2,row,col);
    /*lato destro prole*/
    for(i=j=taglio2;j<RIGHT;i++,j++){
        if (kernelPieces[sol2->matrice_pezzi[0][j][0]][0]<0){
                fig1->matrice_pezzi[0][j][0]=sol2->matrice_pezzi[0][j][0];
                fig1->matrice_pezzi[0][j][1]=sol2->matrice_pezzi[0][j][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=0;
                c1=j;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[0][j][0]=sol2->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso.
                //trova rotaz per bordo
                //L'angolo in questo caso non appartiene mai al lato dx prole (caso limite taglio2=kerlenmin)
                //trova rotaz per bordo
                fig1->matrice_pezzi[0][j][1]=get_border_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SOPRA);
            }
            if (kernelPieces[sol1->matrice_pezzi[0][j][0]][1]<0){
                fig2->matrice_pezzi[0][j][0]=sol1->matrice_pezzi[0][j][0];
                fig2->matrice_pezzi[0][j][1]=sol1->matrice_pezzi[0][j][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=0;
                c1=j;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[0][j][0]=sol1->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso.
                //L'angolo in questo caso non appartiene mai al lato dx prole (caso limite taglio2=kerlenmin)
                //trova rotaz per bordo
                fig2->matrice_pezzi[0][j][1]=get_border_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SOPRA);
            }
    }
    //ultima col a dx
    if(i>taglio2)
        j=0;//col dx appartiene al kernel(si è a dx di taglio1 considerando bordo lin)
    else
        j-=RIGHT;//altrimenti taglio1 ancora da trovare quindi se in col dx pos di taglio1-row-1=taglio1-BOTTOM per saltare el non del kernel
    for(;j<BOTTOM;i++,j++){
        if (kernelPieces[sol2->matrice_pezzi[j][RIGHT][0]][0]<0){
                fig1->matrice_pezzi[j][RIGHT][0]=sol2->matrice_pezzi[j][RIGHT][0];
                fig1->matrice_pezzi[j][RIGHT][1]=sol2->matrice_pezzi[j][RIGHT][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=j;
                c1=RIGHT;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[j][RIGHT][0]=sol2->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==0)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig1->matrice_pezzi[j][RIGHT][1]=get_corner_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SOPRA,DESTRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig1->matrice_pezzi[j][RIGHT][1]=get_border_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],DESTRA);
            }
            if (kernelPieces[sol1->matrice_pezzi[j][RIGHT][0]][1]<0){
                fig2->matrice_pezzi[j][RIGHT][0]=sol1->matrice_pezzi[j][RIGHT][0];
                fig2->matrice_pezzi[j][RIGHT][1]=sol1->matrice_pezzi[j][RIGHT][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=j;
                c1=RIGHT;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[j][RIGHT][0]=sol1->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==0)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig2->matrice_pezzi[j][RIGHT][1]=get_corner_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SOPRA,DESTRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig2->matrice_pezzi[j][RIGHT][1]=get_border_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],DESTRA);
            }
    }
    //ultima riga
    //se taglio1 era in col dx e continua qua j già settato da ciclo prima j=BOTTOM=RIGHT
    if(i==taglio2){
        //se invece ancora da trovare taglio1(cicli precedenti non eseguiti)
        j-=BOTTOM;//j punta a taglio1 in lato corrente considerando pos RIGHT come partenza(cioè come pos0)
        j=RIGHT-j;//per trovare pos reale fare il complemento di j su right.
    }
    for(;j>0;i++,j--){
        if (kernelPieces[sol2->matrice_pezzi[BOTTOM][j][0]][0]<0){
                fig1->matrice_pezzi[BOTTOM][j][0]=sol2->matrice_pezzi[BOTTOM][j][0];
                fig1->matrice_pezzi[BOTTOM][j][1]=sol2->matrice_pezzi[BOTTOM][j][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=BOTTOM;
                c1=j;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[BOTTOM][j][0]=sol2->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==RIGHT)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig1->matrice_pezzi[BOTTOM][j][1]=get_corner_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],DESTRA,SOTTO);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig1->matrice_pezzi[BOTTOM][j][1]=get_border_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SOTTO);
            }
            if (kernelPieces[sol1->matrice_pezzi[BOTTOM][j][0]][1]<0){
                fig2->matrice_pezzi[BOTTOM][j][0]=sol1->matrice_pezzi[BOTTOM][j][0];
                fig2->matrice_pezzi[BOTTOM][j][1]=sol1->matrice_pezzi[BOTTOM][j][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=BOTTOM;
                c1=j;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[BOTTOM][j][0]=sol1->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==RIGHT)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig2->matrice_pezzi[BOTTOM][j][1]=get_corner_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],DESTRA,SOTTO);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig2->matrice_pezzi[BOTTOM][j][1]=get_border_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SOTTO);
            }
    }
    //1^col sx
     if(i>taglio2)
        j=BOTTOM;//taglio1 superato e ultima riga appartiene al kernel(si è a dx di taglio1 considerando bordo lin)
    else{
        j=taglio2-3*RIGHT;//trova pos di taglio1 nel lato corrente considerando pos RIGHT come partenza(cioè come pos0)
        j=BOTTOM-j;//complemento per trovare pos reale
    }
    for(;j>0;i++,j--){
       if (kernelPieces[sol2->matrice_pezzi[j][0][0]][0]<0){
                fig1->matrice_pezzi[j][0][0]=sol2->matrice_pezzi[j][0][0];
                fig1->matrice_pezzi[j][0][1]=sol2->matrice_pezzi[j][0][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=j;
                c1=0;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[j][0][0]=sol2->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==BOTTOM)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig1->matrice_pezzi[j][0][1]=get_corner_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SOTTO,SINISTRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig1->matrice_pezzi[j][0][1]=get_border_fitting_rotation(pieces,sol2->matrice_pezzi[r1][c1][0],SINISTRA);
            }
            if (kernelPieces[sol1->matrice_pezzi[j][0][0]][1]<0){
                fig2->matrice_pezzi[j][0][0]=sol1->matrice_pezzi[j][0][0];
                fig2->matrice_pezzi[j][0][1]=sol1->matrice_pezzi[j][0][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //figlio1
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=j;
                c1=0;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[j][0][0]=sol1->matrice_pezzi[r1][c1][0];
                //rotazione in questo caso non si può copiare perchè pos diversa
                //e potrebbe essere su lato diverso (o angolo diverso se pezzo è di angolo)
                 if(j==BOTTOM)//unico angolo è quello in alto a sin (alto a dx considerato in col a dx)
                    //trova giusta rotaz per l'angolo
                    fig2->matrice_pezzi[j][0][1]=get_corner_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SOTTO,SINISTRA);
                else//altrimenti è sul lato
                    //trova rotaz per bordo
                    fig2->matrice_pezzi[j][0][1]=get_border_fitting_rotation(pieces,sol1->matrice_pezzi[r1][c1][0],SINISTRA);
            }
    }
    //printf("Esco da Crossover bordo\n");
    return;
}

/*funzione per il crossover tra i pezzi della matrice che non sono di bordo 
 * ie indice_riga € [1,row-2] e indice_col€[1,col-2]*/
void crossover_centro(int **kernelPieces,solution_t *sol1, solution_t *sol2, solution_t *fig1,solution_t *fig2, int npieces, int row, int col){
    // generazione tagli, contatori e indice righe/colonne
    int taglio1,taglio2,i,r,c,c1,r1,nval,tmp,q;
    int ker_len_min;//lunghezza minima kernel
    //solution_t *tmp_ptr1,*tmp_ptr2,*tmp_ptr_swap;
    //printf("Entro in Crossover centro\n");
    ker_len_min=(char)npieces/10;//10% num pezzi(approx. all'intero inferiore) conta anche bordo anche se lavora su centro
    //col+2 fa si che min(taglio1) è 3° el 2^riga per evitare bordo
    //(evita 1^riga e almeno un el prima del taglio) e avere almeno un el prima del taglio
    taglio1=rand()%(npieces-(col+1)-(col+2)-ker_len_min)+(col+2);
    while((taglio1%col==0)||(taglio1%col)==(col-1))
        ++taglio1;
    //col+1=numero di pezzi nell'ultima riga + l'ultimo pezzo della penultima
    //cioè il bordo destro della penultima riga e il bordo inferiore
    //npieces-taglio1+1=num el rimanenti
    //taglio1+1 per essere>taglio1
    //kerlen min=min num el in kernel
    if((nval=npieces-taglio1-ker_len_min-(col+2)))
        taglio2=rand()%nval+taglio1+ker_len_min;//se taglio1<max val possibile taglio2 èrandom,cioè lunghezza kernel è casuale
    else
        taglio2=taglio1+ker_len_min;//se taglio1 è val max possibile taglio2 è vincolato da kerlenmin
    //se kernel su + righe i pezzi di bordo(ultimo di una riga e primo della 
    //successiva) contano nella distanza tra i tagli quindi per garantire min 
    //aumenta taglio2 del n° pezzi inclusi (2*ogni salto di riga)
    //taglio2+=2*(taglio2-taglio1+1)/col;
    //DEBUG
    //printf("t1=%d,t2=%d,kerlenmin=%d\n",taglio1,taglio2,ker_len_min);
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
    c=taglio1%col;
    //controlli per saltare i bordi se taglio è su essi
    if(c==0)//bordo sin-> salta pezzo (controllo bordo dx nel for)
        ++c;
     i=r*col+c;
    for(;(c<(col-1))&&(i<taglio2);c++,i++){
        fig1->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
        fig1->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r][c][1];
        kernelPieces[sol1->matrice_pezzi[r][c][0]][0]=(char)i;
        // figlio 2
        fig2->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
        fig2->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r][c][1];
        kernelPieces[sol2->matrice_pezzi[r][c][0]][1]=(char)i;
    }
    //se kernel su + righe continua a scorrere matrice interna fino a taglio2
    i+=2;//considera su matrice linearizzata le posizioni saltate quando finisce
         //di scorrere el interni(3 el ma i già incremantata una volta alla fine
         //del for per la 1^riga)
    r++;
    for(;(r<(row-1))&&(i<taglio2);r++,i+=2){
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
    //DEBUG
    //test_solution(fig1,row,col);
    //test_solution(fig2,row,col);
    /*Generazione lato sinistro della prole*/
    i=col+1;//i=r*col+c,r=c=1
    for(r=1;(r<(row-1))&&(i<taglio1);r++,i+=2){
        for(c=1;(c<(col-1))&&(i<taglio1);c++,i++){
            if (kernelPieces[sol2->matrice_pezzi[r][c][0]][0]<0){
                fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
                fig1->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r][c][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=r;
                c1=c;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    //se r1=r e c=c1 punta all'el che non è stato messo che ha val =
                    //a quello già nel kernel
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
                fig1->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r1][c1][1];
            }
            if (kernelPieces[sol1->matrice_pezzi[r][c][0]][1]<0){
                fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
                fig2->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r][c][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //figlio1
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=r;
                c1=c;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
                fig2->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r1][c1][1];
            }
        }      
    }
    /*Generazione lato detro della prole*/
    //prima riga su cui operare a parte come per kernel
    //npieces-col limita ciclo al penultimo el della peunltima riga (evita bordo
    //inferiore e el di bordo destro sull'ultima riga interna)
    r=taglio2/col;
    c=taglio2%col;
    //controlli per saltare i bordi se taglio è su essi
    if(c==0)//bordo sin-> salta pezzo
        ++c;
     i=r*col+c;
    for(;(c<(col-1))&&(i<(npieces-col));c++,i++){
       if (kernelPieces[sol2->matrice_pezzi[r][c][0]][0]<0){
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
            fig1->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r][c][1];
        }
        // se il pezzo è già presente nel kernel
        else {
             //figlio1
            r1=r;
            c1=c;
            //controlla che quell'elemento non sia nel kernel
            do{
                //ottieni la posizione in cui è stato messo per prendere 
                //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                r1 = tmp/col;
                c1 = tmp % col;
                //nelle prossime iteraz controlla che il pezzo sostituito
                //non sia nel kernel.se si ripete il tutto finche non trova una
                //sostituzione con un pezzo non nel kernel              
            }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
            //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
            fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
            fig1->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r1][c1][1];
        }
        if (kernelPieces[sol1->matrice_pezzi[r][c][0]][1]<0){
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
            fig2->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r][c][1];
        }
        // se il pezzo è già presente nel kernel
        else {
            //figlio1
            //inizialmente punta all'elemento che si vuole copiare ma c'è già
            //cioè punta all'el non del kernel di sol1
            r1=r;
            c1=c;
            //controlla che quell'elemento non sia nel kernel
            do{
                //ottieni la posizione in cui è stato messo per prendere 
                //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                r1 = tmp/col;
                c1 = tmp % col;
                //nelle prossime iteraz controlla che il pezzo sostituito
                //non sia nel kernel.se si ripete il tutto finche non trova una
                //sostituzione con un pezzo non nel kernel
            }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
            //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
            fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
            fig2->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r1][c1][1];
        }
    }
    r++;
    i+=2;
    //npieces-col limita ciclo al penultimo el della peunltima riga (evita bordo
    //inferiore e el di bordo destro sull'ultima riga interna)
    for(;(r<(row-1))&&(i<(npieces-col));r++,i+=2){
        for(c=1;(c<(col-1))&&(npieces-col);c++,i++){
            if (kernelPieces[sol2->matrice_pezzi[r][c][0]][0]<0){
                fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r][c][0];
                fig1->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r][c][1];
            }
            //se il pezzo è già presente nel kernel
            else {
                 //figlio1
                r1=r;
                c1=c;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //printf("%d\n",kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]);
/*
                    for(q=0;q<npieces;q++)
                        printf("%d --> %d\n",q,kernelPieces[q][0]);
                    //nelle prossime iteraz controlla che il pezzo sostituito
*/
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol2->matrice_pezzi[r1][c1][0]][0]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig1->matrice_pezzi[r][c][0]=sol2->matrice_pezzi[r1][c1][0];
                fig1->matrice_pezzi[r][c][1]=sol2->matrice_pezzi[r1][c1][1];
            }
            if (kernelPieces[sol1->matrice_pezzi[r][c][0]][1]<0){
                fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r][c][0];
                fig2->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r][c][1];
            }
            // se il pezzo è già presente nel kernel
            else {
                //figlio1
                //inizialmente punta all'elemento che si vuole copiare ma c'è già
                //cioè punta all'el non del kernel di sol1
                r1=r;
                c1=c;
                //controlla che quell'elemento non sia nel kernel
                do{
                    //ottieni la posizione in cui è stato messo per prendere 
                    //l'elemento dell'altro genitore nella stessa posiz(applica il PMX)
                    tmp=kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1];
                    r1 = tmp/col;
                    c1 = tmp % col;
                    //nelle prossime iteraz controlla che il pezzo sostituito
                    //non sia nel kernel.se si ripete il tutto finche non trova una
                    //sostituzione con un pezzo non nel kernel
                }while(kernelPieces[sol1->matrice_pezzi[r1][c1][0]][1]>0);
                //se era già presente copia quello sostituito altrimenti sol2->mat_pez[r][c]
                fig2->matrice_pezzi[r][c][0]=sol1->matrice_pezzi[r1][c1][0];
                fig2->matrice_pezzi[r][c][1]=sol1->matrice_pezzi[r1][c1][1];
            }
        }
    }
    //printf("Esco da Crossover centro\n");
    return;
}

void write_best_solution(char *nomefile,solution_t sol,int row,int col) {
    int i,j; // contatori nel ciclo 
    FILE *fp;//puntatore al file  di pezzi
     if ((fp=fopen(nomefile,"w")) == NULL){
                 fprintf(stderr,"build_population()-errore in malloc() di kernelPieces.\n");
                 exit(2);
         }
     for(i=0;i<row;i++)
         for(j=0;j<col;j++)
                fprintf(fp,"%d %d \n",sol.matrice_pezzi[i][j][0],sol.matrice_pezzi[i][j][1]);
     fclose(fp);
}

/* Funzione per visualizzare l'andamento dell'evoluzione durante l'esecuzione del software*/

void test_evolution(population_t *pop,solution_t *best){
    test_fitness(pop);
    printf("-----------------------------------------------------------------\n");;
    printf("-----------------------------------------------------------------\n");
    printf("Evoluzione: Generazione %d\n", pop->current_iteration );
    printf("Dimensione popolazione: %d\n", POP_DIM);
    printf("Media Popolazione: %f \t Varianza Popolazione: %f \n",pop->bests[pop->current_iteration][MEDIA],pop->bests[pop->current_iteration][VARIANZA]);
    printf("Miglior Soluzione Corrente punti: %d\n",(int)pop->bests[pop->current_iteration][MAX]);
    printf("Miglior Soluzione Corrente punti: %d\n",(int)best->fitness);
    if (pop->current_iteration  > 0){
                printf("-----------------------------------------------------------------\n");
                printf("Evoluzione parametri %d --> %d\n", pop->current_iteration-1, pop->current_iteration );
                printf("Variazione Media Popolazione: %f \t Variazione Varianza Popolazione: %f \n",(pop->bests[pop->current_iteration][MEDIA]-pop->bests[pop->current_iteration-1][MEDIA]),(pop->bests[pop->current_iteration][VARIANZA]-pop->bests[pop->current_iteration-1][VARIANZA]));
                printf("Variazione Miglior Soluzione punti: %d\n",(int)pop->bests[pop->current_iteration][MAX]-(int)pop->bests[pop->current_iteration-1][MAX]);
    }
    ++pop->current_iteration;
}

/* Funzione per l'analisi dei dati ottenuti alla fine dell'esecuzione, il file di output verrà scritto utilizzando la codifica CSV*/

void write_evolution(population_t *pop,char *nomefile){
       int i,j; // contatori nel ciclo 
       FILE *fp;//puntatore al file  di pezzi
      if ((fp=fopen(nomefile,"w")) == NULL){
                 fprintf(stderr,"Errore nell'apertura del file %s\n",nomefile);
                 exit(2);
         }
       fprintf(fp,"GENERAZIONE,MAX,MEDIA,VARIANZA\n",nomefile);
       for(i=0;i<pop->current_iteration;i++){
            fprintf(fp,"%d,",i);
         for(j=0;j<N_MISURE;j++)
                fprintf(fp,"%d,",(int)pop->bests[i][j]); 
         fprintf(fp,"\n");
       }
       fclose(fp);
       return;
}
