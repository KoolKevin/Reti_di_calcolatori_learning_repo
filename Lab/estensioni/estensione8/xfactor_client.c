#include <stdio.h>

#include "xfactor.h"

int main (int argc, char *argv[]) {
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	
	CLIENT *clnt;
	//variabili per:
	//visualizzazione lista candidati
	candidati_list  *lista_candidati;
	candidati_list cl;
	//aggiunta di un candidato alla lista
	int  *esito;
	candidato_entry  candidato_entry;
	//visualizzazione classifica giudici
	classifica_giudici  *classificaGiudici;
	//espressione di un voto
	input_espressione_voto  input_voto;
	//rimozione di un candidato
	nametype  candidato_da_rimuovere;
	//candidati di un certa fase
	char  fase;
	//candidati di un certa categoria
	char  categoria;
	//candidati con almeno un certo numero di voti
	int  soglia_voti;

	char operazione[MAXNAMELEN + 1];
	char scanf_argument[MAXNAMELEN + 1];
	char c;
	int ok;

	clnt = clnt_create (host, OPERATION, OPERATIONVERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}


	/* CICLO DI RICHIESTE FINO A EOF DEL CLIENTE */

	printf("Inserire:\n");
	printf("1) per vedere la lista dei candidato\n");
	printf("2) per aggiungere un candidato\n");
	printf("3) per visualizzare la classifica dei giudici\n");
	printf("4) per esprimere un voto su un candidato\n");
	printf("5) per eliminare un candidato\n");
	printf("6) per visualizzare i candidati di un certa fase\n");
	printf("7) per visualizzare i candidati di una certa categoria\n");
	printf("8) per visualizzare i candidati con almeno un certo numero di voti\n");
	printf("^D per terminare\n");

    while (gets(operazione)) {
		switch( operazione[0] ) {
			case '1':
				lista_candidati = visualizza_candidati_1(NULL, clnt);
				if (lista_candidati == (candidati_list *) NULL) {
					clnt_perror (clnt, "call failed");
				}

				printf("\n----------------------- ECCO LA LISTA DEI CANDIDATI -----------------------\n");
				printf("NOME CANDIDATO\tNOME GIUDICE\tCATEGORIA\tNOME FILE\tFASE\tVOTI\n");
				for (cl=*lista_candidati; cl != NULL; cl = cl->next ) {
					printf("%s\t",cl->entry.nomeCandidato);
					printf("%s\t",cl->entry.nomeGiudice);
					printf("%c\t\t",cl->entry.categoria);
					printf("%s\t",cl->entry.nomeFile);
					printf("%c\t",cl->entry.fase);
					printf("%d\n",cl->entry.voti);
				}

				printf("\n");

				break;
			case '2':
				candidato_entry.nomeCandidato = (char *)malloc(MAXNAMELEN + 1);		//NB: devo allocare e liberare la memoria di tutte le stringhe
				printf("inserisci il nome del candidato: ");
				gets(candidato_entry.nomeCandidato);

				candidato_entry.nomeGiudice = (char *)malloc(MAXNAMELEN + 1);
				printf("inserisci il nome del giudice: ");
				gets(candidato_entry.nomeGiudice);

				//lettura con scanf 	
				do {
					printf("Inserisci la categoria del candidato[ U || D || O || B ]: ");
					scanf("%s", scanf_argument);

					candidato_entry.categoria = scanf_argument[0];
				} while (candidato_entry.categoria != 'U' && candidato_entry.categoria != 'D' && candidato_entry.categoria != 'O' && candidato_entry.categoria != 'B' );

				getchar();
				
				candidato_entry.nomeFile = (char *)malloc(MAXNAMELEN + 1);
				printf("inserisci il nome del file che descrive il candidato: ");
				gets(candidato_entry.nomeFile);
				
				candidato_entry.fase = 'A';
				candidato_entry.voti = 0;

				//DEBUG:
				// printf("Aggiungo questa entry:\n");
				// printf("%s\t",candidato_entry.nomeCandidato);
				// printf("%s\t",candidato_entry.nomeGiudice);
				// printf("%c\t\t",candidato_entry.categoria);
				// printf("%s\t",candidato_entry.nomeFile);
				// printf("%c\t",candidato_entry.fase);
				// printf("%d\n",candidato_entry.voti);

				esito = aggiungi_candidato_1(&candidato_entry, clnt);
				if (esito == (int *) NULL) {
					clnt_perror (clnt, "call failed");
				}

				if(*esito == 0) {
					printf("aggiunto nuovo candidato!\n");
				}

				//libero la memoria che ho allocato
				free(candidato_entry.nomeCandidato);
				free(candidato_entry.nomeGiudice);
				free(candidato_entry.nomeFile);

				break;
			case '3':
				classificaGiudici = classifica_giudici_1(NULL, clnt);
				if (classificaGiudici == (classifica_giudici *) NULL) {
					clnt_perror (clnt, "call failed");
				}

				printf("\n");
				printf("----------------------- CLASSIFICA ORDINATA DEI GIUDICI -----------------------\n");
				for (int i = 0; i < NUM_GIUDICI; i++) {
					if (classificaGiudici->classifica[i].punteggioTot >= 0) {
						printf("%d) %s con %d voti\n", i+1, classificaGiudici->classifica[i].nomeGiudice, classificaGiudici->classifica[i].punteggioTot);
					}
				}
				printf("\n");

				break;
			case '4':
				input_voto.nomeCandidato = (char *)malloc(MAXNAMELEN + 1);
				printf("\nInserisci il nome del candidato: ");
				gets(input_voto.nomeCandidato);

				printf("Inserisci tipo di operazione (A (addiziona) oppure (S) sottrai ): ");
				scanf("%c", &(input_voto.tipoOp));
				getchar();	//consumo il fine linea;

				esito = esprimi_voto_1(&input_voto, clnt);
				if (esito == (int *) NULL) {
					clnt_perror (clnt, "call failed");
				}

				if (*esito < 0) {
					printf("Problemi nell'attribuzione del voto, nome non trovato\n");
				} else if (*esito == 0) {
					printf("Votazione effettuata con successo\n");
				}

				free(input_voto.nomeCandidato);

				break;
			case '5':
				candidato_da_rimuovere = (char *)malloc(MAXNAMELEN + 1);
				printf("\nInserisci il nome del candidato: ");
				gets(candidato_da_rimuovere);

				esito = rimuovi_candidato_1(&candidato_da_rimuovere, clnt);
				if (esito == (int *) NULL) {
					clnt_perror (clnt, "call failed");
				}

				if (*esito < 0) {
					printf("Candidato da rimuovere non trovato\n");
				} else if (*esito == 0) {
					printf("Candidato rimosso con successo\n");
				}

				free(candidato_da_rimuovere);

				break;
			case '6':
				do {
					printf("Inserisci la fase con cui cercare[ A || B || S ]: ");
					scanf("%s", scanf_argument);

					fase = scanf_argument[0];
				} while (fase != 'A' && fase != 'B' && fase != 'S' );

				getchar();  //consumo fine linea

				lista_candidati = get_candidati_of_fase_1(&fase, clnt);
				if (lista_candidati == (candidati_list *) NULL) {
					clnt_perror (clnt, "call failed");
				}

				printf("\n----------------------- ECCO LA LISTA DEI CANDIDATI DELLA FASE: %c -----------------------\n", fase);
				printf("NOME CANDIDATO\tNOME GIUDICE\tCATEGORIA\tNOME FILE\tFASE\tVOTI\n");
				printf("\n");
				for (cl=*lista_candidati; cl != NULL; cl = cl->next ) {
					printf("%s\t",cl->entry.nomeCandidato);
					printf("%s\t",cl->entry.nomeGiudice);
					printf("%c\t\t",cl->entry.categoria);
					printf("%s\t",cl->entry.nomeFile);
					printf("%c\t",cl->entry.fase);
					printf("%d\n",cl->entry.voti);
				}

				break;
			case '7':
				do {
					printf("Inserisci la categoria con cui cercare[ U || D || O || B ]: ");
					scanf("%s", scanf_argument);

					categoria = scanf_argument[0];
				} while (categoria != 'U' && categoria != 'D' && categoria != 'O' && categoria != 'B' );

				getchar();  //consumo fine linea

					lista_candidati = get_candidati_of_categoria_1(&categoria, clnt);
					if (lista_candidati == (candidati_list *) NULL) {
						clnt_perror (clnt, "call failed");
					}

				printf("\n----------------------- ECCO LA LISTA DEI CANDIDATI DELLA CATEGORIA: %c -----------------------\n", categoria);
				printf("NOME CANDIDATO\tNOME GIUDICE\tCATEGORIA\tNOME FILE\tFASE\tVOTI\n");
				printf("\n");
				for (cl=*lista_candidati; cl != NULL; cl = cl->next ) {
					printf("%s\t",cl->entry.nomeCandidato);
					printf("%s\t",cl->entry.nomeGiudice);
					printf("%c\t\t",cl->entry.categoria);
					printf("%s\t",cl->entry.nomeFile);
					printf("%c\t",cl->entry.fase);
					printf("%d\n",cl->entry.voti);
				}

				break;
			case '8':
				soglia_voti = 0;
				while( soglia_voti <= 0 ) {
					printf("Inserisci soglia di voti con cui cercare(>0): ");
					scanf("%d", &soglia_voti);
					getchar();  
				}

				lista_candidati = get_candidati_with_voti_1(&soglia_voti, clnt);
				if (lista_candidati == (candidati_list *) NULL) {
					clnt_perror (clnt, "call failed");
				}

				printf("\n----------------------- ECCO LA LISTA DEI CANDIDATI SOPRA LA SOGLIA: %d -----------------------\n", soglia_voti);
				printf("NOME CANDIDATO\tNOME GIUDICE\tCATEGORIA\tNOME FILE\tFASE\tVOTI\n");
				printf("\n");
				for (cl=*lista_candidati; cl != NULL; cl = cl->next ) {
					printf("%s\t",cl->entry.nomeCandidato);
					printf("%s\t",cl->entry.nomeGiudice);
					printf("%c\t\t",cl->entry.categoria);
					printf("%s\t",cl->entry.nomeFile);
					printf("%c\t",cl->entry.fase);
					printf("%d\n",cl->entry.voti);
				}


				break;
			default:
				printf("operazione non riconosciuta");
		}

		printf("Inserire:\n");
		printf("1) per vedere la lista dei candidato\n");
		printf("2) per aggiungere un candidato\n");
		printf("3) per visualizzare la classifica dei giudici\n");
		printf("4) per esprimere un voto su un candidato\n");
		printf("5) per eliminare un candidato\n");
		printf("6) per visualizzare i candidati di un certa fase\n");
		printf("7) per visualizzare i candidati di una certa categoria\n");
		printf("8) per visualizzare i candidati con almeno un certo numero di voti\n");
		printf("^D per terminare\n");
	}

	exit (0);
}

