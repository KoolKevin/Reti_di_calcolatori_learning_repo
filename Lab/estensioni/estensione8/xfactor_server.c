#include "xfactor.h"

static candidati_list lista_candidati;
static candidati_list * coda;	//mantengo una variabile globale per la coda che mi è utile quando devo aggiungere nodi alla lista

//questo metodo viene chiamato all'interno dello stub del server
void inizializza() {
    candidati_list cl;	//puntatore al nodo
    candidati_list * clp = &lista_candidati; // NB: questo è un puntatore ad un puntatore di un nodo

	cl = *clp = (candidati_node*) malloc(sizeof(candidati_node));

	/* Alloco memoria e popolo la prima e unica entry della lista inizializzata */
	cl->entry.nomeCandidato = malloc( strlen("Kevin Koltraka")+1 );
	strcpy(cl->entry.nomeCandidato, "Kevin Koltraka");

	cl->entry.nomeGiudice = malloc( strlen("Paul McCartney")+1 );
	strcpy(cl->entry.nomeGiudice, "Paul McCartney");

	cl->entry.categoria = 'U';

	cl->entry.nomeFile = malloc( strlen("kevin.txt")+1 );
	strcpy(cl->entry.nomeFile, "kevin.txt");

	cl->entry.fase = 'A';

	cl->entry.voti = 154;

	coda = clp = &cl->next;
    *clp=NULL;

    //fine inizializzazione
    printf("Terminata inizializzazione struttura dati!\n");

	/*
		Al posto di fare una inizializzazione a mano hard-coded, si potrebbe
		leggere da un file in cui viene salvato lo stato della struttura
		dati periodicamente.
	*/
}

classifica_giudici * classifica_giudici_1_svc(void *argp, struct svc_req *rqstp) {
	static classifica_giudici  result;

    int i, k, presente, indicePosizioneLibera = 0, max, count = 0;
    classifica_entry  swap;

    // Inizializzo il risultato con valori di default
    for (i = 0; i < NUM_GIUDICI; i++) {
		result.classifica[i].nomeGiudice = (char *)malloc(MAXNAMELEN + 1);	
        strcpy(result.classifica[i].nomeGiudice, "L");
        result.classifica[i].punteggioTot = -1;
    }

    // Inserisco i giudici, calcolandone il punteggio
    for (candidati_list cl=lista_candidati; cl != NULL; cl = cl->next ) {
        presente = 0;

        // Se giudice è già presente nell'array di risultato
        for (k = 0; k<NUM_GIUDICI && presente!=1; k++) {
            if (strcmp(result.classifica[k].nomeGiudice, cl->entry.nomeGiudice) == 0) {
                result.classifica[k].punteggioTot += cl->entry.voti;
                presente = 1;
            }
        }

        // Se no, inseriamolo
        if (presente == 0) {
            strcpy(result.classifica[indicePosizioneLibera].nomeGiudice, cl->entry.nomeGiudice);
            result.classifica[indicePosizioneLibera].punteggioTot = cl->entry.voti;
            indicePosizioneLibera++;
        }
    }

    // Ordino result in base al punteggio (Bubble sort)
    for (i = 0; i < NUM_GIUDICI - 1; i++) {
        for (k = 0; k < NUM_GIUDICI - i - 1; k++) {
            if (result.classifica[k].punteggioTot < result.classifica[k + 1].punteggioTot) {
                swap = result.classifica[k];
                result.classifica[k] = result.classifica[k + 1];
                result.classifica[k + 1] = swap;
            }
        }
    }

	return &result;
}

int * esprimi_voto_1_svc(input_espressione_voto *input, struct svc_req *rqstp) {
	static int  result = -1;

	printf("ricevuta richiesta di espressione voto\nNome: %s; Tipo: %c\n", input->nomeCandidato, input->tipoOp);

	for (candidati_list cl=lista_candidati; cl != NULL; cl = cl->next ) {
        if (strcmp(input->nomeCandidato, cl->entry.nomeCandidato) == 0) {
            if (input->tipoOp == 'A') {
				cl->entry.voti++;
            } else if (input->tipoOp == 'S' && cl->entry.voti > 0) { // Voto min è 0
                cl->entry.voti--;
            } else {
                printf("Invalid operation!\n");
                break;
            }

            printf("Risultato: \n\t Candidato = %s \n\t Voti = %d\n", input->nomeCandidato, cl->entry.voti);
            result = 0;
            break;
        }
    }

    if (result < 0) {
        printf("Problemi nell'attribuzione del voto, nome non trovato\n");
    }

	return &result;
}

