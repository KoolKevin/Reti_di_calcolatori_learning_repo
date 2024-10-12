/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _CONTA_H_RPCGEN
#define _CONTA_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct Input {
	char direttorio[256];
	int soglia;
};
typedef struct Input Input;

struct OutputFileScan {
	int caratteri;
	int parole;
	int linee;
	int codiceErrore;
};
typedef struct OutputFileScan OutputFileScan;

struct FileName {
	char name[256];
};
typedef struct FileName FileName;

struct OutputDirScan {
	int nb_files;
	FileName files[8];
};
typedef struct OutputDirScan OutputDirScan;

#define CONTAPROG 0x20000015
#define CONTAVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define FILE_SCAN 1
extern  OutputFileScan * file_scan_1(FileName *, CLIENT *);
extern  OutputFileScan * file_scan_1_svc(FileName *, struct svc_req *);
#define DIR_SCAN 2
extern  OutputDirScan * dir_scan_1(Input *, CLIENT *);
extern  OutputDirScan * dir_scan_1_svc(Input *, struct svc_req *);
extern int contaprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define FILE_SCAN 1
extern  OutputFileScan * file_scan_1();
extern  OutputFileScan * file_scan_1_svc();
#define DIR_SCAN 2
extern  OutputDirScan * dir_scan_1();
extern  OutputDirScan * dir_scan_1_svc();
extern int contaprog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Input (XDR *, Input*);
extern  bool_t xdr_OutputFileScan (XDR *, OutputFileScan*);
extern  bool_t xdr_FileName (XDR *, FileName*);
extern  bool_t xdr_OutputDirScan (XDR *, OutputDirScan*);

#else /* K&R C */
extern bool_t xdr_Input ();
extern bool_t xdr_OutputFileScan ();
extern bool_t xdr_FileName ();
extern bool_t xdr_OutputDirScan ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_CONTA_H_RPCGEN */
