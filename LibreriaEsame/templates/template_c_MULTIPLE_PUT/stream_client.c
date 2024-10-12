#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define FILE_NAME_LENGTH 256
#define LINE_LENGTH 256

int main(int argc, char *argv[]) {
    struct hostent    *host;
    struct sockaddr_in servaddr;
    int port, nread, sd, nwrite;

    DIR           *dir_handle;
    struct dirent *current_file;

    char dir[FILE_NAME_LENGTH + 1];
    char buffer[LINE_LENGTH];
    char nomeFile[FILE_NAME_LENGTH + 1];   
    char path_completo_file[LINE_LENGTH];
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



    /* CORPO DEL CLIENT: ciclo di accettazione di richieste da utente  */
    printf("Inserire il nome della directory da trasferire: ");
    while (gets(dir)) {
        printf("\ttrasferisco: %s\n", dir);

        if ((dir_handle = opendir(dir)) != NULL) {
            // direttorio presente

            //scorro tutti i file della cartella 
            while ((current_file = readdir(dir_handle)) != NULL) {
                //creo il path completo del file
                sprintf(path_completo_file, "%s/%s", dir, current_file->d_name);

                //non considero i sottodirettori
                if ( opendir(path_completo_file) == NULL ) {    
                    //NB: non esploro i sottodirettori

                    //invio lunghezza del nome
                    len = strlen(current_file->d_name) + 1;

                    if (write(sd, &len, sizeof(int)) < 0) {
                        perror("Errore nell'invio della lunghezza del nome del file\n");
                        //passo al prossimo file
                        continue;
                    }
                    //invio nome                        
                    if (write(sd, current_file->d_name, len) < 0) {
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
                    if (write(sd, &len, sizeof(int)) < 0) {
                        perror("Errore nell'invio della lunghezza del nome del file\n");
                        //passo al prossimo file
                        continue;
                    }

                    //invio il file vero e proprio
                    while ( (nread = read(fd, buffer, sizeof(buffer))) > 0 ) {
                        if (write(sd, buffer, nread) < 0) {
                            perror("Errore nel trasferimento del file\n");
                            //smetto di leggere il file
                            break;
                        }
                    }

                    printf("\tinvio di %s terminato\n", current_file->d_name);
                }   
                else{
                    printf("\tsottodirettorio %s saltato\n", current_file->d_name);
                }  
            }      
        }
        else { 
            printf("direttorio inesistente, riprova\n");
        }

        printf("Inserire il nome della directory da trasferire: ");
    }
    /* Chiusura socket */
    close(sd);
    printf("\nClient: termino...\n");

    exit(0);
}