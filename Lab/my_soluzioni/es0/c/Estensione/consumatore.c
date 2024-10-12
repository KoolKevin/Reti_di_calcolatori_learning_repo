#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX_STRING_LENGTH 256

// consumatore.c e' un filtro
int main(int argc, char *argv[]) {

    char *caratteriDaTogliere, read_char, buf[MAX_STRING_LENGTH], nomeFileTemp[MAX_STRING_LENGTH]={'T', 'E', 'M', 'P', '.','\0'}, estensioneFileTemp[MAX_STRING_LENGTH];
    int nread, fd, fdTemp, i, pid, written;
    int valido = 1;
    int numFile = argc - 2;
	
    // controllo numero argomenti
    if (argc < 3) {
        perror(" numero di argomenti sbagliato");
        exit(1);
    }

    caratteriDaTogliere = argv[1];
	
    for ( i=0; i<numFile; i++ ) {
        pid = fork();
        
        if( pid == 0 ) { // Eseguito dai figli
            //printf("sono il figlio %d e mi occupo del file %s\n", i+1, argv[i+2]);
            fd = open(argv[i+2], O_RDONLY);
            
            //costruisco il nome completo del file temporaneo e lo creo
            sprintf(estensioneFileTemp,"%d", i+1);
            strcat(nomeFileTemp, estensioneFileTemp);
            fdTemp = open(nomeFileTemp, O_WRONLY | O_CREAT | O_TRUNC, 00640);
            
	    if (fd < 0) {
	        perror("P0: Impossibile creare/aprire il file");
	        exit(2);
	     }
	     
	     while (nread = read(fd, &read_char, sizeof(char))) /* Fino ad EOF*/ {
                 if (nread > 0) {
                     //controllo se il carattere letto dal file è valido -> per ogni carattere devo scorrere l'array dei caratteri da togliere
                     for(int i=0; i<strlen(caratteriDaTogliere); i++)  {
                         if(read_char==caratteriDaTogliere[i]) 
                              valido=0;
                     }
            
                     if(valido==1) {
                         written = write(fdTemp, &read_char, 1); // uso della primitiva
		
			  if (written < 0) {
			     perror("P0: errore nella scrittura sul file");
			     exit(3);
			  }
                     }
                 }
                 else {
                    printf("(PID %d) impossibile leggere dal file %s", getpid(), argv[i+2]);
                    perror("Errore in lettura");
                    close(fd);
                    exit(4);
                 }
        
                 valido=1;
              }
    
            close(fd);
            close(fdTemp);
            
            //rinomino il file temporaneo in modo da sostituire quello reale
            rename(nomeFileTemp, argv[i+2]);
        }
        else if( pid < 0 ){
            perror("Fork error:");
            exit(1);
        }
    }

}
