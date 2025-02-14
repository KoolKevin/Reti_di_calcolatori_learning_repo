#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DIM_BUFF 256
#define LINE_LENGTH 256

// invocazione DeleteLineClient ip porta

int main(int argc, char *argv[]) {
    int  sd, port, fd_sorg, fd_dest, nread, numeroLinea, ok;
    char buff[DIM_BUFF], c;
    // FILENAME_MAX: lunghezza massima nome file. Costante di sistema.
    char               nome_sorg[FILENAME_MAX + 1], nome_dest[FILENAME_MAX + 1];
    char               okstr[LINE_LENGTH];
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
    host                = gethostbyname(argv[1]);

    /*VERIFICA INTERO*/
    nread = 0;
    while (argv[2][nread] != '\0') {
        if ((argv[2][nread] < '0') || (argv[2][nread] > '9')) {
            printf("Secondo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[2]);

    /* VERIFICA PORT e HOST */
    if (port < 1024 || port > 65535) {
        printf("%s = porta scorretta...\n", argv[2]);
        exit(2);
    }
    if (host == NULL) {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(2);
    } else {
        servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
        servaddr.sin_port        = htons(port);
    }

    

    /* CORPO DEL CLIENT: ciclo di accettazione di richieste da utente ------- */
    printf("Nome del file e numero di linea( separati da UNO spazio), EOF per terminare: ");

    while ( (ok=scanf("%s %d", nome_sorg, &numeroLinea)) != EOF ) { 
        //TODO: modifica il client ed il server per permettere l'utilizzo di una connessione persistente -> manda la lunghezza del file prima del file
        /* CREAZIONE SOCKET ------------------------------------ */        
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd < 0) {
            perror("apertura socket");
            exit(1);
        }
        printf("Client: creata la socket sd=%d\n", sd);

        /* Operazione di BIND implicita nella connect */
        if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0) {
            perror("connect");
            exit(1);
        }
        printf("Client: connect ok\n");

        if (ok != 2) { // errore di formato
            printf("errore di formato!!!\n");
            do {
                c=getchar(); 
                printf("%c", c);
            } while (c!= '\n');

            printf("Nome del file e numero di linea( separati da UNO spazio), EOF per terminare: ");
            continue;
        }
        
        // quando arrivo qui l'input e' stato letto correttamente
        // Consumo il new line, ed eventuali altri caratteri immessi nella riga dopo l'intero letto
        gets(okstr);

        printf("voglio eliminare la riga %d, del file %s \n", numeroLinea, nome_sorg);

        /* Verifico l'esistenza del file */
        if ((fd_sorg = open(nome_sorg, O_RDONLY)) < 0) {
            perror("open file sorgente");
            printf("Nome del file e numero di linea( separati da UNO spazio), EOF per terminare: ");
            continue;
        }

        printf("Nome del file senza linea: ");
        if (gets(nome_dest) == 0)
            break;

        /*Verifico creazione file*/
        if ((fd_dest = open(nome_dest, O_WRONLY | O_CREAT, 0644)) < 0) {
            perror("open file destinatario");
            printf("Nome del file e numero di linea( separati da UNO spazio), EOF per terminare: ");
            continue;
        }


        /*INVIO numero di linea*/
        if( write(sd, &numeroLinea, sizeof(int)) < 0 ) {
            perror("write numeroLinea");
            printf("Nome del file e numero di linea( separati da UNO spazio), EOF per terminare: ");
            continue;
        }

        /*INVIO File*/
        printf("Client: stampo e invio file da cui togliere la riga\n");
        while ((nread = read(fd_sorg, buff, DIM_BUFF)) > 0) {
            write(1, buff, nread);  // stampa
            write(sd, buff, nread); // invio
        }
        printf("Client: file inviato\n");
        /* Chiusura socket in spedizione -> invio dell'EOF */
        shutdown(sd, 1);

        /*RICEZIONE File*/
        printf("Client: ricevo e stampo senza linea richiesta\n");
        while ((nread = read(sd, buff, DIM_BUFF)) > 0) {
            write(fd_dest, buff, nread);
            write(1, buff, nread);
        }
        printf("Trasferimento terminato\n");
        /* Chiusura socket in ricezione */
        shutdown(sd, 0);
        /* Chiusura file */
        close(fd_sorg);
        close(fd_dest);
        

        printf("Nome del file e numero di linea( separati da UNO spazio), EOF per terminare: ");
        close(sd);
    } // while

    

    printf("\nClient: termino...\n");
    exit(0);
}
