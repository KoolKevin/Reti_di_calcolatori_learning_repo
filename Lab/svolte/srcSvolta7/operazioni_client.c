/* operazioni_client.c
 *
 */

#include "operazioni.h"
#include <rpc/rpc.h>
#include <stdio.h>

#define DIM_BUF 20

int main(int argc, char *argv[]) {

    CLIENT  *cl;
    int     *ris;
    char    *server;
    char op1[DIM_BUF], op2[DIM_BUF], operazione[DIM_BUF];
    Operandi op;

    // variante
    // if (argc != 5) {
    //     fprintf(stderr, "uso: %s host s/m op1 op2\n", argv[0]);
    //     exit(1);
    // }
    // if (argv[2][0] != 'm' && argv[2][0] != 's') {
    //     fprintf(stderr, "uso: %s host somma/moltiplicazione op1 op2\n", argv[0]);
    //     fprintf(stderr, "il tipo di operazione deve iniziare per 's' o 'm'\n");
    //     exit(1);
    // }

    if (argc != 2) {
        fprintf(stderr, "uso: %s host\n", argv[0]);
        exit(1);
    }

    server = argv[1];
    //creazione gestore di trasporto
    cl = clnt_create(server, OPERAZIONIPROG, OPERAZIONIVERS, "udp");
    if (cl == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }
        
    printf("inserisci l'operazione(s/m): ");
    while( gets(operazione) )  {
        printf("inserisci il primo operando: ");
        gets(op1);
        op.op1 = atoi(op1);
        printf("inserisci il secondo operando: ");
        gets(op2);
        op.op2 = atoi(op2); 

        //printf("%d%s%d\n", op.op1, operazione, op.op2);

        if (operazione[0] == 's') {
            ris = somma_1(&op, cl);
        }
        if (operazione[0] == 'm') {
            ris = moltiplicazione_1(&op, cl);
        }
        if (ris == NULL) {
            clnt_perror(cl, server);
            exit(1);
        }

        printf("Operandi inviati a %s: %i e %i\n", server, op.op1, op.op2);
        printf("Risultato ricevuto da %s: %i\n\n", server, *ris);

        printf("inserisci l'operazione(s/m): ");
    }

    // Libero le risorse distruggendo il gestore di trasporto
    clnt_destroy(cl);
}
