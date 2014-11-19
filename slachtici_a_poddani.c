/*
Meno:
Datum:

Simulujte nasledujucu situaciu. V malom kralovstve korunovali noveho krala a chodia sa mu neustale klanat styria slachtici a desiati poddani. Prejavovanie ucty kralovi trva nejaky cas (v simulacii 1s) a nejaky cas si slahctic ci poddany dava prestavku (v simulacii 4s). Cela simulacia nech trva 30s.

1. Doplnte do programu pocitadlo pocitajuce, kolko krat sa kralovi poklonili slachtici; a pocitadlo pocitajuce, kolko krat sa kralovi poklonili poddani. [2b]

2. Zabezpecte, aby sa kralovi sucasne klanali maximalne dvaja slachtici a tiez aby sa kralovi neklanal slachtic spolu s poddanym (cize alebo max. 2 slachtici, alebo lubovolne vela poddanych). Ak je pred kralom rad, slachtici maju samozrejme prednost. [5b]

3. Osetrite v programe spravne ukoncenie simulacie po uplynuti stanoveneho casu. [3b]

Poznamky:
- na synchronizaciu pouzite iba mutexy, podmienene premenne alebo semafory
- nespoliehajte sa na uvedene casy, simulacia by mala fungovat aj s inymi casmi
- build (console): gcc poslovia_a_pisari -o poslovia_a_pisari -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_SLACHTICI 2

// signal na zastavenie simulacie
int stoj = 0;
sem_t sem_slachtici;
sem_t room_empty;
sem_t podd_turniket;
sem_t podd_mutex;
sem_t sla_mutex;
int c_poddany = 0;
int c_sla = 0;

// klananie sa
void klananie(void) {
	printf("Klananie\n");
    sleep(1);
}

// prestavka medzi klananiami
void prestavka(void) {
	printf("Prestavka\n");
    sleep(4);
}

// slachtic
void *slachtic( void *ptr ) {

    // pokial nie je zastaveny
    while(!stoj) {
		printf("Slachtic\n");
		sem_wait(&podd_turniket);
		printf("Slachtic turniket\n");
		sem_wait(&sla_mutex);
		printf("Ya turniketo \n");
		if(c_sla == 0) sem_wait(&room_empty);
		c_sla++;
		sem_post(&sla_mutex);
		
		printf("Slachtich sa ide klanat\n");
        klananie();

		sem_wait(&sla_mutex);
		c_sla--;
		sem_post(&sla_mutex);

		sem_post(&podd_turniket);
		printf("Slachtic si ide dat prestavku\n");
        prestavka();
    }
    return NULL;
}

// poddany
void *poddany( void *ptr ) {

    // pokial nie je zastaveny
    while(!stoj) {
		printf("Poddany\n");

		sem_wait(&podd_turniket);
		sem_post(&podd_turniket);
		printf("Poddany turniket\n");

		sem_wait(&podd_mutex);
		c_poddany++;
		sem_post(&podd_mutex);


		printf("Poddany sa ide klanat\n");
        klananie();

		sem_wait(&podd_mutex);
		c_poddany--;
		printf("PoddNY je %d\n", c_poddany);
		if(c_poddany == 0) sem_post(&room_empty);
		sem_post(&podd_mutex);

		printf("poddany si dava prestavku\n");
        prestavka();
    }
    return NULL;
}

int main(void) {
    int i;

	sem_init(&sem_slachtici, 0, MAX_SLACHTICI);
	sem_init(&podd_turniket, 0, 1);
	sem_init(&podd_mutex, 0, 1);
	sem_init(&room_empty, 0, 1);
	sem_init(&sla_mutex, 0, 1);
	sem_init(&podd_mutex, 0, 1);

    pthread_t slachtici[4];
    pthread_t poddani[10];

    for (i=0;i<4;i++) pthread_create( &slachtici[i], NULL, &slachtic, NULL);
    for (i=0;i<10;i++) pthread_create( &poddani[i], NULL, &poddany, NULL);

    sleep(30);
    stoj = 1;

    for (i=0;i<4;i++) pthread_join( slachtici[i], NULL);
    for (i=0;i<10;i++) pthread_join( poddani[i], NULL);

    exit(EXIT_SUCCESS);
}
