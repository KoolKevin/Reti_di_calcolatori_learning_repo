/* Server Select
 * 	Un solo figlio per tutti i file.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define DIM_BUFF         100
#define DIM_RIGA_BUFF    256
#define LENGTH_FILE_NAME 100
#define LENGTH_WORD 50
#define max(a, b)        ((a) > (b) ? (a) : (b))

typedef struct {
    char nomeFile[LENGTH_FILE_NAME];
    char parola[LENGTH_WORD];
} Request;


/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv) {
    int       listenfd, connfd, udpfd, fd_file, nready, maxfdp1;
    int contatoreParola=0, contatoreRiga=0;
    const int on   = 1;
    char     charBuff, word_buffer[DIM_BUFF], riga_buffer[DIM_RIGA_BUFF];
    char err[128], datagram_ris, stream_ris[LENGTH_FILE_NAME];
    const char terminatore_sessione = '\1';
    fd_set    rset;
    int       len, nread, nwrite, port;
    struct sockaddr_in cliaddr, servaddr;
    Request datagramReq, streamReq;
    int flagAggiungiRiga = 0;

    
    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    }
    nread = 0;
    //controllo valore porta
    while (argv[1][nread] != '\0') {
        if ((argv[1][nread] < '0') || (argv[1][nread] > '9')) {
            printf("Terzo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        printf("Porta scorretta...");
        exit(2);
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

    printf("Server avviato\n");

    /* CREAZIONE SOCKET TCP ------------------------------------------------------ */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("apertura socket TCP ");
        exit(1);
    }
    printf("Creata la socket TCP d'ascolto, fd=%d\n", listenfd);

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket TCP");
        exit(2);
    }
    printf("Set opzioni socket TCP ok\n");

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket TCP");
        exit(3);
    }
    printf("Bind socket TCP ok\n");

    if (listen(listenfd, 5) < 0) {
        perror("listen");
        exit(4);
    }
    printf("Listen ok\n");

    /* CREAZIONE SOCKET UDP ------------------------------------------------ */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpfd < 0) {
        perror("apertura socket UDP");
        exit(5);
    }
    printf("Creata la socket UDP, fd=%d\n", udpfd);

    if (setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket UDP");
        exit(6);
    }
    printf("Set opzioni socket UDP ok\n");

    if (bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket UDP");
        exit(7);
    }
    printf("Bind socket UDP ok\n");

    /* AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE -------------------------------- */
    signal(SIGCHLD, gestore);

    /* PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR ------------------------- */
    FD_ZERO(&rset);
    maxfdp1 = max(listenfd, udpfd) + 1;

    /* CICLO DI RICEZIONE EVENTI DALLA SELECT ----------------------------------- */
    for (;;) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);

        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;
            else {
                perror("select");
                exit(8);
            }
        }

        /* GESTIONE RICHIESTE DI GET DI LINEE DI UN FILE ------------------------------------- */
        if (FD_ISSET(listenfd, &rset)) {
            printf("Ricevuta richiesta di get di linee di un file\n");
            len = sizeof(struct sockaddr_in);
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
                if (errno == EINTR)
                    continue;
                else {
                    perror("accept");
                    exit(9);
                }
            }
            
            
            if(fork() == 0) { /* processo figlio che serve la richiesta di operazione */
                close(listenfd);
                printf("Dentro il figlio, pid=%i\n", getpid());

                //sessione client
                while ( read(connfd, &streamReq, sizeof(Request)) > 0 ) {   //leggo il nome della directory
                    printf("Server (figlio) richiesta: file=%s;\tparola=%s\n", streamReq.nomeFile, streamReq.parola);

                    if ((fd_file = open(streamReq.nomeFile, O_RDONLY)) < 0) {
                        perror("Errore apertura file sorgente");
                        strcpy( stream_ris, "file non presente!");
                        write(connfd, stream_ris, strlen(stream_ris)+1 );
                        write(connfd, &terminatore_sessione, sizeof(char));
                        continue; //vado alla prossima richiesta della sessione cliente
                    }

                    contatoreParola = 0;
                    contatoreRiga = 0;
                    flagAggiungiRiga = 0;
                    //leggo il file carattere per carattere per trovare la parola
                    while ((nread = read(fd_file, &charBuff, sizeof(char))) != 0) {
                        if (nread < 0) {
                            sprintf(err, "(PID %d) impossibile leggere dal file", getpid());
                            perror(err);
                            write(connfd, stream_ris, sizeof(stream_ris));
                            write(connfd, &terminatore_sessione, sizeof(char));
                            continue; //vado alla prossima richiesta della sessione cliente
                        } else {
                            //popolo word buffer e cerco la parola se non la ho gia trovata
                            if( flagAggiungiRiga == 0) {
                                if ( (charBuff == ' ' || charBuff == '\r' || charBuff == '\n') ) {
                                    word_buffer[contatoreParola] = '\0';
                                    if ( strcmp(word_buffer, streamReq.parola) == 0 ) { 
                                        //trovato parola
                                        flagAggiungiRiga=1;
                                    }

                                    contatoreParola = 0;
                                }
                                else { // carattere
                                    word_buffer[contatoreParola] = charBuff;
                                    contatoreParola++;
                                }
                            }

                            //popolo riga buffer(
                            if( charBuff == '\n' && flagAggiungiRiga == 1 ) {
                                riga_buffer[contatoreRiga] = '\0';
                                write(connfd, riga_buffer, strlen(riga_buffer)+1);
                                printf("mando la riga %s\n", riga_buffer);
                                contatoreRiga=0;
                                contatoreParola=0;
                                flagAggiungiRiga=0;
                            }
                            else if( charBuff == '\n' && flagAggiungiRiga == 0 ) {
                                contatoreRiga=0;
                            }
                            else{
                                riga_buffer[contatoreRiga] = charBuff;
                                contatoreRiga++;
                            }
                        }
                    }
                    
                    //ho finito di ispezionare il file -> mando un carattere separatore al cliente
                    write(connfd, &terminatore_sessione, sizeof(char));
                }     // for

                printf("Figlio %i: chiudo connessione e termino\n", getpid());
                close(connfd);
                exit(0);
            } // figlio
            /* padre chiude la socket dell'operazione */
            close(connfd);
        } /* fine gestione richieste di file */

        /* GESTIONE RICHIESTE DI CONTROLLO ESISTENZA DI UNA PAROLA IN UN FILE ------------------------------------------ */
        if (FD_ISSET(udpfd, &rset)) {
            printf("Ricevuta richiesta di UDP: check esistenza\n");
            len = sizeof(struct sockaddr_in);

            if (recvfrom(udpfd, &datagramReq, sizeof(datagramReq), 0, (struct sockaddr *)&cliaddr, &len) < 0) {
                perror("recvfrom ");
                continue;
            }

            printf("Operazione richiesta sul file: %s per la parola: %s\n", datagramReq.nomeFile, datagramReq.parola);

            /* Verifico file */
            int ris = 0;
            contatoreParola=0;

            if ((fd_file = open(datagramReq.nomeFile, O_RDONLY)) < 0) {
                perror("Errore apertura file sorgente");
                ris = -1;
            }
            else {
                while ( ris==0 && ((nread = read(fd_file, &charBuff, sizeof(char)))!=0) ) {
                    if (nread < 0) {
                        sprintf(err, "(PID %d) impossibile leggere dal file", getpid());
                        perror(err);
                        ris=-2;
                        break;
                    } 
                    else {
                        if ( (charBuff == ' ' || charBuff == '\r' || charBuff == '\n') ) {
                            word_buffer[contatoreParola] = '\0';
                            printf("parola trovata: %s\n", word_buffer);

                            if ( strcmp(word_buffer, datagramReq.parola) == 0 ) { 
                                //trovato parola
                                printf("parola cercata trovata! %s\n", word_buffer);
                                ris=1;
                            }

                            contatoreParola = 0;
                        }
                        else { // carattere
                            word_buffer[contatoreParola] = charBuff;
                            contatoreParola++;
                        }
                    
                    }
                }
            }

            close(fd_file);

            // Send result to the client
            if (sendto(udpfd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                perror("sendto ");
                continue;
            }
        }     
    }/* ciclo for della select */
}
