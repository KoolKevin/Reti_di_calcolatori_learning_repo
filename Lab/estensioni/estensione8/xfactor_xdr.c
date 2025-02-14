/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "xfactor.h"

bool_t
xdr_nametype (XDR *xdrs, nametype *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, MAXNAMELEN))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_classifica_entry (XDR *xdrs, classifica_entry *objp)
{
	register int32_t *buf;

	 if (!xdr_nametype (xdrs, &objp->nomeGiudice))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->punteggioTot))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_classifica_giudici (XDR *xdrs, classifica_giudici *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->classifica, NUM_GIUDICI,
		sizeof (classifica_entry), (xdrproc_t) xdr_classifica_entry))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_candidato_entry (XDR *xdrs, candidato_entry *objp)
{
	register int32_t *buf;

	 if (!xdr_nametype (xdrs, &objp->nomeCandidato))
		 return FALSE;
	 if (!xdr_nametype (xdrs, &objp->nomeGiudice))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->categoria))
		 return FALSE;
	 if (!xdr_nametype (xdrs, &objp->nomeFile))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->fase))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->voti))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_candidati_list (XDR *xdrs, candidati_list *objp)
{
	register int32_t *buf;

	 if (!xdr_pointer (xdrs, (char **)objp, sizeof (struct candidati_node), (xdrproc_t) xdr_candidati_node))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_candidati_node (XDR *xdrs, candidati_node *objp)
{
	register int32_t *buf;

	 if (!xdr_candidato_entry (xdrs, &objp->entry))
		 return FALSE;
	 if (!xdr_candidati_list (xdrs, &objp->next))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_input_espressione_voto (XDR *xdrs, input_espressione_voto *objp)
{
	register int32_t *buf;

	 if (!xdr_nametype (xdrs, &objp->nomeCandidato))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->tipoOp))
		 return FALSE;
	return TRUE;
}
