/* Client per richiedere l'invio di un file (get, versione 1) */

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DIM_BUFF         100
#define LENGTH_FILE_NAME 50

int main(int argc, char *argv[]) {
    int                sd, nread, port;
    char               c, ok, nome_dir[LENGTH_FILE_NAME], buffChar;
    struct hostent    *host;
    struct sockaddr_in servaddr;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3) {
        printf("Error:%s serverAddress serverPort\n", argv[0]);
        exit(1);
    }
    printf("Client avviato\n");

    /* PREPARAZIONE INDIRIZZO SERVER ----------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host                = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(2);
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

    /* CREAZIONE E CONNESSIONE SOCKET (BIND IMPLICITA) ----------------- */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("apertura socket ");
        exit(3);
    }
    printf("Creata la socket sd=%d\n", sd);

    if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0) {
        perror("Errore in connect");
        exit(4);
    }
    printf("Connect ok\n");

    /* CORPO DEL CLIENT: */
    /* ciclo di accettazione di richieste di nomi di direttorio ------- */
    printf("Nome del file da richiedere: ");

    while (gets(nome_dir)) {
        if (write(sd, nome_dir, (strlen(nome_dir) + 1)) < 0) {  // mando al server il nome della directory che voglio
            perror("write");
            break;
        }
        printf("Richiesta del nome della directory %s inviata... \n", nome_dir);

        if (read(sd, &ok, 1) < 0) { //leggo esito
            perror("read");
            break;
        }

        //TODOS per leggere anche il contenuto di file binari

        if (ok == 'S') {
            //TODO: leggo la dimensione della directory
            //TODO: leggo la dimensione del file

            //leggo il file
            printf("Ricevo i file:\n");
            while ( (nread = read(sd, &buffChar, sizeof(char)/* dimFile */)) > 0 /*< dimDirectory*/) {
                if (buffChar == '\1') {
                    break;
                } else if (buffChar == '\0') {
                    printf("\n");
                } else {
                    write(1, &buffChar, sizeof(char));
                }
            }

            if (nread < 0) {
                perror("read");
                break;
            }
        } else if (ok == 'N')
            printf("File inesistente\n");
        else {
            printf("Errore di protocollo\n"); // controllare sempre che il protocollo sia rispettato
        }
        printf("Nome del file da richiedere: ");
    } // while
    printf("\nClient: termino...\n");
    shutdown(sd, 0);
    shutdown(sd, 1);
    close(sd);
    exit(0);
}
