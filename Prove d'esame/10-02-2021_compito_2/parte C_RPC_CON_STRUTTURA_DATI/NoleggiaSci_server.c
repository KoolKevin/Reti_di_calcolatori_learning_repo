#include "NoleggiaSci.h"

static noleggi tabella_noleggi;

//la chiamata avviene nello stub
void inizializza() {
    int i;

    for (i = 0; i < MAX_NOLEGGI; i++) {
        strcpy(tabella_noleggi.noleggio[i].id, "L");
		strcpy(tabella_noleggi.noleggio[i].data, "-1/-1/-1");
		tabella_noleggi.noleggio[i].giorni_noleggio = -1;
		strcpy(tabella_noleggi.noleggio[i].modello, "L");
		tabella_noleggi.noleggio[i].costo_giornaliero = -1;
		strcpy(tabella_noleggi.noleggio[i].nome_file_foto, "L");
    }


	//riempimento con valori arbitrari
	strcpy(tabella_noleggi.noleggio[0].id, "X12AB");
	strcpy(tabella_noleggi.noleggio[0].data, "12/12/2012");
	tabella_noleggi.noleggio[0].giorni_noleggio = 14;
	strcpy(tabella_noleggi.noleggio[0].modello, "Volki Shark");
	tabella_noleggi.noleggio[0].costo_giornaliero = 7;
	strcpy(tabella_noleggi.noleggio[0].nome_file_foto, "VolkiShark1.jpg");

	strcpy(tabella_noleggi.noleggio[2].id, "Y23CC");
	strcpy(tabella_noleggi.noleggio[2].data, "23/12/2012");
	tabella_noleggi.noleggio[2].giorni_noleggio = 7;
	strcpy(tabella_noleggi.noleggio[2].modello, "Volki Shark");
	tabella_noleggi.noleggio[2].costo_giornaliero = 7;
	strcpy(tabella_noleggi.noleggio[2].nome_file_foto, "VolkiShark2.jpg");

	strcpy(tabella_noleggi.noleggio[3].id, "ACD14");
	strcpy(tabella_noleggi.noleggio[3].modello, "Fisher Snow");
	tabella_noleggi.noleggio[3].costo_giornaliero = 5;
	strcpy(tabella_noleggi.noleggio[3].nome_file_foto, "FisherSnow.jpg");


	//STAMPA DI DEBUG
	printf("------------------------------------------STAMPO LE PRIME 10 ENTRY!!!------------------------------------------\n");
	printf("Id\t\tData\t\tGiorni\t\tModello\t\t\tCosto giornaliero\t\tNome file foto\n");
	for (i = 0; i < 10; i++) {
		printf("%s\t\t", tabella_noleggi.noleggio[i].id);
		printf("%s\t", tabella_noleggi.noleggio[i].data);
		printf("%d\t\t", tabella_noleggi.noleggio[i].giorni_noleggio);
		printf("%s\t\t\t", tabella_noleggi.noleggio[i].modello);
		printf("%f\t\t", tabella_noleggi.noleggio[i].costo_giornaliero);
		printf("%s\n", tabella_noleggi.noleggio[i].nome_file_foto);
	}

    printf("\nTerminata inizializzazione struttura dati!\n");
}



int * elimina_sci_1_svc(input_elimina_sci *input, struct svc_req *rqstp) {
	static int  result;

	printf("Ricevuta richiesta di eliminazione di uno sci\n");

	result = -1;

	for(int i=0; i<MAX_NOLEGGI; i++) {
		if( strcmp(input->id, tabella_noleggi.noleggio[i].id) == 0 ) {
			result = 0;

			//cancellola foto
			if( remove(tabella_noleggi.noleggio[i].nome_file_foto) == 0 ) {
				printf("\tfoto sci eliminata\n");
			}
			else{
				printf("\terrore nella eliminazione della foto dello sci\n");
				result = -2;
				break;
			}
			
			//elimino la entry;
			strcpy(tabella_noleggi.noleggio[i].id, "L");
			strcpy(tabella_noleggi.noleggio[i].data, "-1/-1/-1");
			tabella_noleggi.noleggio[i].giorni_noleggio = -1;
			strcpy(tabella_noleggi.noleggio[i].modello, "L");
			tabella_noleggi.noleggio[i].costo_giornaliero = -1;
			strcpy(tabella_noleggi.noleggio[i].nome_file_foto, "L");

			printf("\tsci eliminato\n");
		}
	}

	if(result == -1) {
		printf("\tsci non trovato\n");
	}

	return &result;
}

int * noleggio_sci_1_svc(input_noleggia_sci *input, struct svc_req *rqstp) {
	static int  result;

	printf("Ricevuta richiesta di noleggio dello sci: %s\n", input->id);

	result = -1;

	for(int i=0; i<MAX_NOLEGGI; i++) {
		if( strcmp(input->id, tabella_noleggi.noleggio[i].id) == 0 ) {
			result = 0;

			//controllo che non sia uno sci già noleggiato
			if( tabella_noleggi.noleggio[i].giorni_noleggio != -1 ) {
				result = -2;
				break;
			}

			//aggiungo il noleggio
			strcpy(tabella_noleggi.noleggio[i].data, input->data);
			tabella_noleggi.noleggio[i].giorni_noleggio = input->giorni_noleggio;
		}
	}

	if(result == -1) {
		printf("\tsci non trovato\n");
	}
	else if(result == -2) {
		printf("\tsci gia noleggiato\n");
	}
	else{
		printf("\tnoleggio avvenuto con successo!\n");
	}

	return &result;
}
