/*
Meno:
Datum:

Simulujte nasledujucu situaciu. V kmeni su dve kasty: lovci (6 lovcov) a zberaci (12 zberacov). Uctievaju bozstvo, ktoremu chodia davat dary do chramu. Lovec lovi zver nejaky cas (v simulacii 6s) a potom ide do chramu dat cast ulovku ako dar bozstvu, co tiez trva nejaky cas (v simulacii 2s). Zberac zbiera plody nejaky cas (v simulacii 4s) a potom ide do chramu dat cast plodov bozstvu, co tiez trva nejaky cas (v simulacii 1s). Cela simulacia nech trva 30s.

1. Doplnte do programu pocitadlo pocitajuce, kolko krat bozstvu dali dar lovci a kolko krat zberaci. [2b]

2. Zabezpecte, aby do chramu sucasne mohli vojst maximalne dvaja lovci alebo styria zberaci, iba prislusnici jednej kasty naraz. Ak je pred chramom rad, zabezpecte spravodlivy pristup (kasty su si rovnocenne). [5b]

3. Osetrite v programe spravne ukoncenie simulacie po uplynuti stanoveneho casu. [3b]

Poznamky:
- na synchronizaciu pouzite iba mutexy, podmienene premenne alebo semafory
- nespoliehajte sa na uvedene casy, simulacia by mala fungovat aj s inymi casmi
- build (console): gcc lovci_a_zberaci.c -o lovci_a_zberaci -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// signal na zastavenie simulacie
int stoj = 0;

int dar_od_lovcov = 0;
int dar_od_zberacov = 0;

pthread_mutex_t mutex_dari_lovcov = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_dari_zberacov = PTHREAD_MUTEX_INITIALIZER;
sem_t l_controll, z_controll;
int l_count = 0;
int z_count = 0;
sem_t room_empty;
sem_t l_in_room, z_in_room;
sem_t z_turnstile;

// lovec
void lov(int id) {
	printf("%d lovi\n", id);
    sleep(6);
}

void dar_lov(int id) {
	printf("%d daruje lov v chrame\n", id);
	pthread_mutex_lock(&mutex_dari_lovcov);
	dar_od_lovcov++;
	pthread_mutex_unlock(&mutex_dari_lovcov);
    sleep(2);
}

void *lovec( void *ptr ) {
	int id = (long) ptr;

    while(!stoj) {
		
        lov(id);

		sem_wait(&l_controll);
		if(l_count == 0) {
			printf("Prvy lovec%d\n", id);
			sem_wait(&z_turnstile);	

			printf("PL zab turn %d\n", id);
			sem_post(&z_controll);
			printf("PL odblok controll %d\n", id);
			sem_wait(&room_empty);
			printf("PL room_emp %d\n", id);
		}
		l_count++;
		sem_post(&l_controll);

		sem_wait(&l_in_room);
        dar_lov(id);
		sem_post(&l_in_room);

		sem_wait(&l_controll);
		l_count--;
		if(l_count == 0) sem_post(&room_empty);
		sem_post(&l_controll);
    }
    return NULL;
}

// zberac
void zber(int id) {
	printf("%d zbera\n", id);
}

void dar_zber(int id) {
	printf("%d daruje zber\n", id);
	pthread_mutex_lock(&mutex_dari_zberacov);
	dar_od_zberacov++;
	pthread_mutex_unlock(&mutex_dari_zberacov);
    sleep(1);
}

void *zberac( void *ptr ) {
	int id = (long) ptr;

    // pokial nie je zastaveny
    while(!stoj) {
        zber(id);

		sem_wait(&z_turnstile);
		sem_post(&z_turnstile);

		sem_wait(&z_controll);
		if(z_count == 0) sem_wait(&room_empty);
		z_count++;
		sem_post(&z_controll);

		sem_wait(&z_in_room);
        dar_zber(id);
		sem_post(&z_in_room);

		sem_wait(&z_controll);
		z_count--;
		if(z_count == 0) sem_post(&room_empty);
		sem_post(&z_controll);
    }
    return NULL;
}

int main(void) {
    long i;

    pthread_t lovci[6];
    pthread_t zberaci[12];

	sem_init(&l_controll, 0, 1);
	sem_init(&z_controll, 0, 1);
	sem_init(&room_empty, 0, 1);
	sem_init(&l_in_room, 0, 4);
	sem_init(&z_in_room, 0, 6);
	sem_init(&z_turnstile, 0, 1);

    for (i=0;i<6;i++) pthread_create( &lovci[i], NULL, &lovec, (void*)i);
    for (i=0;i<12;i++) pthread_create( &zberaci[i], NULL, &zberac, (void*)i);

    sleep(30);
    stoj = 1;

    for (i=0;i<6;i++) pthread_join( lovci[i], NULL);
    for (i=0;i<12;i++) pthread_join( zberaci[i], NULL);

	printf("Zberaci darovali %d darov\n", dar_od_zberacov);
	printf("Lovci darovali %d darov\n", dar_od_lovcov);

    exit(EXIT_SUCCESS);
}
