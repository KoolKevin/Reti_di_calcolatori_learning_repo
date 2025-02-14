/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "RentACar.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

prenotazioni_cercate *
visualizza_prenotazioni_1(tipoVeicolo *argp, CLIENT *clnt)
{
	static prenotazioni_cercate clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, VISUALIZZA_PRENOTAZIONI,
		(xdrproc_t) xdr_tipoVeicolo, (caddr_t) argp,
		(xdrproc_t) xdr_prenotazioni_cercate, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

int *
aggiorna_licenza_1(input_aggiorna_licenza *argp, CLIENT *clnt)
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, AGGIORNA_LICENZA,
		(xdrproc_t) xdr_input_aggiorna_licenza, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
