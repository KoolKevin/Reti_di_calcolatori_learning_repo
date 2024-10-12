#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_STRING_LENGTH 256

// produttore.c NON e' un filtro
int main(int argc, char *argv[]) {
    int   fd[argc], indiceFile, written, i, carattere;
    char  riga[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];

    // controllo numero argomenti
    if (argc < 2) {
        perror(" numero di argomenti sbagliato");
        exit(1);
    }
    
    for(i=1; i<argc; i++) {
	fd[i] = open(argv[i], O_WRONLY | O_CREAT | O_TRUNC, 00640);

	if (fd < 0) {
	    perror("P0: Impossibile creare/aprire il file");
	    exit(2);
	}
    }
	
    printf("inserisci le righe [FORMATO -> numFile:riga]!\n");	
    // sarebbe comodo usare la scanf dato che supporta l'input con formato ma dopo la riga non può avere spazi :(
    // di conseguenza uso una getchar()
	
    // IPOTESI SEMPLIFICATIVA: il produttore non produce mai più di 9 file alla volta -> l'indice del file è composto da un solo carattere;
    // altrimenti dovrei fare un parsing della riga più articolato controllando quando trovo il separatore e salvandomi
    // ogni carattere del numero del file uno alla volta
    
    int contatoreLinea = 0;
    int fineLineaTrovato = 0;
    int c=-1;
    
    while ( (carattere = getchar()) != EOF) {	
    	contatoreLinea++;
    	//controllo il carattere di indice file
    	if( contatoreLinea==1  ) { 
    	    indiceFile = carattere-'0'; //sottraendo '0' converto il carattere in un int
    	    //printf("indice file: %d\n", indiceFile);
    	    if( indiceFile>argc-1 || indiceFile<1 ){	
    	    	//errore di formato
		perror(" formato della riga sbagliato, inserisci un numero di file corretto. Riprova:");
		do {c=getchar();} while (c!= '\n');	//consumo la riga sbagliata
		contatoreLinea=0;
		continue;	//
    	    }
    	}
    	else{
    	    //controllo la riga
    	    if( contatoreLinea==2 ) {
    	    	//butto via il separatore
    	    	continue;
    	    }
	    else{
	    	//riga effettiva
	    	written = write(fd[indiceFile], &carattere, 1); // uso della primitiva
		
	        if (written < 0) {
	           perror("P0: errore nella scrittura sul file");
                   exit(4);
	        }
	    	
	    }
	    
	    //dopo fine lina resetto il contatore
	    if( carattere == '\n') {
	    	contatoreLinea=0;
	    }    
	}
    }
    
    for(i=1; i<argc; i++) {
	close(fd[i]);
    }
}