candidati_list * visualizza_candidati_1_svc(void *argp, struct svc_req *rqstp) {
	return &lista_candidati;
}

int * aggiungi_candidato_1_svc(candidato_entry *entry, struct svc_req *rqstp) {
	static int  result;

	candidati_list cl;	//puntatore al nodo

	cl = *coda = (candidati_node*) malloc(sizeof(candidati_node));

	/* Alloco memoria e popolo la prima e unica entry della lista inizializzata */
	cl->entry.nomeCandidato = malloc( strlen(entry->nomeCandidato)+1 );
	strcpy(cl->entry.nomeCandidato, entry->nomeCandidato);

	cl->entry.nomeGiudice = malloc( strlen(entry->nomeGiudice)+1 );
	strcpy(cl->entry.nomeGiudice, entry->nomeGiudice);

	cl->entry.categoria = entry->categoria;

	cl->entry.nomeFile = malloc( strlen(entry->nomeFile)+1 );
	strcpy(cl->entry.nomeFile, entry->nomeFile);

	cl->entry.fase = entry->fase;

	cl->entry.voti = entry->voti;

	coda = &cl->next;
    *coda = NULL;

	//questa rocedura fallisce solo se finisce la memoria, in genere ritorno sempre 0 quindicePosizioneLiberai
	result = 0;

    //fine inizializzazione
    printf("Aggiunto un candidato alla struttura dati!\n");

	return &result;

	/*
		TODO: 
		Dovrei controllare che cosa sto inserendo nella lista:
			-non posso inserire candidati duplicati(nomeCandidato chiave)
			-non posso inserire categorie non esistenti
			-non posso inserire nomi di giudici non validi
	*/
}

int * rimuovi_candidato_1_svc(nametype *nome_candidato, struct svc_req *rqstp) {
	static int  result = -1;

	//mantengo un puntatore al nodo precedente in modo da riuscire a ricollegare la lista spezzata dopo l'eliminazione
	candidati_list nodo_precedente = lista_candidati;	

	//controllo che la lista non sia vuota
	if( lista_candidati == NULL ) {
		printf("LISTA VUOTA, RIMOZIONE IMPOSSIBILE\n");
		result = -1;
		return &result;
	}	

	//controllo eliminazione testa della lista
	if( strcmp( lista_candidati->entry.nomeCandidato, *nome_candidato ) == 0) {
		//controllo se la lista contiene solo un elemento
		if( lista_candidati->next == NULL ) {
			coda = &lista_candidati;
		}

		//il nodo successivo diventa la nuova testa
		lista_candidati = lista_candidati->next;		

		//libero tutta la memoria del nodo
		free(nodo_precedente->entry.nomeCandidato);
		free(nodo_precedente->entry.nomeGiudice);
		free(nodo_precedente->entry.nomeFile);
		free(nodo_precedente);

		result = 0;
		return &result;
	}

	//controllo elimazione tutto il resto
	for (candidati_list cl=lista_candidati->next; cl != NULL; cl = cl->next ) {
        if (strcmp(*nome_candidato, cl->entry.nomeCandidato) == 0) {
			//attacco il nodo precedente al successivo
			nodo_precedente->next = cl->next;
			//controllo se sto eliminando la coda e nel caso aggiorna la variabile globale
			if( cl->next == NULL ) {
				coda = &nodo_precedente->next;
			}
			//elimino il nodo
			free(cl->entry.nomeCandidato);
			free(cl->entry.nomeGiudice);
			free(cl->entry.nomeFile);
			free(cl);

			result = 0;
			break;
		}

		nodo_precedente = cl;
	}
	

	return &result;
}

