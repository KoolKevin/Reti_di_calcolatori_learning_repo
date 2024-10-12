/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "sala.h"
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

static void
sala_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		Input prenota_postazione_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case PRENOTA_POSTAZIONE:
		_xdr_argument = (xdrproc_t) xdr_Input;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) prenota_postazione_1_svc;
		break;

	case VISUALIZZA_STATO:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_Sala;
		local = (char *(*)(char *, struct svc_req *)) visualizza_stato_1_svc;
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

/* dichiarazione metodo di inizializzione */
void inizializza();

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (SALA, SALAVERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, SALA, SALAVERS, sala_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (SALA, SALAVERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, SALA, SALAVERS, sala_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (SALA, SALAVERS, tcp).");
		exit(1);
	}

	/* inizializzazione della struttura dati */
	// NB: la struttura dati è sala è static, quindi visibile solo all'interno del file in cui è definita.
	// a quanto pare le funzioni eseguono nell'environment in cui sono definite!
	// Quindi in questo caso seppur io stia chiamando inizializza all'interno dello stub, in cui NON ho visibilità della sala,
	// inizializza() riesce a vederla lo stesso per questo motivo.
	inizializza();

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
