/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "fattorex.h"

bool_t
xdr_nodo_giudice (XDR *xdrs, nodo_giudice *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->name, MAX_LENGTH,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->next, sizeof (nodo_giudice), (xdrproc_t) xdr_nodo_giudice))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_RIGA (XDR *xdrs, RIGA *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->candidato, MAX_LENGTH,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->giudice, MAX_LENGTH,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->categoria))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->nomeFile, MAX_LENGTH,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->fase))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->voti))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_MATRICE (XDR *xdrs, MATRICE *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->riga, NUMCANDIDATI,
		sizeof (RIGA), (xdrproc_t) xdr_RIGA))
		 return FALSE;
	return TRUE;
}
