#include<stdio.h>

#include "RentACar.h"


int main (int argc, char *argv[]) {
	char *host;
	CLIENT *clnt;

	char input_utente[STRING_DIM + 1];
	int i;
	//variabili per le procedure remote
	prenotazioni_cercate  *result_1;
	tipoVeicolo  tipoV;
	int  *result_2;
	input_aggiorna_licenza  aggiorna_licenza_arg;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];	

	clnt = clnt_create (host, RENT_A_CAR, RENT_A_CAR_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	printf("Inserire:\nV) per visualizzare le prenotazioni di un certo tipo di veicolo\tA) per aggiornare una patente \t^D per terminare: ");
    while (gets(input_utente)) {
        if (strcmp(input_utente, "V") == 0) {
			//recupero argomento
            printf("Tipologia di veicolo(camper, auto): \n");
            gets(input_utente);
			strcpy(tipoV.tipo, input_utente);
			//invocazioone
            result_1 = visualizza_prenotazioni_1(&tipoV, clnt);
			//elaborazione risultato
			if (result_1 == (prenotazioni_cercate *) NULL) {
				clnt_perror (clnt, "call failed");
			}
			else{
				printf("TARGA\t\tPATENTE\t\tTIPO VEICOLO\t\tDIRETTORIO IMMAGINI\n");
				for(i=0; i<MAX_PRENOTAZIONI_CERCATE; i++) {
					if( strcmp(result_1->entry[i].targa, "L") == 0 ) {
						break;
					}

					printf("%s\t\t", result_1->entry[i].targa);
					printf("%s\t\t", result_1->entry[i].patente);
					printf("%s\t\t", result_1->entry[i].tipo);
					printf("%s\n", result_1->entry[i].direttorio_img);
				}
			}
        }
        else if (strcmp(input_utente, "A") == 0) {
            //recupero argomento
            printf("Targa del veicolo: \n");
            gets(input_utente);            
			strcpy(aggiorna_licenza_arg.targa, input_utente);

			printf("Patente nuova(5 interi): \n");
            gets(input_utente);            
			strcpy(aggiorna_licenza_arg.patente_nuova, input_utente);
			//invocazioone
			result_2 = aggiorna_licenza_1(&aggiorna_licenza_arg, clnt);
			//elaborazione risultato
			if (result_2 == (int *) NULL) {
				clnt_perror (clnt, "call failed");
			}
			else{
				if(result_2 < 0) {
					printf("targa veicolo non trovata\n");
				}
				else{
					printf("patente aggiornata correttamente\n");
				}
			}
        }
        else {
            printf("Argomento di ingresso errato!!\n");
        }

        printf("Inserire:\nV) per visualizzare le prenotazioni di un certo tipo di veicolo\tA) per aggiornare una patente \t^D per terminare: ");
    }

	clnt_destroy (clnt);

	exit (0);
}
