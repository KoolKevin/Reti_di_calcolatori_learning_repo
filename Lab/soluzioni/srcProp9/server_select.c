/* Server Select (versione 1)*/
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define DIM_BUFF         100
#define LENGTH_FILE_NAME 90
#define LENGTH_PAROLA    30

#define max(a, b) ((a) > (b) ? (a) : (b))

/********************************************************/
/* Struttura per datagramma */
struct file_e_parola {
    char filename[LENGTH_FILE_NAME];
    char parola[LENGTH_PAROLA];
};

/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}

/********************************************************/
/* Legge una alla volta le linee del file collocandole nel buffer
 * passato come argomento. Il valore di ritorno è quello della read,
 * che può essere controllato per EOF.
 */
int leggiLinea(int fd, char *linea) {
    char c;
    int  letti, i = 0;
    while ((letti = read(fd, &c, 1) != 0) && (c != '\n')) {
        linea[i] = c;
        i++;
    }
    linea[i] = '\0';
    return letti;
}

/********************************************************/
int main(int argc, char **argv) {
    int                  listenfd, connfd, udpfd, fd_file, nready, maxfdp1;
    const int            on = 1;
    struct file_e_parola recv_dgram;
    char                 nome_file[LENGTH_FILE_NAME], parola[LENGTH_PAROLA];
    char                 linea[DIM_BUFF];
    fd_set               rset;
    int                  len, nwrite, port, length, ris;
    struct sockaddr_in   cliaddr, servaddr;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    } else
        port = atoi(argv[1]); // Aggiungere controllo porta
    printf("Server avviato\n");

    /* CREAZIONE SOCKET TCP ------------------------------------------------------ */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("apertura socket TCP ");
        exit(1);
    }
    printf("Creata la socket TCP d'ascolto, fd=%d\n", listenfd);

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

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

    /* INIZIALIZZAZIONE INDIRIZZO SERVER E BIND ---------------------------- */
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

        /* GESTIONE RICHIESTE DI GET DI UN FILE ------------------------------------- */
        if (FD_ISSET(listenfd, &rset)) {
            printf("Ricevuta richiesta di get di un file\n");
            len = sizeof(struct sockaddr_in);
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
                if (errno == EINTR)
                    continue;
                else {
                    perror("accept");
                    exit(9);
                }
            }

            if (fork() == 0) { /* processo figlio che serve la richiesta di operazione */
                close(listenfd);
                printf("Dentro il figlio, pid=%i\n", getpid());
                int readRes;

                // 1. Lettura della lunghezza del nome del file
                while ((readRes = read(connfd, &length, sizeof(int))) > 0) {

                    // 2. Lettura nome file
                    if ((readRes = read(connfd, &nome_file, length)) < 0) {
                        perror("read");
                        break;
                    } else if (readRes == 0) // abbiamo raggiunto la EOF
                    {
                        printf("Ricevuto EOF\n");
                        break;
                    }
                    printf("Richiesto file %s\n", nome_file);

                    // 3. Lettura parola, lunghezza poi stringa
                    if ((readRes = read(connfd, &length, sizeof(int))) < 0) {
                        perror("read");
                        break;
                    } else if (readRes == 0) // abbiamo raggiunto la EOF
                    {
                        printf("Ricevuto EOF\n");
                        break;
                    }
                    if ((readRes = read(connfd, &parola, length)) < 0) {
                        perror("read");
                        break;
                    } else if (readRes == 0) // abbiamo raggiunto la EOF
                    {
                        printf("Ricevuto EOF\n");
                        break;
                    }
                    printf("Richiesta parola %s\n", parola);

                    // Apertura file
                    fd_file = open(nome_file, O_RDONLY);
                    if (fd_file < 0) {
                        printf("File inesistente\n");
                        length = -2;
                        write(connfd, &length, sizeof(int));
                    } else {
                        /* lettura da file e invio delle linee con parola */
                        while (leggiLinea(fd_file, linea) > 0) {
                            if (strstr(linea, parola)) {
                                length = strlen(linea) + 1;
                                // invio lunghezza linea e linea
                                if ((nwrite = write(connfd, &length, sizeof(int))) < 0) {
                                    perror("write");
                                    break;
                                }
                                if ((nwrite = write(connfd, linea, strlen(linea) + 1)) < 0) {
                                    perror("write");
                                    break;
                                }
                            }
                        }
                        // il file e' terminato, lo segnalo al client e libero le
                        // risorse
                        length = -1;
                        if ((nwrite = write(connfd, &length, sizeof(int))) < 0) {
                            perror("write");
                            break;
                        }
                        close(fd_file);
                        printf("Terminato invio file\n");
                    } // else
                }     // while
                
                // Lettura nome file, lunghezza poi stringa
                if (readRes < 0) {
                    perror("read");
                } else if (readRes == 0) // abbiamo raggiunto la EOF
                {
                    printf("Ricevuto EOF\n");
                }
                printf("Figlio %i: chiudo connessione e termino\n", getpid());
                close(connfd);
                exit(0);
            } // figlio
            /* padre chiude la socket dell'operazione */
            close(connfd);
        } /* fine gestione richieste di file */

        /* GESTIONE RICHIESTE DI CONTEGGIO ------------------------------------------ */
        if (FD_ISSET(udpfd, &rset)) {
            printf("Server: ricevuta richiesta di verifica parola in file\n");
            len = sizeof(struct sockaddr_in);

            if (recvfrom(udpfd, &recv_dgram, sizeof(recv_dgram), 0, (struct sockaddr *)&cliaddr, &len) < 0) {
                perror("recvfrom");
                continue;
            }

            /*
             * Controlla che almeno una riga del file contenga la
             * parola cercata e restituisce l'esito del controllo.
             */
            int fd, ris = -1;
            printf("Nome file: %s\n", recv_dgram.filename);
            printf("Parola: %s\n", recv_dgram.parola);

            fd = open(recv_dgram.filename, O_RDONLY);
            if (fd < 0) {
                printf("File inesistente\n");
                ris = -2;
            } else {
                while (leggiLinea(fd, linea) > 0) {
                    if (strstr(linea, recv_dgram.parola)) {
                        ris = 0;
                        break;
                    }
                }

                close(fd);
            }
            printf("Risultato del controllo: %d\n", ris);

            // Invio risultato al client
            if (sendto(udpfd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                perror("sendto");
                continue;
            }
        } /* fine gestione */
    }     /* ciclo for della select */
}