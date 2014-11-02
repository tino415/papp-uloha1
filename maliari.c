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

#define pocet_maliarov 10
#define dlzka_behu_programu 30
#define pocet_maliarov_na_prestavku 3
#define pocet_vedier_na_prestavku 4


// signal na zastavenie
int stoj = 0;
int minute_vedra = 0;
int cakajuci_na_prestavku = 0;
int brana = 0;
pthread_mutex_t mutex_minute_vedra = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pocet_cakajucich = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_cakajuci_na_prestavku;
sem_t sem_iduci_na_prestavku;

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

    // pokial nie je zastaveny
    while(!stoj) {
        maluj(id);
		if(stoj) break;
        ber(id);
		minute_vedra++;
		if(stoj) break;

		if((minute_vedra % pocet_vedier_na_prestavku) == 0) {
			printf("%d chce ist na prestavku\n", id);
			sem_wait(&sem_cakajuci_na_prestavku);
			if(stoj) break;

			printf("%d sa dostal za prvy semafor\n", id);
			
			pthread_mutex_lock(&mutex_pocet_cakajucich);
			cakajuci_na_prestavku++;

			printf("%d je za mutexom na pocet cakajucich\n", id);

			if(cakajuci_na_prestavku == pocet_maliarov_na_prestavku) {
				printf("%d ide posledny na prestavku\n", id);
				int i;
				cakajuci_na_prestavku = 0;
				pthread_mutex_unlock(&mutex_pocet_cakajucich);
				for(i=0;i<(pocet_maliarov_na_prestavku-1);i++) {
					sem_post(&sem_iduci_na_prestavku);
					sem_post(&sem_cakajuci_na_prestavku);
					sem_post(&sem_cakajuci_na_prestavku);
					sem_post(&sem_cakajuci_na_prestavku);
				}
			} else {
				printf("%d caka na poslednecho\n", id);
				pthread_mutex_unlock(&mutex_pocet_cakajucich);
				sem_wait(&sem_iduci_na_prestavku);
				if(stoj) break;
			}
			oddychuj(id);
		}
    }

	printf("Maliar %d minul %d vedier\n", id, minute_vedra);
    return NULL;
}

int main(void) {
    long i;

	sem_init(&sem_cakajuci_na_prestavku, 0, 3);
	sem_init(&sem_iduci_na_prestavku, 0, 0);

    pthread_t maliari[pocet_maliarov];

    for (i=0;i<pocet_maliarov;i++) pthread_create(&maliari[i], NULL, &maliar, (void*)i);

    sleep(dlzka_behu_programu);
    stoj = 1;

	for (i=0;i<pocet_maliarov;i++) {
		sem_post(&sem_iduci_na_prestavku);
		sem_post(&sem_cakajuci_na_prestavku);
	}

    for (i=0;i<pocet_maliarov;i++) pthread_join(maliari[i], NULL);

	printf("Dokopi sa minulo %d vedier\n", minute_vedra);

    exit(EXIT_SUCCESS);
}
