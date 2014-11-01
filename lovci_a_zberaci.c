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

// lovec
void lov(void) {
    sleep(6);
}

void dar_lov(void) {
    sleep(2);
}

void *lovec( void *ptr ) {

    while(!stoj) {
        lov();
        dar_lov();
    }
    return NULL;
}

// zberac
void zber(void) {
    sleep(4);
}

void dar_zber(void) {
    sleep(1);
}

void *zberac( void *ptr ) {

    // pokial nie je zastaveny
    while(!stoj) {
        zber();
        dar_zber();
    }
    return NULL;
}

int main(void) {
    int i;

    pthread_t lovci[6];
    pthread_t zberaci[12];

    for (i=0;i<6;i++) pthread_create( &lovci[i], NULL, &lovec, NULL);
    for (i=0;i<12;i++) pthread_create( &zberaci[i], NULL, &zberac, NULL);

    sleep(30);
    stoj = 1;

    for (i=0;i<6;i++) pthread_join( lovci[i], NULL);
    for (i=0;i<12;i++) pthread_join( zberaci[i], NULL);

    exit(EXIT_SUCCESS);
}
