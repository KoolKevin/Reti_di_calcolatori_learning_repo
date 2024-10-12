/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "fattorex.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

void inizializza() {
    int i;

    // inizializzazione struttura dati
    for (i = 0; i < NUM_CANDIDATI; i++) {
        strcpy(matrice.riga[i].candidato, "L");
        strcpy(matrice.riga[i].giudice, "L");
		matrice.riga[i].categoria = 'L';
		strcpy(matrice.riga[i].nomeFile, "L");
		matrice.riga[i].fase = 'L';
		matrice.riga[i].voti = -1;
    }

	//riga 1
	strcpy(matrice.riga[0].candidato, "Koltraka Kevin");
	strcpy(matrice.riga[0].giudice, "Maria de Filippi");
	matrice.riga[0].categoria = 'U';
	strcpy(matrice.riga[0].nomeFile, "kevin.txt");
	matrice.riga[0].fase = 'S';
	matrice.riga[0].voti = 236;
	//riga 2
	strcpy(matrice.riga[1].candidato, "Francesco Roffia");
	strcpy(matrice.riga[1].giudice, "Paul Mcartney");
	matrice.riga[1].categoria = 'O';
	strcpy(matrice.riga[1].nomeFile, "frank.txt");
	matrice.riga[1].fase = 'A';
	matrice.riga[1].voti = 2252;
	//riga 7
	strcpy(matrice.riga[6].candidato, "Mario Rossi");
	strcpy(matrice.riga[6].giudice, "Paul Mcartney");
	matrice.riga[6].categoria = 'B';
	strcpy(matrice.riga[6].nomeFile, "mario.txt");
	matrice.riga[6].fase = 'B';
	matrice.riga[6].voti = 12;

    printf("Terminata inizializzazione struttura dati!\n");
}

static void
fattorex_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		char aggiungi_voto_1_arg;
		char rimuovi_voto_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case VISUALIZZA_CLASSIFICA:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_classifica_giudici;
		local = (char *(*)(char *, struct svc_req *)) visualizza_classifica_1_svc;
		break;

	case AGGIUNGI_VOTO:
		_xdr_argument = (xdrproc_t) xdr_char;
		_xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) aggiungi_voto_1_svc;
		break;

	case RIMUOVI_VOTO:
		_xdr_argument = (xdrproc_t) xdr_char;
		_xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) rimuovi_voto_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (FATTOREX, FATTOREXVERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, FATTOREX, FATTOREXVERS, fattorex_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (FATTOREX, FATTOREXVERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, FATTOREX, FATTOREXVERS, fattorex_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (FATTOREX, FATTOREXVERS, tcp).");
		exit(1);
	}

	//inizializzazione struttura dati
	inizializza();
	//stampa di debug
	int i;
	for (i = 0; i < NUM_CANDIDATI; i++) {
		printf("%s; ", matrice.riga[i].candidato );
		printf("%s; ", matrice.riga[i].giudice );
		printf("%c; ", matrice.riga[i].categoria  );
		printf("%s; ", matrice.riga[i].nomeFile  );
		printf("%c; ", matrice.riga[i].fase );
		printf("%d;\n", matrice.riga[i].voti  );
    }

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
