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

// signal na zastavenie simulacie
int stoj = 0;

// pekar
void priprava(void) {
    sleep(4);
}

void pecenie(void) {
    sleep(2);
}

void *pekar( void *ptr ) {

    while(!stoj) {
        priprava();
        pecenie();
    }
    return NULL;
}

int main(void) {
    int i;

    pthread_t pekari[10];

    for (i=0;i<10;i++) pthread_create( &pekari[i], NULL, &pekar, NULL);

    sleep(30);
    stoj = 1;

    for (i=0;i<10;i++) pthread_join( pekari[i], NULL);

    exit(EXIT_SUCCESS);
}
