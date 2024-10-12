/* Server che fornisce la valutazione di un'operazione tra due interi */
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
#include <unistd.h>

//invocation: LongestWordServer port

/********************************************************/
void gestore(int signo) {
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv) {
    int fd, sd, port, len, num1;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    struct hostent *clienthost;
    char nomeFile[FILENAME_MAX + 1];    //richiesta
    int risposta = -1;                  //risposta
    int lunghezzaTemp=0, nread;
    char buff;
    
    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2) {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    } else {
        num1 = 0;
        while (argv[1][num1] != '\0') {
            if ((argv[1][num1] < '0') || (argv[1][num1] > '9')) {
                printf("Secondo argomento non intero\n");
                printf("Error: %s port\n", argv[0]);
                exit(2);
            }
            num1++;
        }
        port = atoi(argv[1]);
        if (port < 1024 || port > 65535) {
            printf("Error: %s port\n", argv[0]);
            printf("1024 <= port <= 65535\n");
            exit(2);
        }
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER ---------------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

    /* CREAZIONE, SETAGGIO OPZIONI E CONNESSIONE SOCKET -------------------- */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("creazione socket ");
        exit(1);
    }
    printf("Server: creata la socket, sd=%d\n", sd);

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("set opzioni socket ");
        exit(1);
    }
    printf("Server: set opzioni socket ok\n");

    if (bind(sd, (struct sockaddr_in *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind socket ");
        exit(1);
    }
    printf("Server: bind socket ok\n");
    
    signal(SIGCHLD, gestore);

    /* CICLO DI RICEZIONE RICHIESTE ------------------------------------------ */
    for (;;) {
        len = sizeof(struct sockaddr_in);
        if (recvfrom(sd, nomeFile, sizeof(nomeFile), 0, (struct sockaddr_in *)&cliaddr, &len) < 0) {
            perror("recvfrom ");
            continue;
        }
        
        if (fork()==0) /* figlio */{
            printf("File richiesto: %s\n", nomeFile);
            //stampo informazioni del cliente se disponibili
            clienthost = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
            if (clienthost == NULL)
                printf("client host information not found\n");
            else
                printf("Operazione richiesta da: %s %i\n", clienthost->h_name,
                    (unsigned)ntohs(cliaddr.sin_port));

            /*EXEC*/ 
            
            //provo ad aprire il file
            if( (fd=open(nomeFile, O_RDONLY)) < 0 ) {
                perror("open");
                
                //mando un esito negativo
                risposta=-1;
                if (sendto(sd, &risposta, sizeof(risposta), 0, (struct sockaddr_in *)&cliaddr, len) < 0) {
                    perror("sendto ");
                }
                
                continue;   // non riesco ad aprire il file e quindi passo alla prossima richiesta
            }
            
            printf("contenuto del file: \n");
            //cerco la parola più lunga
            lunghezzaTemp=0;
            while( (nread=read(fd, &buff, 1)) > 0 ) {
                
                //TODO: controlla se nread = -1 e quindi ritorna un esito negativo
                write(1, &buff, nread);
                
                if( buff!=' ' && buff!='\n' && buff!='\t' ) {
                    lunghezzaTemp++;
                }
                else {
                    if( risposta < lunghezzaTemp ) {
                        risposta=lunghezzaTemp;
                    }
                    
                    lunghezzaTemp=0;
                }
            }
            
            close(fd);  
        
            //mando la risposta
            if (sendto(sd, &risposta, sizeof(risposta), 0, (struct sockaddr_in *)&cliaddr, len) < 0) {
                perror("sendto ");
                continue;
            }
            
            close(sd);
            exit(0);
        } // if
    } // for
}
