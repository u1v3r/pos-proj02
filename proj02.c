#define _POSIX_C_SOURCE 199506L
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1

#define DEBUG
#define NO_TICKET -1

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>


int getticket(void);
void await(int aenter);
void advance(void);
void *thread_fnc(void *thread_id);
void random_wait(int thread_id);
void print_help(void);


struct thread_s{
    volatile int ticket;
    pthread_t thread_pt;
};

struct thread_s *thread_t;
int threads_count = 0;                       /* pocet vytvorenych vlakien */
int pass_count = 0;                          /* pocet prechodov kritickou sekciou (KS) */
volatile sig_atomic_t actual_ticket = 0;     /* obsahuje aktualny ticket */
volatile sig_atomic_t counter = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;


int main(int argc, char* argv[]){

    pthread_attr_t attr;
    int res,i;

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

    thread_t = (struct thread_s *)malloc(sizeof(struct thread_s) * threads_count);

    pthread_cond_init(&cond,NULL);

    for(i = 0; i < threads_count; i++){
        /* Vytvori vlakna a prem. i bude id vlakna */
        if((res = pthread_create(&thread_t[i].thread_pt,&attr,thread_fnc,(void *)i)) != 0){
            printf("pthread_create() error %d\n",res);
            return 1;
        }
    }

    if((res = pthread_attr_destroy(&attr)) != 0){
        printf("pthread_attr_destroy() error %d\n",res);
        return 1;
    }

    for (i = 0 ;  i < threads_count;  i++){
        if ((res = pthread_join(thread_t[i].thread_pt,NULL)) != 0){
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

    #ifdef DEBUG
        printf("vystupuje ticket %d\n",aenter);
    #endif
    pthread_mutex_unlock(&mutex);

    return;
}

void advance(void){

    #ifdef DEBUG
        printf("je v advance\n");
    #endif

    pthread_mutex_lock(&mutex);
    actual_ticket++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    return;
}

void *thread_fnc(void *thread_id){

    int ticket;
    int id = (int)thread_id;

    while ((ticket = getticket()) < pass_count){
        random_wait(id);
        /* kazde vlakno caka na vstup do KS */
        await(ticket);                                  /* Vstup do KS */
        printf("%d\t(%d)\n", ticket, id+1);               /* fflush(stdout); */
        advance();                                      /* Výstup z KS */
        random_wait(id);
    }


    return (void *)1;
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
