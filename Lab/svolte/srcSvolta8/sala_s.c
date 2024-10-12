#include "sala.h"
#include <fcntl.h>
#include <rpc/rpc.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

/* STATO SERVER */

static Sala sala;

//la chiamata avviene nello stub
void inizializza() {
    int i, j;

    // inizializzazione struttura dati
    for (i = 0; i < NUMFILE; i++) {
        for (j = 0; j < LUNGHFILA; j++) {
            sala.fila[i].posto[j] = 'L';
        }
    }

    //riempimento
    sala.fila[1].posto[0] = 'D';
    sala.fila[2].posto[1] = 'P';
    sala.fila[5].posto[2] = 'P';

    for (j = 0; j < 3; j++) {
        sala.fila[8].posto[j] = 'B';
    }
    for (i = 1; i < 4; i++) {
        for (j = 4; j < 6; j++) {
            sala.fila[i].posto[j] = 'P';
        }
    }
    for (i = 5; i < 7; i++) {
        for (j = 4; j < 6; j++) {
            sala.fila[i].posto[j] = 'P';
        }
    }

    sala.fila[1].posto[6] = 'B';

    //fine inizializzazione
    printf("Terminata inizializzazione struttura dati!\n");
}

int *prenota_postazione_1_svc(Input *input, struct svc_req *rqstp) {
    static int result = -1;

    printf("Ricevuta richiesta di prenotazione per: %c, fila %i, colonna %i\n", input->tipo, input->fila, input->colonna);

    if (sala.fila[input->fila].posto[input->colonna] != 'L') {
        result = -2;
        return (&result);
    } else {
        sala.fila[input->fila].posto[input->colonna] = input->tipo;
        result                                       = 0;
        return (&result);
    }
} // prenota

Sala *visualizza_stato_1_svc(void *in, struct svc_req *rqstp) {
    return (&sala);
} // visualizza
