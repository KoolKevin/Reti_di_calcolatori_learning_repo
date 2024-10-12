#include<stdio.h>

#include "NoleggiaSci.h"

int main (int argc, char *argv[]) {
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];

	//variabili per le procedure remote
	CLIENT *clnt;
	int  *esito;
	input_elimina_sci  input_eliminazione;
	input_noleggia_sci  input_noleggio;
	
	char input_utente[STRING_DIM + 1];
	int i, ok;
	char c;
	

	clnt = clnt_create (host, NOLEGGIA_SCI, NOLEGGIA_SCI_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	printf("Inserire:\nE) per eliminare uno sci\tA) per aggiungere un noleggio\t^D per terminare: ");
    while (gets(input_utente)) {
        if (strcmp(input_utente, "E") == 0) {
			//recupero argomento
            printf("id: \n");
            gets(input_eliminazione.id);

			//invocazioone
            esito = elimina_sci_1(&input_eliminazione, clnt);
			if (esito == (int *) NULL) {
				clnt_perror (clnt, "call failed");
			}
			else{
				if(*esito < 0) {
					printf("Errore nella eliminazione\n");
				}
				else{
					printf("Eliminazione dello sci avvenuta con successo\n");
				}
			}
        }
        else if (strcmp(input_utente, "A") == 0) {
            //recupero argomenti
            printf("id: \n");
            gets(input_noleggio.id);       

			printf("data inizio noleggio(dd/mm/yyyy): \n");		//questo do per scontato che sia inserito bene perchè è lungo da controllare
            gets(input_noleggio.data);       

			do{
				printf("durata del noleggio in giorni:\n");

				while( (ok = scanf("%d", &input_noleggio.giorni_noleggio)) != 1 ) {
					printf("\tcaratteri invalidi: ");

					do {
						c=getchar();
						printf("%c ", c);
					} while (c != '\n');

				}
			} while( input_noleggio.giorni_noleggio < 1 || input_noleggio.giorni_noleggio > 30 );

			getchar();//consumo fine linea

			//invocazioone
			esito = noleggio_sci_1(&input_noleggio, clnt);
			if (esito == (int *) NULL) {
				clnt_perror (clnt, "call failed");
			}
			else{
				if(*esito < 0) {
					if(*esito == -1)
						printf("sci non trovato\n");
					if(*esito == -2)
						printf("sci già occupato\n");
				}
				else{
					printf("noleggio effettuto con successo\n");
				}
			}
        }
        else {
            printf("Argomento di ingresso errato!!\n");
        }

        printf("Inserire:\nE) per eliminare uno sci\tA) per aggiungere un noleggio\t^D per terminare: ");
    }

	clnt_destroy (clnt);

	exit(0);
}
