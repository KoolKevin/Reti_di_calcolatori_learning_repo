/* dl-client.c
 * Il cliente invia il numero di linea
 * e il contenuto di un file al server, riceve il nuovo contenuto
 * e lo salva in locale.
 */

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DIM_BUFF    256
#define LINE_LENGTH 128

int main(int argc, char *argv[]) {
    int                sd, fd_sorg, fd_dest, nread, port, line;
    char               buff[DIM_BUFF], car, c;
    char               nome_sorg[LINE_LENGTH], nome_dest[LINE_LENGTH], okstr[LINE_LENGTH];
    char               terminator = '\0';
    struct hostent    *host;
    struct sockaddr_in servaddr;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3) {
        printf("Error:%s serverAddress serverPort\n", argv[0]);
        exit(1);
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    /*
     * NOTA: gethostbyname restituisce gli indirizzi gia' in formato di rete
     */
    host = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(1);
    }

    nread = 0;
    while (argv[2][nread] != '\0') {
        if ((argv[2][nread] < '0') || (argv[2][nread] > '9')) {
            printf("Secondo argomento non intero\n");
            exit(2);
        }
        nread++;
    }

    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("Porta scorretta...");
        exit(2);
    }

    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
    servaddr.sin_port        = htons(port);

    /* CORPO DEL CLIENT:
     ciclo di accettazione di richieste da utente ------- */
    printf("Richiesta di eliminare una linea in un file\n");
    printf("Nome del file, EOF per terminare: ");

    /* ATTENZIONE!!
     * Cosa accade se la riga e' piu' lunga di LINE_LENGTH-1?
     * Stesso dicasi per le altre gets...
     */
    while (gets(nome_sorg)) {
        // Richiesta e verifica file source
        printf("File da aprire: %s\n", nome_sorg);
        if ((fd_sorg = open(nome_sorg, O_RDONLY)) < 0) {
            perror("open file sorgente");
            printf("Nome del file, EOF per terminare: ");
            continue;
        }

        // richiesta e verifica file dest
        printf("Nome del file senza la linea: ");
        if (gets(nome_dest) == 0) {
            // EOF has been reached
            break;
        } else {
            if ((fd_dest = open(nome_dest, O_WRONLY | O_CREAT, 0644)) < 0) {
                perror("open file destinatario");
                printf("Nome del file, EOF per terminare: ");
                continue;
            } else {
                printf("File senza la linea: %s\n", nome_dest);
            }
        }

        // intero da tastiera
        printf("linea da eliminare: ");
        while (scanf("%d", &line) != 1) {
            do {
                c = getchar();
                printf("%c ", c);
            } while (c != '\n');
            printf("Inserire int");
            continue;
        }
        gets(okstr);
        printf("Numero linea %d \n", line);

        /* CREAZIONE SOCKET ------------------------------------ */
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd < 0) {
            perror("apertura socket");
            exit(1);
        }
        printf("Client: creata la socket sd=%d\n", sd);

        /* Operazione di BIND implicita nella connect */
        if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0) {
            perror("connect");
            exit(1);
        }
        printf("Client: connect ok\n");

        // Invio del numero riga
        printf("Invio il numero riga: %d\n", line);
        write(sd, &line, sizeof(int));

        /* INVIO DEL FILE *************************************************************************
         * Esercizio: provare a misurare il tempo di trasferimento con diverse dimensioni del
         * buffer, misurandolo sia lato client che lato server.
         *
         * Inoltre, in questa soluzione inviamo prima tutto il file e poi lo riceviamo tutto.
         * Siccome il server NON salva il file dal suo lato, ci possono essere dei
         * problemi se il file è di grandi dimensioni? Quali? Come potremmo cambiare
         * questa soluzione per evitarli?
         */
        printf("Client: invio file\n");
        while ((nread = read(fd_sorg, buff, DIM_BUFF)) > 0) {
            write(sd, buff, nread);
        }
        /* Chiusura socket in spedizione -> invio dell'EOF */
        shutdown(sd, 1);
        printf("Client: file inviato\n");

        /* RICEZIONE File */
        printf("Client: ricevo e stampo file senza la linea\n");
        while ((nread = read(sd, buff, DIM_BUFF)) > 0) {
            write(fd_dest, buff, nread);
            write(1, buff, nread);
        }
        printf("\nTrasferimento terminato\n");
        /* Chiusura socket in ricezione */
        shutdown(sd, 0);

        /* Chiusura file */
        close(fd_sorg);
        close(fd_dest);

        printf("Nome del file per cui si vuole eliminare la linea, EOF per terminare: ");
    } // while

    printf("\nClient: termino...\n");
    exit(0);
}
