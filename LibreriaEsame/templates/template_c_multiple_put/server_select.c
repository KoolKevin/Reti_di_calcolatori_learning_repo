#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINE_LENGTH 256
#define WORD_LENGTH 64

#define TARGA_LENGTH 7
#define PATENTE_LENGTH 5
#define MAX_PRENOTAZIONI 10

#define max(a, b)   ((a) > (b) ? (a) : (b))

/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/
typedef struct {
    char targa[TARGA_LENGTH + 1];
    char patente[PATENTE_LENGTH + 1];
    char tipo_veicolo[WORD_LENGTH + 1];
    char dir_img[WORD_LENGTH + 1];
} Prenotazione;

typedef struct {
    char targa[TARGA_LENGTH + 1];
    char nuova_patente[PATENTE_LENGTH + 1];
} udp_req_t;
/********************************************************/


/* STRUTTURA DATI GLOBALE */
static Prenotazione struttura_prenotazioni[MAX_PRENOTAZIONI]; 


int main(int argc, char **argv) {
    struct sockaddr_in cliaddr, servaddr;
    struct hostent    *hostTcp, *hostUdp;
    int                port, listen_sd, conn_sd, udp_sd, nread, maxfdp1, len;
    const int          on = 1;
    fd_set             rset;

    udp_req_t          req;

    /************ VARIABILI DA MODIFICARE IN BASE ALL'ES ************/
    int fd;
    char nome_file[WORD_LENGTH], dir[LINE_LENGTH];
    char char_buf;
    int contaLetti;
    
    int esito;
    int error;
    
    /****************************************************************/

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port \n", argv[0]);
        exit(1);
    } else {
        nread = 0;
        while (argv[1][nread] != '\0') {
            if ((argv[1][nread] < '0') || (argv[1][nread] > '9')) {
                printf("Secondo argomento non intero\n");
                exit(2);
            }
            nread++;
        }
        port = atoi(argv[1]);
        if (port < 1024 || port > 65535) {
            printf("Porta scorretta...");
            exit(2);
        }
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ----------------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

    /* CREAZIONE E SETTAGGI SOCKET TCP --------------------------------------- */
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0) {
        perror("creazione socket");
        exit(3);
    }
    printf("Server: creata la socket d'ascolto per le richieste di ordinamento, fd=%d\n", listen_sd);

    if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket d'ascolto");
        exit(3);
    }
    printf("Server: set opzioni socket d'ascolto ok\n");

    if (bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket d'ascolto");
        exit(3);
    }
    printf("Server: bind socket d'ascolto ok\n");

    if (listen(listen_sd, 5) < 0) {
        perror("listen");
        exit(3);
    }
    printf("Server: listen ok\n");

    /* CREAZIONE E SETTAGGI SOCKET UDP --------------------------------------- */
    udp_sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sd < 0) {
        perror("apertura socket UDP");
        exit(4);
    }
    printf("Creata la socket UDP, fd=%d\n", udp_sd);

    if (setsockopt(udp_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket UDP");
        exit(4);
    }
    printf("Set opzioni socket UDP ok\n");

    if (bind(udp_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket UDP");
        exit(4);
    }
    printf("Bind socket UDP ok\n");

    //gestore per evitare figli zombie
    signal(SIGCHLD, gestore);

    /* PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR ------------------------- */
    FD_ZERO(&rset);
    maxfdp1 = max(listen_sd, udp_sd) + 1;

    /* INIZIALIZZAZIONE STRUTTURA DATI */
    for(int i=0; i<MAX_PRENOTAZIONI; i++) {
        strcpy(struttura_prenotazioni[i].targa, "L");
        strcpy(struttura_prenotazioni[i].patente, "L");
        strcpy(struttura_prenotazioni[i].tipo_veicolo, "L");
        strcpy(struttura_prenotazioni[i].dir_img, "L");
    }

    strcpy(struttura_prenotazioni[0].targa, "AN745NL");
    strcpy(struttura_prenotazioni[0].patente, "00003");
    strcpy(struttura_prenotazioni[0].tipo_veicolo, "auto");
    strcpy(struttura_prenotazioni[0].dir_img, "AN745NL_img");

    printf("--------------- STRUTTURA DATI INIZIALE ----------------\n");
    printf("TARGA\t\tPATENTE\t\tTIPO\t\tDIR\n");
    for(int i=0; i<MAX_PRENOTAZIONI; i++) {
        printf("%s\t\t", struttura_prenotazioni[i].targa);
        printf("%s\t\t", struttura_prenotazioni[i].patente);
        printf("%s\t\t", struttura_prenotazioni[i].tipo_veicolo);
        printf("%s\n", struttura_prenotazioni[i].dir_img);
    }

    /* CICLO DI RICEZIONE RICHIESTE --------------------------------------------- */
    for (;;) {
        FD_SET(listen_sd, &rset);
        FD_SET(udp_sd, &rset);

        if ((nread = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("select");
                exit(5);
            }
        }
        /* GESTIONE RICHIESTE UDP  ----------------------------- */
        if (FD_ISSET(udp_sd, &rset)) {
            printf("------------------------ Ricevuta richiesta di UDP: aggiornamento numero di patente ---------------------------\n");
            len = sizeof(struct sockaddr_in);

            //ricezione richiesta
            if (recvfrom(udp_sd, &req, sizeof(req), 0, (struct sockaddr *)&cliaddr, &len) < 0) {
                perror("recvfrom ");
                continue;
            }

            printf("richiesta: targa=%s; patente=%s\n", req.targa, req.nuova_patente);

            /* Stampo a video le informazioni sul client */
            hostUdp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (hostUdp == NULL) {
                printf("Operazione richiesta da: %s\n", inet_ntoa(cliaddr.sin_addr));
            } else {
                printf("Operazione richiesta da: %s %i\n", hostUdp->h_name, (unsigned)ntohs(cliaddr.sin_port));
            }

            /************************ ELABORAZIONE RICHIESTA**************************/

            /* Verifico file */
            int ris = -1;

            for(int i=0; i<MAX_PRENOTAZIONI; i++) {
                if( strcmp(req.targa, struttura_prenotazioni[i].targa) == 0 ) {
                    ris = 0;

                    //aggiorno la entry
                    strcpy(struttura_prenotazioni[i].patente, req.nuova_patente);

                    break;
                }
            }
            
            printf("--------------- STRUTTURA DATI DOPO L'aggiornamento ----------------\n");
            printf("TARGA\t\tPATENTE\t\tTIPO\t\tDIR\n");
            for(int i=0; i<MAX_PRENOTAZIONI; i++) {
                printf("%s\t\t", struttura_prenotazioni[i].targa);
                printf("%s\t\t", struttura_prenotazioni[i].patente);
                printf("%s\t\t", struttura_prenotazioni[i].tipo_veicolo);
                printf("%s\n", struttura_prenotazioni[i].dir_img);
            }


            if (ris < 0) {
                printf("Targa non trovata!\n");
            }
            else{
                printf("aggiornamento avvenuta con successo!\n");
            }

            /*******************************************************************************/

            // invio risposta
            if (sendto(udp_sd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                perror("sendto");
                continue;
            }
        }
        /* GESTIONE RICHIESTE TCP  ----------------------------- */
        if (FD_ISSET(listen_sd, &rset)) {
            printf("------------------ Ricevuta richiesta di PUT di direttorio! ------------------\n");

            //accetto la connessione
            len = sizeof(cliaddr);
            if ((conn_sd = accept(listen_sd, (struct sockaddr *)&cliaddr, &len)) < 0) {
                if (errno == EINTR) {
                    perror("Forzo la continuazione della accept");
                    continue;
                } else {
                    perror("Error in accept: ");
                    exit(6);
                }
            }

            //delego ad un figlio
            if (fork() == 0) {
                close(listen_sd);

                // Stampo a video le informazioni sul client
                hostTcp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
                if (hostTcp == NULL) {
                    printf("Server (figlio): host client e' %s\n", inet_ntoa(cliaddr.sin_addr));
                } else {
                    printf("Server (figlio): host client e' %s \n", hostTcp->h_name);
                }  

                error = 0;
                // Ricezione dei file
                while ((nread = read(conn_sd, &len, sizeof(int))) > 0) {
                    if (len == -1) {
                        //ho finito di leggere il file!
                        
                        // Esco dal ciclo di gestione di questo file
                        // senza però chiudere la socket.
                        break;
                    }
                    else {
                        if (read(conn_sd, nome_file, len) < 0) {
                            error = 1;
                            perror("read nomefile");
                            //passo alla prossima ricezione
                            continue;
                        }

                        // Scrivo a video
                        printf("\tricevo %s\n", nome_file);
                        //creazione file
                        if ((fd = open(nome_file, O_WRONLY | O_CREAT, 0644)) < 0) {
                            error=1;
                            perror("open file");
                            //passo alla prossima ricezione
                            continue;
                        }

                        //leggo la lunghezza del file
                        if (read(conn_sd, &len, sizeof(int)) < 0) {
                            error = 1;
                            perror("read lunghezza file");
                            //passo alla prossima ricezione
                            continue;
                        }
                        printf("\tricevo %d byte\n", len);

                        contaLetti = 0;
                        while( contaLetti < len ) {
                            //leggo un carattere alla volta per evitare di strabordare in un altro file nella lettura dello stream
                            read(conn_sd, &char_buf, sizeof(char));
                            write(fd, &char_buf, sizeof(char));
                            contaLetti++;
                        }

                        close(fd);
                        printf("Trasferimento di %s completato!\n", nome_file);
                    }
                }

                /*******************************************************************************/

                // Libero risorse
                printf("Figlio TCP terminato, libero risorse e chiudo. \n");
                close(conn_sd);
                exit(0);
            }               // if fork

            // padre
            close(conn_sd); 
        }                   // if socket_stream
    }                       // for
} // main
