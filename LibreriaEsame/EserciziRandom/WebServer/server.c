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
#define max(a, b)   ((a) > (b) ? (a) : (b))

struct http_response_header{
    char status_line[LINE_LENGTH];
    char content_type[WORD_LENGTH];
    char content_length[WORD_LENGTH];
};

/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}

int main(int argc, char **argv) {
    struct sockaddr_in cliaddr, servaddr;
    struct hostent    *hostTcp;
    int                port, listen_sd, conn_sd, nread, len;
    const int          on = 1;

    /************ VARIABILI DA MODIFICARE IN BASE ALL'ES ************/
    char request_buffer[1024];   
    char * response_body;     
    char response_message[4096];
    struct http_response_header response_header;

    DIR           *dir1, *dir2, *dir3;
    struct dirent *dd1, *dd2;
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

    //gestore per evitare figli zombie
    signal(SIGCHLD, gestore);

    /* CICLO DI RICEZIONE RICHIESTE --------------------------------------------- */
    for (;;) {
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

        //delego ad un figlio la risposta
        if (fork() == 0) {
            close(listen_sd);

            // Stampo a video le informazioni sul client
            hostTcp = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (hostTcp == NULL) {
                printf("Server (figlio): host client e' %s\n", inet_ntoa(cliaddr.sin_addr));
            } else {
                printf("Server (figlio): host client e' %s \n", hostTcp->h_name);
            }

            //inizializzo il buffer
            memset(request_buffer, 0, sizeof(request_buffer));
            // Leggo la richiesta del client
            /* NB: bisogna stare attenti a dimensionare il buffer di ricezione della richiesta, qua io uso 1kb penso vada bene */
            while ((nread = read(conn_sd, request_buffer, sizeof(request_buffer))) > 0) {
                printf("----------------------------- NUOVA RICHIESTA! ---------------------------------\n\n");
                write(1, &request_buffer, sizeof(request_buffer));
                
                //messaggio di risposta;
                response_body = "aspetta\r\n";
                // Inizializzazione degli header
                memset(&response_header, 0, sizeof(response_header));
                strcpy(response_header.status_line, "HTTP/1.1 200 OK\r\n");
                strcpy(response_header.content_type, "Content-Type: text/html\r\n");
                // Calcolo e impostazione del Content-Length
                char content_length_header[64];
                int content_length = strlen(response_body); // Lunghezza del corpo escludendo il \r\n iniziale
                sprintf(content_length_header, "Content-Length: %d\r\n", content_length);
                strcpy(response_header.content_length, content_length_header);

                // Invio degli header e del corpo
                sprintf(response_message, "%s%s%s", response_header.status_line, response_header.content_type, response_header.content_length);
                sprintf(response_message, "%s\r\n%s", response_message, response_body);

                if (write(conn_sd, response_message, strlen(response_message)) < 0) {
                    perror("Errore nell'invio della risposta\n");
                    continue;
                }
                
                /************************ ELABORAZIONE RICHIESTA**************************/

                // char risp;
                // if ((dir1 = opendir(dir)) != NULL) { // direttorio presente
                //     risp = 'S';
                //     printf("Invio risposta affermativa al client\n");
                //     write(conn_sd, &risp, sizeof(char));

                //     //scorro tutti i file della cartella di primo livello
                //     while ((dd1 = readdir(dir1)) != NULL) {
                //         if (strcmp(dd1->d_name, ".") != 0 && strcmp(dd1->d_name, "..") != 0) {
                //             // build new path
                //             sprintf(newDir, "%s/%s", dir, dd1->d_name);
                //             if ((dir2 = opendir(newDir)) != NULL) { 
                //                 // scorro tutti i file delle directory di secondo livello
                //                 while ((dd2 = readdir(dir2)) != NULL) {
                //                     if (strcmp(dd2->d_name, ".") != 0 && strcmp(dd2->d_name, "..") != 0) {
                //                         // build new path
                //                         strcat(newDir, "/");
                //                         strcat(newDir, dd2->d_name);
                //                         // se non è un direttorio, è un file da considerare
                //                         if ((dir3 = opendir(newDir)) == NULL) {
                //                             // file of sec livello
                //                             // Il nome del file viene inviato subito, appena
                //                             // scoperto. Sarebbe un errore creare una grande
                //                             // stringa con tutti i nomi e inviarla solo alla
                //                             // fine!
                //                             printf("Invio nome: %s\n", dd2->d_name);
                //                             if (write(conn_sd, dd2->d_name, (strlen(dd2->d_name) + 1)) < 0)
                //                             {
                //                                 perror("Errore nell'invio del nome file\n");
                //                                 continue;
                //                             }
                //                         } 
                //                     }     
                //                 }       
                //                 printf("Fine invio\n");
                //             } 
                //         }     
                //     }      
                //     //invio separatore risposte   
                //     risp = '\1';
                //     write(conn_sd, &risp, sizeof(char));
                //}
                // else { 
                //     risp = 'N';
                //     printf("Invio risposta negativa al client per dir %s \n", dir);
                //     write(conn_sd, &risp, sizeof(char));
                // }
            } // while read req


            /*******************************************************************************/


            // Libero risorse
            printf("Figlio TCP terminato, libero risorse e chiudo. \n");
            close(conn_sd);
            exit(0);
        }               // if fork

        // padre
        close(conn_sd); 
    }                       // for
} // main
