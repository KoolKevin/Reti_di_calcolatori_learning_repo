#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define LINE_LENGTH 256

int main(int argc, char *argv[]) {
    struct hostent    *host;
    struct sockaddr_in servaddr;
    int                port, nread, sd, nwrite;
    /************ VARIABILI DA MODIFICARE IN BASE ALL'ES ************/
    char               directoryName[LINE_LENGTH], buffChar;
    /****************************************************************/

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

    /* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host                = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("Error while resolving %s to IP address\n", argv[1]);
        exit(1);
    }
    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
    servaddr.sin_port        = htons(port);

    /* CREAZIONE SOCKET ------------------------------------ */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Open socket: ");
        exit(3);
    }
    printf("Client: created socket sd=%d\n", sd);

    /* Operazione di BIND implicita nella connect */
    if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0) {
        perror("connect");
        exit(3);
    }
    printf("Client: connect ok\n");


    
    /********************** MODIFICA DA QUA IN POI *********************/



    /* CORPO DEL CLIENT: ciclo di accettazione di richieste da utente  */
    printf("Inserire nome del direttorio, EOF per terminare: ");
    while (gets(directoryName)) {
        printf("Invio il nome del direttorio: %s\n", directoryName);

        /* invio nome directory/file */
        nwrite = write(sd, directoryName, (strlen(directoryName) + 1) );

        // Lettura risposta dal server
        read(sd, &buffChar, sizeof(char));
        // elaborazione esito
        if (buffChar == 'S') {
            printf("Ricevo e stampo i file nel direttorio remoto:\n");
            // Lettura dei nomi di file, in questo caso abbiamo file testo e quindi posso usare un carattere sparatore.
            // Nel caso di file binari leggere prima la dimensione e poi il contenuto con 2 read
            while ((nread = read(sd, &buffChar, sizeof(char))) > 0) {   
                //NB: se leggo 0 vuol dire che l'altro ha chiuso e quindi è arrivato EOF
                //    mentre è molto diverso non avere niente da leggere, in questo caso la read si blocca e non restituisce niente
                if (buffChar == '\1') {
                    break;
                } else if (buffChar == '\0') {
                    printf("\n");
                } else {
                    write(1, &buffChar, sizeof(char));
                }
            }
        } 
        else {
            printf("directory non presente sul server\n");
        }

        printf("Nome del direttorio, EOF per terminare: ");
    }
    /* Chiusura socket */
    close(sd);
    printf("\nClient: termino...\n");

    exit(0);
}