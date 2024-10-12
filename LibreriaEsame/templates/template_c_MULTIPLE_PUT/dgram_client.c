#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define LINE_LENGTH 256
#define WORD_LENGTH 64

#define TARGA_LENGTH 7
#define PATENTE_LENGTH 5

/************ STRUCT PER RICHIESTA DATAGRAM, (MODIFICA IN BASE ALL'ES) ************/
typedef struct {
    char targa[TARGA_LENGTH + 1];
    char nuova_patente[PATENTE_LENGTH + 1];
} udp_req_t;
/**********************************************************************************/

int main(int argc, char **argv) {
    struct hostent    *host;
    struct sockaddr_in clientaddr, servaddr;
    udp_req_t          req;

    int                port, nread, sd, ris, len = 0;
    

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3) {
        printf("Usage: %s serverAddress serverPort\n", argv[0]);
        exit(1);
    }
    nread = 0;
    while (argv[2][nread] != '\0') {
        if ((argv[2][nread] < '0') || (argv[2][nread] > '9')) {
            printf("Second argument must be an integer!\n");
            printf("Usage: %s serverAddress serverPort\n", argv[0]);
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("Port must be in the range [1024, 65535]\n");
        exit(2);
    }

    /* INIZIALIZZAZIONE INDIRIZZO CLIENT -------------------------- */
    memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
    clientaddr.sin_family      = AF_INET;
    clientaddr.sin_addr.s_addr = INADDR_ANY;
    clientaddr.sin_port        = 0;

    /* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host                = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("Error while resolving %s to IP address\n", argv[1]);
        exit(2);
    } else {
        servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
        servaddr.sin_port        = htons(port);
    }

    /* CREAZIONE SOCKET ---------------------------------- */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Open socket: ");
        exit(3);
    }
    printf("Client: created socket sd=%d\n", sd);

    /* BIND SOCKET  -------------------------------------- */
    if (bind(sd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
        perror("bind socket ");
        exit(1);
    }
    printf("Client: socket bound to port %i\n", clientaddr.sin_port);



    /********************** MODIFICA DA QUA IN POI *********************/



    /* CORPO DEL CLIENT: ciclo di accettazione di richieste da utente */

    printf("Dammi la targa(esattamente 7 caratteri); EOF per terminare: ");
    while (gets(req.targa)) {
        printf("Dammi il nuovo numero di patente(esattamente 5 caratteri): ");
        if (!gets(req.nuova_patente)) {
            printf("Input error!\n");
            break;
        }

        printf("Invio richiesta: targa=%s; nuova_patente=%s\n", req.targa, req.nuova_patente);

        /* invio richiesta */
        len = sizeof(servaddr);
        if (sendto(sd, &req, sizeof(req), 0, (struct sockaddr *)&servaddr, len) < 0) {
            perror("sendto");
            // se questo invio fallisce il client torna all'inzio del ciclo
            printf("Dammi la targa(esattamente 7 caratteri); EOF per terminare: ");
            continue;
        }

        /* ricezione del risultato */
        if (recvfrom(sd, &ris, sizeof(ris), 0, (struct sockaddr *)&servaddr, &len) < 0) {
            perror("Error in recvfrom: ");
            // se questo invio fallisce il client torna all'inzio del ciclo
            printf("Dammi la targa(esattamente 7 caratteri); EOF per terminare: ");
            continue;
        }

        /* elaborazione del risultato */
        if (ris < 0) {
            printf("Errore: targa non trovata\n");
        } else {
            printf("Aggiornamento avvenuto con successo\n");
        }

        printf("Dammi la targa(esattamente 7 caratteri); EOF per terminare: ");
    }

    close(sd);
}