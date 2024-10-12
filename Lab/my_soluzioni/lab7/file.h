/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _FILE_H_RPCGEN
#define _FILE_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct File {
	char nomeFile[50];
};
typedef struct File File;

struct RisultatoConteggio {
	int numRighe;
	int numParole;
	int numCaratteri;
};
typedef struct RisultatoConteggio RisultatoConteggio;

struct InputDirScan {
	File dir;
	int soglia;
};
typedef struct InputDirScan InputDirScan;

struct FileSopraSoglia {
	File files[8];
};
typedef struct FileSopraSoglia FileSopraSoglia;

struct OutputDirScan {
	int numerosita;
	FileSopraSoglia fileSopraSoglia;
};
typedef struct OutputDirScan OutputDirScan;

#define FILEPROG 0x20000013
#define FILEVERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define FILE_SCAN 1
extern  RisultatoConteggio * file_scan_1(File *, CLIENT *);
extern  RisultatoConteggio * file_scan_1_svc(File *, struct svc_req *);
#define DIR_SCAN 2
extern  OutputDirScan * dir_scan_1(InputDirScan *, CLIENT *);
extern  OutputDirScan * dir_scan_1_svc(InputDirScan *, struct svc_req *);
extern int fileprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define FILE_SCAN 1
extern  RisultatoConteggio * file_scan_1();
extern  RisultatoConteggio * file_scan_1_svc();
#define DIR_SCAN 2
extern  OutputDirScan * dir_scan_1();
extern  OutputDirScan * dir_scan_1_svc();
extern int fileprog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_File (XDR *, File*);
extern  bool_t xdr_RisultatoConteggio (XDR *, RisultatoConteggio*);
extern  bool_t xdr_InputDirScan (XDR *, InputDirScan*);
extern  bool_t xdr_FileSopraSoglia (XDR *, FileSopraSoglia*);
extern  bool_t xdr_OutputDirScan (XDR *, OutputDirScan*);

#else /* K&R C */
extern bool_t xdr_File ();
extern bool_t xdr_RisultatoConteggio ();
extern bool_t xdr_InputDirScan ();
extern bool_t xdr_FileSopraSoglia ();
extern bool_t xdr_OutputDirScan ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_FILE_H_RPCGEN */
