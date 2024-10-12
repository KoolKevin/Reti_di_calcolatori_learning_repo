#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX_STRING_LENGTH 256

// consumatore.c e' un filtro
int main(int argc, char *argv[]) {

    char *file_in, *caratteriDaTogliere, read_char, buf[MAX_STRING_LENGTH];
    int   nread, fd;
    int valido = 1;

    // controllo numero argomenti
    if (argc < 2 || argc > 3) {
        perror(" numero di argomenti sbagliato");
        exit(1);
    }

    caratteriDaTogliere = argv[1];
    
    
    if(argc == 3) {
        file_in = argv[2];
        fd = open(file_in, O_RDONLY);
    }
    else if(argc == 2) 
	fd = 0;

    if (fd < 0) {
        perror("P0: Impossibile aprire il file.");
        exit(2);
    }
    

    while (nread = read(fd, &read_char, sizeof(char))) /* Fino ad EOF*/ {
        if (nread > 0) {
            for(int i=0; i<strlen(caratteriDaTogliere); i++)  {
                if(read_char==caratteriDaTogliere[i]) 
                    valido=0;
            }
            
            if(valido==1)
                putchar(read_char);
        }
        else {
            printf("(PID %d) impossibile leggere dal file %s", getpid(), file_in);
            perror("Errore in lettura");
            close(fd);
            exit(3);
        }
        
        valido=1;
    }
    close(fd);
}
