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

#define NUM_VOCALI 10

#define max(a, b)   ((a) > (b) ? (a) : (b))

char vocali[NUM_VOCALI] = {'A', 'a', 'E', 'e', 'I', 'i', 'O', 'o', 'U', 'u'};

int isVocale(char c) {
    for(int i=0; i<NUM_VOCALI; i++) {
        if( c == vocali[i] ) {
            return 0;
        }
    }

    return 1;
}

/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}

int main(int argc, char **argv) {
    struct sockaddr_in cliaddr, servaddr;
    struct hostent    *hostTcp, *hostUdp;
    int                port, listen_sd, conn_sd, udp_sd, nread, maxfdp1, len;
    const int          on = 1;
    fd_set             rset;

    /************ VARIABILI DA MODIFICARE IN BASE ALL'ES ************/
    int fd, fd_temp;
    //NB: qua non c'è bisogno di una struct per le richieste udp, comunque importante che entrambi abbiano la stessa dim però
    char file_name[LINE_LENGTH], dir_name[LINE_LENGTH], file_name_temp[LINE_LENGTH];       
    char path_completo_file[LINE_LENGTH];
    char buffer[LINE_LENGTH], char_buf;
    DIR           *dir_handle;
    struct dirent *current_file;
    int esito;
    int numVocali, numConsonanti;
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
    printf("Server: creata la socket stream, fd=%d\n", listen_sd);

    if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket stream");
        exit(3);
    }
    printf("Server: set opzioni socket stream ok\n");

    if (bind(listen_sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket d'ascolto");
        exit(3);
    }
    printf("Server: bind socket stream ok\n");

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
            printf("------------------------ Ricevuta richiesta UDP ---------------------------\n");
            len = sizeof(struct sockaddr_in);

            //ricezione richiesta
            if (recvfrom(udp_sd, &file_name, sizeof(file_name), 0, (struct sockaddr *)&cliaddr, &len) < 0) {
                perror("recvfrom ");
                continue;
            }

            printf("richiesta eliminazione delle vocali di: %s\n", file_name);

            //parallelo dato che non sappiamo quanto possa essere lungo il file, se fosse molto lungo bloccherebbe il servitore non potrebbe
            //servire richieste per tutto il tempo della elaborazione. Renderlo parallelo però crea il problema dell'accesso concorrente allo stesso file
            //in questo la write sul file temporaneo del processo che perde la corsa critica fallisce, poco male.
            if( fork() == 0 ) {
                esito = 0;

                /************************ ELABORAZIONE RICHIESTA**************************/

                if ((fd = open(file_name, O_RDONLY)) < 0) {
                    perror("Errore apertura file");
                    //passo alla prossima richiesta
                    esito = -1;
                    if (sendto(udp_sd, &esito, sizeof(esito), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                        perror("sendto");
                        continue;
                    }

                    continue;
                }
                
                //IPOTESI SEMPLIFICATIVA: suppongo che mi venga passato solo un nome di file e non un path che comprende anche directory
                //                        altrimenti dovrei fare parsing di quest'ultimo per capire dove creare il file temporaneo
                sprintf(file_name_temp, "%s_tmp", file_name);

                if ((fd_temp = open(file_name_temp, O_CREAT | O_WRONLY, 0644)) < 0) {
                    perror("Errore apertura file temp");
                    //passo alla prossima richiesta
                    esito = -1;
                    if (sendto(udp_sd, &esito, sizeof(esito), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                        perror("sendto");
                        continue;
                    }

                    continue;
                }
                
                while( (nread=read(fd, &char_buf, sizeof(char))) > 0 ) {
                    //questa funzione la chiamo solo qua, scriverla in linea risparmierebbe il costo 
                    //dell'allocazione sullo stack del record di attivazione della funzione
                    //ma renderebbe il codice altamente illeggibile.
                    if( isVocale(char_buf) != 0 ) {
                        if( write(fd_temp, &char_buf, sizeof(char)) < 0 ) {
                            perror("Errore scrittura file temp");
                            //passo alla prossima richiesta
                            esito = -1;
                            if (sendto(udp_sd, &esito, sizeof(esito), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                                perror("sendto");
                                continue;
                            }

                            continue;
                        }
                    }
                    else{
                        esito++;
                    }
                }

                if(nread<0) {
                    perror("Errore lettura file");
                    //passo alla prossima richiesta
                    esito = -1;
                    if (sendto(udp_sd, &esito, sizeof(esito), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                        perror("sendto");
                        continue;
                    }

                    continue;
                }

                close(fd);
                close(fd_temp);

                rename(file_name_temp, file_name);

                /*******************************************************************************/

                // invio risposta
                if (sendto(udp_sd, &esito, sizeof(esito), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                    perror("sendto");
                    continue;
                }

                exit(0);
            }
        }
        /* GESTIONE RICHIESTE TCP  ----------------------------- */
        if (FD_ISSET(listen_sd, &rset)) {
           

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

            printf("------------------ Ricevuta richiesta di GET di direttorio! ------------------\n");

            //delego ad un figlio
            if (fork() == 0) {
                close(listen_sd);

                // Leggo la richiesta del client
                while ((nread = read(conn_sd, dir_name, sizeof(dir_name))) > 0) {
                    numVocali = numConsonanti = 0;

                    for(int i=0; i < strlen(dir_name); i++) {
                        printf("\tcontrollo %c = %d\n", dir_name[i], isVocale(dir_name[i]) ); 

                        if( isVocale(dir_name[i]) == 0 ) {
                            numVocali++;
                        }
                        else{
                            numConsonanti++;
                        }
                    }

                    printf("\til nome passato ha %d vocali e %d consonanti\n", numVocali, numConsonanti);

                    //controllo se il direttorio è valido
                    if( numVocali >= 1 && numConsonanti >= 1 ) {
                        if ((dir_handle = opendir(dir_name)) != NULL) {
                            // direttorio presente

                            //scorro tutti i file della cartella 
                            while ((current_file = readdir(dir_handle)) != NULL) {
                                //creo il path completo del file
                                sprintf(path_completo_file, "%s/%s", dir_name, current_file->d_name);
                                
                                //non considero i direttori 
                                if( opendir(path_completo_file) == NULL ) {
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

                                    close(fd);
                                }
                                
                            }      

                            // invio separatore risposte, se il client riceve un -1 come lunghezza del nome 
                            // capisce che si deve sbloccare e passare alla prossima richiesta
                            len = -1;
                            write(conn_sd, &len, sizeof(int));
                        }
                        else { 
                            printf("Invio risposta negativa al client per dir %s\n", dir_name);
                            len = -10;
                            write(conn_sd, &len, sizeof(int));
                        }
                    }
                    else {
                        printf("directory invalida");
                        len = -20;
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
