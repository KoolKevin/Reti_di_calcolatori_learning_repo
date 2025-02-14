/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "file.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

RisultatoConteggio *
file_scan_1(File *argp, CLIENT *clnt)
{
	static RisultatoConteggio clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, FILE_SCAN,
		(xdrproc_t) xdr_File, (caddr_t) argp,
		(xdrproc_t) xdr_RisultatoConteggio, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {

		return (NULL);
	}
	return (&clnt_res);
}

OutputDirScan *
dir_scan_1(InputDirScan *argp, CLIENT *clnt)
{
	static OutputDirScan clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, DIR_SCAN,
		(xdrproc_t) xdr_InputDirScan, (caddr_t) argp,
		(xdrproc_t) xdr_OutputDirScan, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
			
		return (NULL);
	}
	return (&clnt_res);
}
