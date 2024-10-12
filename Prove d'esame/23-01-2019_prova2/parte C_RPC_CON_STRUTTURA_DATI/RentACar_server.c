#include "RentACar.h"

static prenotazioni tabella_prenotazioni;

//la chiamata avviene nello stub
void inizializza() {
    int i;

    for (i = 0; i < MAX_PRENOTAZIONI; i++) {
        strcpy(tabella_prenotazioni.entry[i].targa, "L");
		strcpy(tabella_prenotazioni.entry[i].patente, "L");
		strcpy(tabella_prenotazioni.entry[i].tipo, "L");
		strcpy(tabella_prenotazioni.entry[i].direttorio_img, "L");
    }


	//riempimento con valori arbitrari
	strcpy(tabella_prenotazioni.entry[0].targa, "AN745NL");
	strcpy(tabella_prenotazioni.entry[0].patente, "00003");
	strcpy(tabella_prenotazioni.entry[0].tipo, "auto");
	strcpy(tabella_prenotazioni.entry[0].direttorio_img, "AN745NL_img/");

	strcpy(tabella_prenotazioni.entry[1].targa, "AA457GF");
	strcpy(tabella_prenotazioni.entry[1].patente, "50006");
	strcpy(tabella_prenotazioni.entry[1].tipo, "camper");
	strcpy(tabella_prenotazioni.entry[1].direttorio_img, "AA457GF_img/");

	strcpy(tabella_prenotazioni.entry[2].targa, "NU547PL");
	strcpy(tabella_prenotazioni.entry[2].patente, "40063");
	strcpy(tabella_prenotazioni.entry[2].tipo, "auto");
	strcpy(tabella_prenotazioni.entry[2].direttorio_img, "NU547PL_img/");

	strcpy(tabella_prenotazioni.entry[3].targa, "LR897AH");
	strcpy(tabella_prenotazioni.entry[3].patente, "56832");
	strcpy(tabella_prenotazioni.entry[3].tipo, "camper");
	strcpy(tabella_prenotazioni.entry[3].direttorio_img, "LR897AH_img/");

	strcpy(tabella_prenotazioni.entry[4].targa, "MD506DW");
	strcpy(tabella_prenotazioni.entry[4].patente, "00100");
	strcpy(tabella_prenotazioni.entry[4].tipo, "camper");
	strcpy(tabella_prenotazioni.entry[4].direttorio_img, "MD506DW_img/");	

	//STAMPA DI DEBUG
	printf("TARGA\t\tPATENTE\t\tTIPO VEICOLO\t\tDIRETTORIO IMMAGINI\n");
	for (i = 0; i < 10; i++) {
		printf("%s\t\t", tabella_prenotazioni.entry[i].targa);
		printf("%s\t\t", tabella_prenotazioni.entry[i].patente);
		printf("%s\t\t", tabella_prenotazioni.entry[i].tipo);
		printf("%s\n", tabella_prenotazioni.entry[i].direttorio_img);
	}

    printf("Terminata inizializzazione struttura dati!\n");
}

prenotazioni_cercate * visualizza_prenotazioni_1_svc(tipoVeicolo *input, struct svc_req *rqstp) {
	static prenotazioni_cercate  result;

	int i;
	int indiceRisultato = 0;

	//inizializzazione risultato
	for (i = 0; i < MAX_PRENOTAZIONI_CERCATE; i++) {
        strcpy(result.entry[i].targa, "L");
		strcpy(result.entry[i].patente, "L");
		strcpy(result.entry[i].tipo, "L");
		strcpy(result.entry[i].direttorio_img, "L");
    }

	printf("ricevuta richiesta di visualizzazione dei veicoli del tipo: %s\n", input->tipo);

	for (i = 0; i < MAX_PRENOTAZIONI; i++) {
		//controllo tipo
		if( strcmp(tabella_prenotazioni.entry[i].tipo, input->tipo) == 0 ) {
			//controllo che la targa sia >= di 'EDXXXXXX '
			if( tabella_prenotazioni.entry[i].targa[0] > 'E' || (tabella_prenotazioni.entry[i].targa[0] == 'E' && tabella_prenotazioni.entry[i].targa[1] >= 'D') ) {
				strcpy(result.entry[indiceRisultato].targa, tabella_prenotazioni.entry[i].targa);
				strcpy(result.entry[indiceRisultato].patente, tabella_prenotazioni.entry[i].patente);
				strcpy(result.entry[indiceRisultato].tipo, tabella_prenotazioni.entry[i].tipo);
				strcpy(result.entry[indiceRisultato].direttorio_img, tabella_prenotazioni.entry[i].direttorio_img);

				indiceRisultato++;
			}
		}

		//solo i primi 6 risultati significativi
		if(indiceRisultato == 6) {
			break;
		}
	}


	printf("TARGA\t\tPATENTE\t\tTIPO VEICOLO\t\tDIRETTORIO IMMAGINI\n");
	for(i=0; i<MAX_PRENOTAZIONI_CERCATE; i++) {
		if( strcmp(result.entry[i].targa, "L") == 0 ) {
			break;
		}

		printf("%s\t\t", result.entry[i].targa);
		printf("%s\t\t", result.entry[i].patente);
		printf("%s\t\t", result.entry[i].tipo);
		printf("%s\n", result.entry[i].direttorio_img);
	}

	return &result;
}

int * aggiorna_licenza_1_svc(input_aggiorna_licenza *input, struct svc_req *rqstp) {
	static int  result;

	int i;
	result = -1;	//targa non trovata

	for (i = 0; i < MAX_PRENOTAZIONI; i++) {
		if( strcmp(tabella_prenotazioni.entry[i].targa, input->targa) == 0 ) {
			
			strcpy(tabella_prenotazioni.entry[i].patente, input->patente_nuova);
			result = 0;
			//una volta trovata la entry da aggiornare esco
			break;	
		}
	}

	return &result;
}
