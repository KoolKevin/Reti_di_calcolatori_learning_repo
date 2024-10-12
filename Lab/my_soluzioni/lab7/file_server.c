#include <rpc/rpc.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include "file.h"

RisultatoConteggio * file_scan_1_svc(File *argp, struct svc_req *rqstp)
{
	static RisultatoConteggio  result;
	int fd, nread, wordCharCounter;
	char read_char;
	char err[128];
	
	//inizializzazione risultato
	result.numRighe = 0;
	result.numParole = 0;
	result.numCaratteri = 0;
	
	if ((fd = open(argp->nomeFile, O_RDONLY)) < 0) {
		perror("Errore apertura file sorgente");
		result.numRighe = -1;
		result.numParole = -1;
		result.numCaratteri = -1;
		
		return  &result;
	}

	/* Se il file esiste */
	wordCharCounter=0;
	while ((nread = read(fd, &read_char, sizeof(char))) != 0) {
		if (nread < 0) {
			//errore in lettura
			sprintf(err, "(PID %d) impossibile leggere dal file", getpid());
			perror(err);
			result.numRighe = -1;
			result.numParole = -1;
			result.numCaratteri = -1;
			
			return  &result;
		} else {
			//conteggio caratteri
			result.numCaratteri++;
			
			//conteggioLinee
			if (read_char == '\n' ) {
				result.numRighe++;
				//se vado a capo finisce anche una parola
				if( wordCharCounter>0 ) {
					result.numParole++;
				}
				wordCharCounter = 0;
			}
			
			//conteggio parole
			if (read_char == ' '|| read_char == '\t') {
				if( wordCharCounter>0) {
					result.numParole++;
				}
				
				wordCharCounter = 0;
			} 
			else if(read_char != '\n') {
				//ho letto un carattere di una parola
				wordCharCounter++;
			}
		}
	}

	return &result;
}

OutputDirScan * dir_scan_1_svc(InputDirScan *argp, struct svc_req *rqstp)
{
	static OutputDirScan  result;
	DIR *dirHandle;
    struct dirent *dd1;
	char dirFile[512];
	int fd;
	int dim;

	//inizializzazione risultato
	result.numerosita = 0;

	//ottengo l'handle alla directory
	if ((dirHandle = opendir(argp->dir.nomeFile)) != NULL) { 
		// direttorio presente 

		//scorro i file della directory passata
		while ((dd1 = readdir(dirHandle)) != NULL) { 
			sprintf(dirFile, "%s/%s", argp->dir.nomeFile, dd1->d_name);
			printf("dirFile: %s\n", dirFile);

			if ( opendir(dirFile) == NULL ) {  //se non riesco ad aprire vuol dire che è un file

				//calcolo dimensione file
				if ( (fd=open(dirFile, O_RDONLY)) < 0 ) {
					perror("Errore nell'apertura di un file per calcolarne la dimensione\n");
					result.numerosita = -1;
				
					return  &result;
				}
				else{
					dim = lseek(fd, 0, SEEK_END);
					// if( dim < 0) {
					// 	perror("Errore nel calcolo della dimensione di un file\n");
					// }

					printf("il file %s pesa %d byte\n", dirFile, dim);

					if( dim > argp->soglia ) {
						strcpy(result.fileSopraSoglia.files[result.numerosita].nomeFile, dirFile);
						result.numerosita++;
					}
				}

				close(fd);
			}
		}
	}
	else{
		//direttorio non presente
		perror("Errore, direttorio richiesto non presente\n");
		result.numerosita = -1;
	
		return  &result;
	}

	return &result;
}
