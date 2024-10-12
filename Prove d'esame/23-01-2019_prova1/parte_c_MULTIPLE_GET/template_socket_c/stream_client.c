#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_NAME_LENGTH 256
#define TARGA_LENGTH 7

int main(int argc, char *argv[]) {
    struct hostent    *host;
    struct sockaddr_in servaddr;
    int port, nread, sd, nwrite;

    char targa[TARGA_LENGTH + 1];
    char char_buf;
    char nomeFile[FILE_NAME_LENGTH + 1];   
    int len;
    int fd;
    int error;
    int contaLetti = 0;

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

    error=0;

    /* CORPO DEL CLIENT: ciclo di accettazione di richieste da utente  */
    printf("Inserire la targa del veicolo di cui vuoi le foto(esattamente 7 caratteri): ");
    while (gets(targa)) {
        printf("\tcerco le foto di: %s\n", targa);

        /* invio nome directory/file */
        nwrite = write(sd, targa, (strlen(targa) + 1) );

        // Lettura risposta dal server
        while ((nread = read(sd, &len, sizeof(int))) > 0) {
            if (len == -1) {
                //ho finito di leggere il file!
                
                // Esco dal ciclo di gestione di questo file
                // senza però chiudere la socket.
                break;
            }
            // Segnale file inesistente
            else if (len == -10) {
                printf("Directory inesistente\n");
                break;
            } else {
                if (read(sd, &nomeFile, len) < 0) {
                    error = 1;
                    perror("read nomefile");
                    //passo alla prossima ricezione
                    continue;
                }

                // Scrivo a video
                printf("\tricevo %s\n", nomeFile);
                //creazione file
                if ((fd = open(nomeFile, O_WRONLY | O_CREAT, 0644)) < 0) {
                    error=1;
                    perror("open file");
                    //passo alla prossima ricezione
                    continue;
                }

                //leggo la lunghezza del file
                if (read(sd, &len, sizeof(int)) < 0) {
                    error = 1;
                    perror("read lunghezza file");
                    //passo alla prossima ricezione
                    continue;
                }
                printf("\tricevo un file lungo %d byte\n", len);

                contaLetti = 0;
                while( contaLetti < len ) {
                    //leggo un carattere alla volta per evitare di strabordare in un altro file nella lettura dello stream
                    read(sd, &char_buf, sizeof(char));
                    write(fd, &char_buf, sizeof(char));
                    contaLetti++;
                }

                close(fd);
                printf("Trasferimento di %s completato!\n", nomeFile);
            }
        }

        if (nread < 0 || error) {
            printf("c'è stato un errore\n");
            break;
        }

        printf("Inserire la targa del veicolo di cui vuoi le foto(esattamente 7 caratteri): ");
    }
    /* Chiusura socket */
    close(sd);
    printf("\nClient: termino...\n");

    exit(0);
}