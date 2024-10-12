#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include "file.h"

static char numeri[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}; 

/*
	Funzione che controlla se un carattere è numerico.
	Ritorna:
		0 -> carattere non numerico
		1 -> carattere numerico
*/
int isNumeric(char c) {
	for(int i=0; i<10; i++) {
		if(c == numeri[i]) {
			return 1;
		}
	}

	return 0;
}

/*
	Funzione che controlla se un filename terminia con '.txt'
	Ritorna:
		1 -> se il filename termina con .txt
		0 -> altrimenti
*/
int isTxt(char *filename) {
    int length = strlen(filename);

    char *extension = ".txt";
    int extLength = strlen(extension);

    if (length < extLength) {
		// Il nome del file è più corto dell'estensione ".txt"
        return 0; 
    }

    // Confronto gli ultimi 'extLength' caratteri del filename passato con l'estensione
    if (strcmp(filename + length - extLength, extension) == 0) {
        return 1; 
    } else {
        return 0;
    }
}


int * elimina_occorrenze_1_svc(nome_file *filename, struct svc_req *rqstp) {
	static int  result;
	result = -1;

	char filename_temp[FILE_NAME_DIM];
	char char_buf;
	int fd, fd_temp;
	int nread;

	printf("RICEVUTA RICHIESTA DI ELIMINAZIONE DI CARATTERI NUMERICI DEL FILE: %s\n", filename->nome);

	//controllo se il filename passato si riferisce ad un file di testo
	if( isTxt(filename->nome) == 0 ) {
		printf("Filename ricevuto non si riferisce ad un file di testo");
		return &result;
	}

	//apro il file richiesto in lettura
	if ((fd = open(filename->nome, O_RDONLY)) < 0) {
		perror("Errore apertura file");
		return &result;
	}

	//apro un file temporaneo in scrittura
	sprintf(filename_temp, "%s.tmp", filename->nome);

	if ((fd_temp = open(filename_temp, O_CREAT | O_WRONLY, 0644)) < 0) {
		perror("Errore creazione file temporaneo");
		return &result;
	}

	//eseguo le eliminazioni
	result = 0;
	while( (nread=read(fd, &char_buf, sizeof(char))) > 0 ) {
		if( isNumeric(char_buf) == 0 ) {
			if( write(fd_temp, &char_buf, sizeof(char)) < 0 ) {
				result = -1;
				perror("Errore nello scrivere sul file temp");
				return &result;
			}
		}
		else{
			result++;
		}
	}

	if(nread<0) {
		result = -1;
		perror("Errore nel leggere dal file");
		return &result;
	}

	close(fd);
	close(fd_temp);
	//sostituisco il file originale con quello temporaneo che ha le modifiche
	rename(filename_temp, filename->nome);

	printf("richiesta gestita con successo!\n");

	return &result;
}

output_lista_sottodirettori * lista_sottodirettori_1_svc(nome_file *dirname, struct svc_req *rqstp)  {
	static output_lista_sottodirettori  result;

	DIR *dir_handle;
	DIR *subdir_handle;
    struct dirent *current_file;
	char path_completo_file[1024];
	int contaTxt;
	int indiceResult=0;

	printf("Ricevuta richiesta di listing per il direttorio: %s\n", dirname->nome);

	//inizializzo il risultato
	for(int i=0; i<MAX_SOTTODIRETTORI; i++) {
		strcpy(result.sottodirettori[i].nome, "L");
	}
	result.valido = 0;

	//controllo se è un direttorio valido
	if ((dir_handle = opendir(dirname->nome)) != NULL) {
		printf("direttorio %s valido\n", dirname->nome);
		//scorro tutti i file del direttorio di primo livello 
		while ((current_file = readdir(dir_handle)) != NULL && indiceResult < MAX_SOTTODIRETTORI) {
			//creo il path completo del file
			sprintf(path_completo_file, "%s/%s", dirname->nome, current_file->d_name);
			
			//considero solo i direttori 
			if((subdir_handle=opendir(path_completo_file)) != NULL ) {
				printf("\tesploro il sottodirettorio: %s\n", path_completo_file);

				contaTxt = 0;

				//scorro tutti file del direttorio di secondo livello
				while ((current_file = readdir(subdir_handle)) != NULL) {
					if( isTxt(current_file->d_name) == 1 ) {
						printf("\t\tfile di testo trovato: %s\n", current_file->d_name);
						contaTxt++;
					}
				}

				if(contaTxt >= 5) {
					printf("\til direttorio %s ha almeno 5 file di testo\n", path_completo_file);
					strcpy(result.sottodirettori[indiceResult].nome, path_completo_file);
					indiceResult++;
				}
				else{
					printf("\til direttorio %s NON ha almeno 5 file di testo\n", path_completo_file);
				}
			}
		}
	}
	else { 
		printf("Invio risposta negativa al client per dir %s\n", dirname->nome);
		result.valido = -1;
	}

	return &result;
}
