#include <stdio.h>

#include "file.h"

int main (int argc, char *argv[]) {
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];

	CLIENT *clnt;

	char input_utente[256];
	int i;
	//variabili per procedure remote
	int  *caratteri_eliminati;
	nome_file  nomefile;
	output_lista_sottodirettori  *lista_sottodirettori;

	clnt = clnt_create (host, FILES, FILES_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	printf("Inserire:\nE) per eliminare tutti i caratteri numeri da un file di testo\tL) per listare i sottodirettori di una directory\t^D per terminare: ");
    while (gets(input_utente)) {
        if (strcmp(input_utente, "E") == 0) {
			//recupero argomento
            printf("nome file( ***.txt ): ");
            gets(nomefile.nome);
			//invocazioone
            caratteri_eliminati = elimina_occorrenze_1(&nomefile, clnt);
			if (caratteri_eliminati == (int *) NULL) {
				clnt_perror (clnt, "call failed");
			}
			else{
				printf("sono stati eliminati %d caratteri numerici da %s\n", *caratteri_eliminati, nomefile.nome);
			}
        }
        else if (strcmp(input_utente, "L") == 0) {
            //recupero argomento
            printf("nome direttorio: ");
            gets(nomefile.nome);            

			//invocazioone
			lista_sottodirettori = lista_sottodirettori_1(&nomefile, clnt);
			if (lista_sottodirettori == (output_lista_sottodirettori *) NULL) {
				clnt_perror (clnt, "call failed");
			}
			else{
				if(lista_sottodirettori->valido < 0) {
					printf("directory invalida, riprova\n");
				}
				else{
					printf("LISTA:\n");
					for(int i=0; i<MAX_SOTTODIRETTORI; i++) {
						if( strcmp(lista_sottodirettori->sottodirettori[i].nome, "L") == 0 ) {
							break;
						}

						printf("\t%s\n", lista_sottodirettori->sottodirettori[i].nome);
					}
				}
			}
        }
        else {
            printf("Argomento di ingresso errato!!\n");
        }

        printf("Inserire:\nE) per eliminare tutti i caratteri numeri da un file di testo\tL) per listare i sottodirettori di una directory\t^D per terminare: ");
    }

	clnt_destroy (clnt);
	
	exit (0);
}
