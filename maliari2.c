/*
Meno:
Datum:

Simulujte nasledujucu situaciu. Desiati maliari maluju steny.
Maliarovi trva nejaky cas, kym stenu maluje (v simulacii 2s) a nejaky cas, kym si ide nabrat farbu do vedra (v simulacii 1s). Cela simulacia nech trva nejaky cas (30s).

1. Doplnte do programu pocitadlo celkoveho poctu vedier minutej farby a tiez nech si kazdy maliar pocita, kolko vedier farby uz minul preniesol, na konci simulacie vypiste hodnoty pocitadiel. [2b]

2. Ked maliar minie 4 vedra, pocka na dvoch svojich kolegov a kazdy si spravi prestavku na nejaky cas (v simulacii 2s). [5b]

3. Osetrite v programe spravne ukoncenie simulacie hned po uplynuti stanoveneho casu (nezacne sa dalsia cinnost). [3b]


Poznamky:
- na synchronizaciu pouzite iba mutexy+podmienene premenne alebo semafory
- nespoliehajte sa na uvedene casy, simulacia by mala fungovat aj s inymi casmi
- build (console): gcc maliari.c -o maliari -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define POCET_MALIAROV 10
#define DLZKA_BEHU_PROGRAMU 30
#define POCET_MALIAROV_NA_PRESTAVKU 3
#define POCET_VEDIER_NA_PRESTAVKU 4

// signal na zastavenie
int stoj = 0;
int minute_vedra = 0;
int cakajuci_na_prestavku = 0;
pthread_mutex_t mutex_minute_vedra = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pocet_cakajucich = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_turniket = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_cakajuci_na_prestavku;

// maliar - malovanie steny
void maluj(int id) {
	printf("%d maluje\n", id);
    sleep(2);
}

//  maliar - branie farby
void ber(int id) {
	printf("%d berie farbu\n", id);
	pthread_mutex_lock(&mutex_minute_vedra);
		minute_vedra++;
	pthread_mutex_unlock(&mutex_minute_vedra);
  	sleep(1);

}

void oddychuj(int id) {
	printf("Oddychuje %d\n", id);
	sleep(2);
}

// maliar
void *maliar( void *ptr ) {
	int id = (long)ptr;
	int minute_vedra = 0;
	int i;

    // pokial nie je zastaveny
    while(!stoj) {
        maluj(id);
		if(stoj) break;
        ber(id);
		minute_vedra++;
		if(stoj) break;

		if((minute_vedra % POCET_VEDIER_NA_PRESTAVKU) == 0) {

			pthread_mutex_lock(&mutex_pocet_cakajucich);

			printf("%d chce ist na prestavku [cakajuci %d]\n", id, cakajuci_na_prestavku);
			if(cakajuci_na_prestavku == (POCET_MALIAROV_NA_PRESTAVKU-1)) {
				printf("%d je posledny\n", id);
				cakajuci_na_prestavku = 0;
				for(i=0; i<(POCET_MALIAROV_NA_PRESTAVKU-1);i++) {
					sem_post(&sem_cakajuci_na_prestavku);
					pthread_mutex_lock(&mutex_turniket);
				}
				pthread_mutex_unlock(&mutex_pocet_cakajucich);
			} else {
				cakajuci_na_prestavku++;
				printf("%d caka na posledneho\n", id);
				pthread_mutex_unlock(&mutex_pocet_cakajucich);
				sem_wait(&sem_cakajuci_na_prestavku);
				pthread_mutex_unlock(&mutex_turniket);
			}
			if(stoj) break;
			oddychuj(id);
		}
    }

	printf("Maliar %d minul %d vedier\n", id, minute_vedra);
    return NULL;
}

int main(void) {
    long i;

	sem_init(&sem_cakajuci_na_prestavku, 0, 0);

    pthread_t maliari[POCET_MALIAROV];

    for (i=0;i<POCET_MALIAROV;i++) pthread_create(&maliari[i], NULL, &maliar, (void*)i);

    sleep(DLZKA_BEHU_PROGRAMU);
    stoj = 1;

	for (i=0;i<POCET_MALIAROV;i++) {
		sem_post(&sem_cakajuci_na_prestavku);
	}

    for (i=0;i<POCET_MALIAROV;i++) pthread_join(maliari[i], NULL);

	printf("Dokopi sa minulo %d vedier\n", minute_vedra);

    exit(EXIT_SUCCESS);
}
