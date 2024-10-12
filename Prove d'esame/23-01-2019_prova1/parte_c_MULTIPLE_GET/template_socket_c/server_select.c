#include <arpa/inet.h>
#include <dirent.h>
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
    char targa[WORD_LENGTH], dir[LINE_LENGTH];
    char path_completo_file[LINE_LENGTH];
    char buffer[LINE_LENGTH];
    DIR           *dir_handle;
    struct dirent *current_file;
    int esito;
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
            printf("------------------ Ricevuta richiesta di GET di immagini! ------------------\n");

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

                // Leggo la richiesta del client
                while ((nread = read(conn_sd, targa, sizeof(targa))) > 0) {
                    sprintf(dir, "%s_img", targa);
                    printf("\tServer (figlio): direttorio richiesto: %s\n", dir);
                    
                    char risp;
                    if ((dir_handle = opendir(dir)) != NULL) {
                        // direttorio presente

                        //scorro tutti i file della cartella 
                        while ((current_file = readdir(dir_handle)) != NULL) {
                            //non considero i direttori '.' e '..'
                            if (strcmp(current_file->d_name, ".") != 0 && strcmp(current_file->d_name, "..") != 0) {    
                                //NB: assumo che nella directory ci siano solo immagini e non altre dirs

                                //invio lunghezza del nome
                                len = strlen(current_file->d_name) + 1;

                                if (write(conn_sd, &len, sizeof(int)) < 0) {
                                    perror("Errore nell'invio della lunghezza del nome del file\n");
                                    //passo al prossimo file
                                    continue;
                                }
                                //invio nome                        
                                if (write(conn_sd, current_file->d_name, len) < 0) {
                                    perror("Errore nell'invio del nome file\n");
                                    //passo al prossimo file
                                    continue;
                                }
                                
                                printf("\tho inviato il nome di %s, lunghezza: %d\n", current_file->d_name, len);

                                //apro il file e ne calcolo la dimensione
                                
                                //creo il path completo del file
                                sprintf(path_completo_file, "%s/%s", dir, current_file->d_name);

                                if ((fd = open(path_completo_file, O_RDONLY)) < 0) {
                                    perror("Errore apertura file");
                                    //passo al prossimo file
                                    continue;
                                }

                                len = lseek(fd, 0, SEEK_END);
                                printf("\tdimensione: %d\n", len);
                                lseek(fd, 0, SEEK_SET);

                                //invio dimensione file
                                if (write(conn_sd, &len, sizeof(int)) < 0) {
                                    perror("Errore nell'invio della lunghezza del nome del file\n");
                                    //passo al prossimo file
                                    continue;
                                }

                                //invio il file vero e proprio
                                while ( (nread = read(fd, buffer, sizeof(buffer))) > 0 ) {
                                    if (write(conn_sd, buffer, nread) < 0) {
                                        perror("Errore nel trasferimento del file\n");
                                        //smetto di leggere il file
                                        break;
                                    }
                                }
                            }     
                        }      

                        // invio separatore risposte, se il client riceve un -1 come lunghezza del nome 
                        // capisce che si deve sbloccare e passare alla prossima richiesta
                        len = -1;
                        write(conn_sd, &len, sizeof(int));
                    }
                    else { 
                        printf("Invio risposta negativa al client per dir %s\n", dir);
                        len = -10;
                        write(conn_sd, &len, sizeof(int));
                    }
                } // while read req


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
