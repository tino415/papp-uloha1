/*
Meno:
Datum:

Simulujte nasledujucu situaciu. V pekarni pracuju pekari (10 pekarov), ktori pecu chlieb v peciach (4 pece). Pekar pripravuje chlieb nejaky cas (v simulacii 4s) a potom ide k volnej peci a pecie v nej chlieb (2s). Cela simulacia nech trva 30s.

1. Doplnte do programu pocitadlo pocitajuce, kolko chlebov bolo upecenych. [2b]

2. Zabezpecte, aby do obsadenej pece pekar vlozil chlieb az ked sa uvolni, cize aby poclal, kym nebude nejaka pec volna. Simulujte situaciu, ze ked pekar upecie 2 chleby, pocka na vsetkych kolegov a spravia si prestavku (v simulacii 4s). [5b]

3. Osetrite v programe spravne ukoncenie simulacie po uplynuti stanoveneho casu tak, aby pekar prerusil cinnost hned, ako je to mozne (ak uz zacal pripravu alebo pecenie moze ju dokoncit).  [3b]

Poznamky:
- na synchronizaciu pouzite iba mutexy, podmienene premenne alebo semafory
- nespoliehajte sa na uvedene casy, simulacia by mala fungovat aj s inymi casmi
- build (console): gcc pekari.c -o pekari -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define POCET_PECI 4
#define POCET_CHLEBOV_NA_PRESTAVKU 2
#define POCET_PEKAROV 10

// signal na zastavenie simulacie
int stoj = 0;

int pocet_chlebov;
int pocet_pekarov = 0;
pthread_mutex_t mutex_pocet_chlebov = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t mutex_pocitadlo_pekarov = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t mutex_turniket = PTHREAD_MUTEX_INITIALIZER; 
sem_t sem_pece;
sem_t sem_pekari;

// pekar
void priprava(int id) {
	printf("Pekar %d pripravuje chleba\n", id);
    sleep(4);
}

void pecenie(int id) {
	printf("Pekar %d pecie chleba\n", id);
	sem_wait(&sem_pece);
    sleep(2);
	sem_post(&sem_pece);

	pthread_mutex_lock(&mutex_pocet_chlebov);
		pocet_chlebov++;
	pthread_mutex_unlock(&mutex_pocet_chlebov);
}

void prestavka(int id) {
	printf("Pekar %d prestávkuje\n", id);
	sleep(4);
}

void *pekar( void *ptr ) {

	int id = (long) ptr;
	int pocet_chlebov = 0;
	int i;

    while(!stoj) {
        priprava(id);
		if(stoj) break;
        pecenie(id);
		if(stoj) break;
		pocet_chlebov++;
		if(pocet_chlebov % POCET_CHLEBOV_NA_PRESTAVKU == 0) {
			pthread_mutex_lock(&mutex_pocitadlo_pekarov);
			printf("%d ide na prestavku\n", id);
			if(stoj) break;
			if(pocet_pekarov == (POCET_PEKAROV - 1)) {
				pocet_pekarov = 0;
				printf("%d je posledny\n", id);
				for(i=0;i<(POCET_PEKAROV -1);i++) {
					sem_post(&sem_pekari);
					pthread_mutex_lock(&mutex_turniket);
				}
				pthread_mutex_unlock(&mutex_pocitadlo_pekarov);
			} else {
				printf("%d ide cakat na posledného\n", id);
				pocet_pekarov++;
				pthread_mutex_unlock(&mutex_pocitadlo_pekarov);
				sem_wait(&sem_pekari);
				pthread_mutex_unlock(&mutex_turniket);
			}
			if(stoj)break;
			prestavka(id);
		}
    }
    return NULL;
}

int main(void) {
    long i;

    pthread_t pekari[10];
	sem_init(&sem_pece, 0, POCET_PECI);
	sem_init(&sem_pekari, 0, 0);

    for (i=0;i<10;i++) pthread_create( &pekari[i], NULL, &pekar, (void*)i);

    sleep(30);
    stoj = 1;

    for (i=0;i<10;i++) pthread_join( pekari[i], NULL);

	printf("Upieklo sa %d chlebov\n", pocet_chlebov);

    exit(EXIT_SUCCESS);
}
