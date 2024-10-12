#include "xfactor.h"

#define MATRICE_SIZE 12

struct RIGA {
	char candidato[MAX_NAME_SIZE];
	char giudice[MAX_NAME_SIZE];
	char categoria;
	char nomeFile[MAX_NAME_SIZE];
	char fase;
	int voti;
};
typedef struct RIGA RIGA;

static RIGA matriceVoti[MATRICE_SIZE];
static int inizializzato=0;

void inizializza() {
    int i;

    // inizializzazione struttura dati
    for (i = 0; i < MATRICE_SIZE; i++) {
        strcpy(matriceVoti[i].candidato, "L");
        strcpy(matriceVoti[i].giudice, "L");
		matriceVoti[i].categoria = 'L';
		strcpy(matriceVoti[i].nomeFile, "L");
		matriceVoti[i].fase = 'L';
		matriceVoti[i].voti = -1;
    }

	//riga 1
	strcpy(matriceVoti[0].candidato, "Koltraka Kevin");
	strcpy(matriceVoti[0].giudice, "Maria de Filippi");
	matriceVoti[0].categoria = 'U';
	strcpy(matriceVoti[0].nomeFile, "kevin.txt");
	matriceVoti[0].fase = 'S';
	matriceVoti[0].voti = 236;
	//riga 2
	strcpy(matriceVoti[1].candidato, "Francesco Roffia");
	strcpy(matriceVoti[1].giudice, "Paul Mcartney");
	matriceVoti[1].categoria = 'O';
	strcpy(matriceVoti[1].nomeFile, "frank.txt");
	matriceVoti[1].fase = 'A';
	matriceVoti[1].voti = 122;
	//riga 6
	strcpy(matriceVoti[5].candidato, "Mario Bianchi");
	strcpy(matriceVoti[5].giudice, "Barack Obama");
	matriceVoti[5].categoria = 'B';
	strcpy(matriceVoti[5].nomeFile, "bianchi.txt");
	matriceVoti[5].fase = 'B';
	matriceVoti[5].voti = 115;
	//riga 7
	strcpy(matriceVoti[6].candidato, "Mario Rossi");
	strcpy(matriceVoti[6].giudice, "Paul Mcartney");
	matriceVoti[6].categoria = 'B';
	strcpy(matriceVoti[6].nomeFile, "mario.txt");
	matriceVoti[6].fase = 'B';
	matriceVoti[6].voti = 115;

    printf("Terminata inizializzazione struttura dati!\n");
	inizializzato=1;

	//stampa di debug
	for (i = 0; i < MATRICE_SIZE; i++) {
		printf("\n%s; ", matriceVoti[i].candidato );
		printf("%s; ", matriceVoti[i].giudice );
		printf("%c; ", matriceVoti[i].categoria  );
		printf("%s; ", matriceVoti[i].nomeFile  );
		printf("%c; ", matriceVoti[i].fase );
		printf("%d;\n\n", matriceVoti[i].voti  );
	}
}

Output * classifica_giudici_1_svc(void *argp, struct svc_req *rqstp)
{
	static Output  result;
	int i, j;
	int trovato=0;
	int indiceResult=0;

	if(inizializzato==0) {
		inizializza();
	}

	printf("richiesta di visualizzazione classifica ricevuta\n");

	//inizializzazione risultato; MOLTO IMPORTANTE perchè è static
	for(i=0; i<NUM_GIUDICI; i++) {
		strcpy(result.classificaGiudici[i].nomeGiudice, "L");
		result.classificaGiudici[i].punteggioTot = -1;
	} 

	//scorro la matrice
	for (i=0; i<MATRICE_SIZE; i++) {
		if( strcmp(matriceVoti[i].giudice, "L") == 0)	//salto le entry vuote
			continue;

		//controllo che il giudice sia nuovo scorrendo tutte le entry del risultato
		for(j=0; j<NUM_GIUDICI && !trovato; j++) {
			//giudice gia presente -> sommo i voti
			if( strcmp( matriceVoti[i].giudice, result.classificaGiudici[j].nomeGiudice) == 0 ) {
				result.classificaGiudici[j].punteggioTot += matriceVoti[i].voti;
				trovato=1;
			}
		} 

		if(!trovato) {
			//non ho trovato il giudice -> aggiungo una nuova entry al risultato
			strcpy(result.classificaGiudici[indiceResult].nomeGiudice, matriceVoti[i].giudice);
			result.classificaGiudici[indiceResult].punteggioTot = matriceVoti[i].voti;
			indiceResult++;
		}
	 	
		trovato=0;
    }

	printf("indiceResult %d\n", indiceResult);

	//ordino la classifica
	int indiceMax=0;
	Giudice temp;

	while (indiceResult > 1) {
		//trovo l'indice del giudice con più voti
		for (i = 1; i < indiceResult; i++) {
			if(result.classificaGiudici[indiceMax].punteggioTot > result.classificaGiudici[i].punteggioTot) {
				indiceMax=i;
			}
		}

		if (indiceMax < indiceResult - 1) {
			temp=result.classificaGiudici[indiceResult-1];
			result.classificaGiudici[indiceResult-1] = result.classificaGiudici[indiceMax];
			result.classificaGiudici[indiceMax] = temp;
		}

		indiceResult--;
		indiceMax=0;
	}
	

	return &result;

}

int * esprimi_voto_1_svc(Input *argp, struct svc_req *rqstp)
{
	static int  result;
	int trovato=0;
	int i;

	if(inizializzato==0) {
		inizializza();
	}

	printf("richiesta di voto ricevuta\n");

	if( argp->tipoOp == 'a' ) {
		for (i=0; i<MATRICE_SIZE && !trovato; i++) {
			if( strcmp(matriceVoti[i].giudice, "L") == 0)	//salto le entry vuote
				continue;

			if( strcmp(matriceVoti[i].candidato, argp->nomeCandidato) == 0 ) {
				matriceVoti[i].voti++;
				trovato=1;
			}
    	}

		if(trovato == 0) {
			result=-1;
		}
		else{
			result=0;
		}
	}
	else if( argp->tipoOp == 'r' ) {
		for (i=0; i<MATRICE_SIZE && !trovato; i++) {
			if( strcmp(matriceVoti[i].giudice, "L") == 0)	//salto le entry vuote
				continue;

			if( strcmp(matriceVoti[i].candidato, argp->nomeCandidato) == 0 ) {
				matriceVoti[i].voti--;
				trovato=1;
			}
    	}

		if(trovato == 0) {
			result=-1;
		}
		else{
			result=0;
		}
	}
	else{
		result = -2;
	}

	return &result;
}
