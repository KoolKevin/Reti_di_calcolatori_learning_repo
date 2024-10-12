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
#define LENGTH_FILE_NAME 50
#define max(a, b)        ((a) > (b) ? (a) : (b))

typedef struct {
    char nomeFile[LENGTH_FILE_NAME];
    char parola[LENGTH_FILE_NAME];
} DatagramRequest;

/********************************************************/
int conta_file(char *name) {
    DIR           *dir;
    struct dirent *dd;
    int            count = 0;
    dir                  = opendir(name);
    if (dir == NULL)
        return -1;
    while ((dd = readdir(dir)) != NULL) {
        printf("Trovato il file %s\n", dd->d_name);
        count++;
    }
    printf("Numero totale di file %d\n", count);
    closedir(dir);
    return count;
}
/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv) {
    int       listenfd, connfd, udpfd, fd_temp_udp, fd_file, nready, maxfdp1;
    const int on   = 1;
    char      buff[DIM_BUFF], charBuff, word_buffer[DIM_BUFF], dir[LENGTH_FILE_NAME], fileNameTemp[LENGTH_FILE_NAME];
    char err[128], ris, newDir[LENGTH_FILE_NAME];
    fd_set    rset;
    int       len, nread, nwrite, port;
    struct sockaddr_in cliaddr, servaddr;
    DatagramRequest datagramReq;
    DIR           *dir1, *dir2, *dir3;
    struct dirent *dd1, *dd2;

    
    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    }
    nread = 0;
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

                //sessione client
                while ( read(connfd, &dir, sizeof(dir)) > 0 ) {   //leggo il nome della directory
                   
                    printf("Server (figlio): direttorio richiesto: %s\n", dir);

                    //ottengo l'handle alla directory
                    if ((dir1 = opendir(dir)) != NULL) { // direttorio presente 
                        ris='S';
                        write(connfd, &ris, sizeof(char));
                        
                        //calcolo ed invio dimensione directory

                        while ((dd1 = readdir(dir1)) != NULL) { //scorro i file della directory passata
                            sprintf(newDir, "%s/%s", dir, dd1->d_name);
                            //printf("newdir: %s\n", newDir);

                            if ((dir2 = opendir(newDir)) == NULL) { //se non riesco ad aprire vuol dire che è un file

                                //invio dimensione file
                                printf("Invio nome: %s\n", dd1->d_name);
                                if (write(connfd, dd1->d_name, (strlen(dd1->d_name) + 1)) < 0)
                                {
                                    perror("Errore nell'invio del nome file\n");
                                    continue;
                                }
                            }
                        }

                        ris = '\1';     
                        write(connfd, &ris, sizeof(char));  //scrivo il carattere di fine direttorio
                    }
                    else{
                        ris='N';
                        printf("Invio risposta negativa al client per dir %s \n", dir);
                        write(connfd, &ris, sizeof(char));
                    }

                }     // for

                printf("Figlio %i: chiudo connessione e termino\n", getpid());
                close(connfd);
                exit(0);
            } // figlio
            /* padre chiude la socket dell'operazione */
            close(connfd);
        } /* fine gestione richieste di file */

        /* GESTIONE RICHIESTE DI CONTEGGIO ------------------------------------------ */

        if (FD_ISSET(udpfd, &rset)) {
            printf("Ricevuta richiesta di UDP: eliminazione di una parola\n");
            len = sizeof(struct sockaddr_in);

            if (recvfrom(udpfd, &datagramReq, sizeof(datagramReq), 0, (struct sockaddr *)&cliaddr, &len) < 0) {
                perror("recvfrom ");
                continue;
            }

            printf("Operazione richiesta sul file: %s per la parola: %s\n", datagramReq.nomeFile, datagramReq.parola);

            /* Verifico file */
            int ris = 0;
            int count_letters = 0;
            if ((fd_file = open(datagramReq.nomeFile, O_RDONLY)) < 0) {
                perror("Errore apertura file sorgente");
                ris = -1;
            }

            /* Se il file esiste */
            if (ris == 0) {
                /* Creo il file temporaneo */
                fileNameTemp[0] = '\0';
                strcat(fileNameTemp, datagramReq.nomeFile);
                strcat(fileNameTemp, "_temp");
                if ((fd_temp_udp = open(fileNameTemp, O_CREAT | O_WRONLY, 0777)) < 0) {
                    perror("Errore apertura file");
                    ris = -1;
                } else { // Se il file temporaneo è stato creato correttamente
                    /* Eliminazione della parola */

                    count_letters = 0;
                    while ((nread = read(fd_file, &charBuff, sizeof(char))) != 0) {
                        if (nread < 0) {
                            sprintf(err, "(PID %d) impossibile leggere dal file", getpid());
                            perror(err);
                            ris = -1;
                            break;
                        } else {
                            // La presenza di un separatore indica la fine di una parola. Altri
                            // separatori? Punteggiatura? Sarebbe meglio avere un array di
                            // separatori, magari letto da un file di configurazione all'avvio del
                            // server.
                            if (charBuff == ' ' || charBuff == '\r' || charBuff == '\n') {
                                word_buffer[count_letters] = '\0';
                                if (strcmp(word_buffer, datagramReq.parola)) { // parola diversa
                                    write(fd_temp_udp, &word_buffer, strlen(word_buffer));
                                } else {
                                    ris++;
                                }
                                count_letters = 0;
                                // Il separatore non fa parte della parola: lo dobbiamo lasciare in
                                // ogni caso!
                                write(fd_temp_udp, &charBuff, 1);
                            } else { // carattere
                                word_buffer[count_letters] = charBuff;
                                count_letters++;
                            }
                        }
                    }
                }
            }
            if (ris >= 0) {
                printf("Nel file %s sono state eliminate le parole uguali a %s\n", datagramReq.nomeFile, datagramReq.parola);
            }

            close(fd_file);
            close(fd_temp_udp);

            // Rinomino il file temporaneo, sovrascrivendo il file originale
            rename(fileNameTemp, datagramReq.nomeFile);

            // Send result to the client
            if (sendto(udpfd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                perror("sendto ");
                continue;
            }
        } /* fine gestione richieste di conteggio */
    }     /* ciclo for della select */
}
