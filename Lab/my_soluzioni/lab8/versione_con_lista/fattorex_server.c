/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "fattorex.h"

nodo_giudice * visualizza_classifica_1_svc(void *argp, struct svc_req *rqstp)
{
	static nodo_giudice  result;
	nodo_giudice *nodo;

	 for (i = 0; i < NUMCANDIDATI; i++) {
        if( strcmp(matrice.riga[i].giudice, "L") != 0 ) {
			result = *nodo = (nodo_giudice*) malloc(sizeof(nodo_giudice));
			strcpy(result->name, matrice.riga[i].giudice);
			result->next = null;
		}

    }

	return &result;
}

void * aggiungi_voto_1_svc(char *argp, struct svc_req *rqstp)
{
	static char * result;

	/*
	 * insert server code here
	 */

	return (void *) &result;
}

void *
rimuovi_voto_1_svc(char *argp, struct svc_req *rqstp)
{
	static char * result;

	/*
	 * insert server code here
	 */

	return (void *) &result;
}
