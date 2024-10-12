#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_STRING_LENGTH 256

// produttore.c NON e' un filtro
int main(int argc, char *argv[]) {
    int   fd, readValues, bytes_to_write, written, righe, i;
    char *file_out;
    char  riga[MAX_STRING_LENGTH], buf;

    // controllo numero argomenti
    if (argc != 2) {
        perror(" numero di argomenti sbagliato");
        exit(1);
    }

    file_out = argv[1];
    printf("inserisci tutte le righe che vuoi!\n");


    fd = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 00640);
    if (fd < 0) {
        perror("P0: Impossibile creare/aprire il file");
        exit(2);
    }

    printf("Inserisci la nuova riga\n");
    
    while(read(0, &buf, 1) > 0) {
        written = write(fd, &buf, 1); // uso della primitiva
        if (written < 0) {
           perror("P0: errore nella scrittura sul file");
            exit(3);
        }
    }
    
    close(fd);
}