candidati_list * get_candidati_of_fase_1_svc(char *fase, struct svc_req *rqstp) {
	static candidati_list  result;

	candidati_list * result_pointer = &result;
	candidati_list nodo_risultato;

	/* libero la memoria della chiamata precedente */
    xdr_free((xdrproc_t) xdr_candidati_list, (caddr_t) &result);

	for (candidati_list cl=lista_candidati; cl != NULL; cl = cl->next ) {
		if( *fase == cl->entry.fase ) {
			//aggiungo al risultato, copiando il nodo corrente
			nodo_risultato = *result_pointer = (candidati_node*) malloc(sizeof(candidati_node));

			nodo_risultato->entry.nomeCandidato = malloc( strlen( cl->entry.nomeCandidato )+1 );
			strcpy(nodo_risultato->entry.nomeCandidato, cl->entry.nomeCandidato);

			nodo_risultato->entry.nomeGiudice = malloc( strlen(cl->entry.nomeGiudice)+1 );
			strcpy(nodo_risultato->entry.nomeGiudice, cl->entry.nomeGiudice);

			nodo_risultato->entry.categoria = cl->entry.categoria;

			nodo_risultato->entry.nomeFile = malloc( strlen( cl->entry.nomeFile )+1 );
			strcpy(nodo_risultato->entry.nomeFile, cl->entry.nomeFile);

			nodo_risultato->entry.fase = cl->entry.fase;

			nodo_risultato->entry.voti = cl->entry.voti;

			//sposto il result pointer
			result_pointer = &nodo_risultato->next;
		}
	}

	//chiudo la lista
	*result_pointer=NULL;

	return &result;
}

candidati_list * get_candidati_of_categoria_1_svc(char *categoria, struct svc_req *rqstp) {
	static candidati_list  result;

	candidati_list * result_pointer = &result;
	candidati_list nodo_risultato;

	/* libero la memoria della chiamata precedente */
    xdr_free((xdrproc_t) xdr_candidati_list, (caddr_t) &result);

	for (candidati_list cl=lista_candidati; cl != NULL; cl = cl->next ) {
		if( *categoria == cl->entry.categoria ) {
			//aggiungo al risultato, copiando il nodo corrente
			nodo_risultato = *result_pointer = (candidati_node*) malloc(sizeof(candidati_node));

			nodo_risultato->entry.nomeCandidato = malloc( strlen( cl->entry.nomeCandidato )+1 );
			strcpy(nodo_risultato->entry.nomeCandidato, cl->entry.nomeCandidato);

			nodo_risultato->entry.nomeGiudice = malloc( strlen(cl->entry.nomeGiudice)+1 );
			strcpy(nodo_risultato->entry.nomeGiudice, cl->entry.nomeGiudice);

			nodo_risultato->entry.categoria = cl->entry.categoria;

			nodo_risultato->entry.nomeFile = malloc( strlen( cl->entry.nomeFile )+1 );
			strcpy(nodo_risultato->entry.nomeFile, cl->entry.nomeFile);

			nodo_risultato->entry.fase = cl->entry.fase;

			nodo_risultato->entry.voti = cl->entry.voti;

			//sposto il result pointer
			result_pointer = &nodo_risultato->next;
		}
	}

	//chiudo la lista
	*result_pointer=NULL;

	return &result;
}

candidati_list * get_candidati_with_voti_1_svc(int *soglia_voti, struct svc_req *rqstp) {
	static candidati_list  result;

	candidati_list * result_pointer = &result;
	candidati_list nodo_risultato;

	/* libero la memoria della chiamata precedente */
    xdr_free((xdrproc_t) xdr_candidati_list, (caddr_t) &result);

	for (candidati_list cl=lista_candidati; cl != NULL; cl = cl->next ) {
		if( cl->entry.voti >= *soglia_voti ) {
			//aggiungo al risultato, copiando il nodo corrente
			nodo_risultato = *result_pointer = (candidati_node*) malloc(sizeof(candidati_node));

			nodo_risultato->entry.nomeCandidato = malloc( strlen( cl->entry.nomeCandidato )+1 );
			strcpy(nodo_risultato->entry.nomeCandidato, cl->entry.nomeCandidato);

			nodo_risultato->entry.nomeGiudice = malloc( strlen(cl->entry.nomeGiudice)+1 );
			strcpy(nodo_risultato->entry.nomeGiudice, cl->entry.nomeGiudice);

			nodo_risultato->entry.categoria = cl->entry.categoria;

			nodo_risultato->entry.nomeFile = malloc( strlen( cl->entry.nomeFile )+1 );
			strcpy(nodo_risultato->entry.nomeFile, cl->entry.nomeFile);

			nodo_risultato->entry.fase = cl->entry.fase;

			nodo_risultato->entry.voti = cl->entry.voti;

			//sposto il result pointer
			result_pointer = &nodo_risultato->next;
		}
	}

	//chiudo la lista
	*result_pointer=NULL;

	return &result;
}
