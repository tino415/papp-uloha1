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

// signal na zastavenie simulacie
int stoj = 0;

// klananie sa
void klananie(void) {
    sleep(1);
}

// prestavka medzi klananiami
void prestavka(void) {
    sleep(4);
}

// slachtic
void *slachtic( void *ptr ) {

    // pokial nie je zastaveny
    while(!stoj) {
        klananie();
        prestavka();
    }
    return NULL;
}

// poddany
void *poddany( void *ptr ) {

    // pokial nie je zastaveny
    while(!stoj) {
        klananie();
        prestavka();
    }
    return NULL;
}

int main(void) {
    int i;

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
