/*------------------------------------------------------------------------------





------------------------------------------------------------------------------*/

#define _POSIX_C_SOURCE 199506L
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1

/*#define DEBUG*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>


/** Prideli ticket vlaknu */
int getticket(void);

/** Vpusti len jedno vlakno s platnym ticketom */
void await(int aenter);

/** Inkrementuje aktualne cislo ticketu a odblokuje dalsie vlakno */
void advance(void);

/** Obsluzi vlakno */
void *thread_fnc(void *thread_id);

/** Caka 0 - 0.5 s */
void random_wait(int thread_id);

/** Na vystup vypise napovedu */
void print_help(void);


pthread_t *thread_t;                         /* pole vlakien */
int threads_count = 0;                       /* pocet vytvorenych vlakien */
int pass_count = 0;                          /* pocet prechodov kritickou sekciou (KS) */
volatile sig_atomic_t actual_ticket = 0;     /* obsahuje aktualny ticket */
volatile sig_atomic_t counter = 0;           /* vyuziva sa na vytvarnie ticketov */
pthread_mutex_t mutex;
pthread_cond_t cond;


int main(int argc, char* argv[]){

    pthread_attr_t attr;
    int res;
    intptr_t i; /* nech nevyhadzuje warning pri predavani do thread_fnc()*/

    /* spracovanie parametrov */
    if(argc == 3){
        threads_count = atoi(argv[1]);
        pass_count = atoi(argv[2]);

        /* nezaporne cislo */
        if(threads_count < 0 || pass_count < 0){
            fprintf(stderr,"invalid params\n");
            return 1;
        }
    }else {
        print_help();
        return 1;
    }


    /* Vytvorenie zadaneho poctu vlakien */
    if((res = pthread_attr_init(&attr)) != 0){
        printf("pthread_attr_init() error %d\n",res);
        return 1;
    }

    if((res = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE)) != 0){
        printf("pthread_attr_setdetachstate() error %d\n",res);
        return 1;
    }

    if((res = pthread_cond_init(&cond,NULL)) != 0){
        printf("pthread_cond_init() error %d\n",res);
        return 1;
    }

    /* alokacia pamate pre thready */
    thread_t = (pthread_t *)malloc(sizeof(pthread_t) * threads_count);

    for(i = 0; i < threads_count; i++){
        /* Vytvori vlakna a prem. i bude id vlakna */
        if((res = pthread_create(&thread_t[i],&attr,thread_fnc,(void *)i)) != 0){
            printf("pthread_create() error %d\n",res);
            return 1;
        }
    }

    /* atributy uz netreba */
    if((res = pthread_attr_destroy(&attr)) != 0){
        printf("pthread_attr_destroy() error %d\n",res);
        return 1;
    }

    for (i = 0 ;  i < threads_count;  i++){
        if ((res = pthread_join(thread_t[i],NULL)) != 0){
            printf("pthread_attr_init() err %d\n",res);
            return 1;
        }
    }


    return 0;
}

int getticket(void){

    int number;

    pthread_mutex_lock(&mutex);
    number = (int)counter++;
    pthread_mutex_unlock(&mutex);

    return number;
}

void await(int aenter){

    pthread_mutex_lock(&mutex);

    /* vstupuju vsetci */
    #ifdef DEBUG
        printf("vstupuje ticket %d\n",aenter);
    #endif

    /* pokracuje len ten koho cislo sa zhoduje s aktualnym ticketom */
    while(actual_ticket != aenter)
        pthread_cond_wait(&cond,&mutex);

    /* vystupuje len thread s platnym ticketom */
    #ifdef DEBUG
        printf("vystupuje ticket %d\n",aenter);
    #endif
    pthread_mutex_unlock(&mutex);

    return;
}

void advance(void){

    #ifdef DEBUG
        printf("thread je v advance\n");
    #endif

    pthread_mutex_lock(&mutex);
    actual_ticket++;                    /* inkrementuje aktualny ticket */
    pthread_cond_broadcast(&cond);      /* zobudi vsetky vlakna a tie si skontroluju ticket */
    pthread_mutex_unlock(&mutex);

    return;
}

void *thread_fnc(void *thread_id){

    int ticket;
    intptr_t id = (intptr_t)thread_id;

    while ((ticket = getticket()) < pass_count){
        random_wait(id);
        /* kazde vlakno caka na vstup do KS */
        await(ticket);                                  /* Vstup do KS */
        printf("%d\t(%d)\n", ticket, (int)id+1);             /* fflush(stdout); */
        advance();                                      /* Výstup z KS */
        random_wait(id);
    }

    return (void *) 1;
}

void random_wait(int thread_id){

    struct timespec ts;
    unsigned int seek = (thread_id + 1) * getpid();

    ts.tv_nsec = rand_r(&seek) % 500000000; /* 0 - 0.5 s */
    ts.tv_sec = 0;

    nanosleep(&ts,NULL);
}

void print_help(void){

    printf("Pouzitie: proj02 [pocet vlakien] [pocet prechodov]\n\n");
    printf("pocet vlakien   - pocet vytvaranych vlakien\n");
    printf("pocet prechodov - celkovy pocet prechodov kritickou sekciou\n\n");
    printf("Priklad: proj02 1024 100\n");

}
