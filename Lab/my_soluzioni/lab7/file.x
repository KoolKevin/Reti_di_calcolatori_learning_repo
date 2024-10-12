/* 
 * operazioni.x
 *	Questo file definisce la struttura Operandi e i relativi metodi remoti.
 * 	Poi si dovra' procedere come segue:
 *  rpcgen operazioni.x --> genera i seguenti file:
 *		1) operazioni.h --> libreria da includere in operazioni_proc.c
 *					e operazioni_client.c.
 *		2) operazioni_xdr.c --> routine di conversione.
 * 		3) operazioni_clnt.c; operazioni_svc.c --> stub C e S.
 */

/*strutture dati per file_scan*/
struct File{
	char nomeFile[50];
};

struct RisultatoConteggio{
	int numRighe;
	int numParole;
	int numCaratteri;
};

/*strutture dati per dir_scan*/
struct InputDirScan{
	File dir;
	int soglia;
};

struct FileSopraSoglia{
	File files[8];
};

struct OutputDirScan{
	int numerosita;
	FileSopraSoglia fileSopraSoglia;
};




program FILEPROG {
	version FILEVERSION {
		RisultatoConteggio FILE_SCAN(File) = 1;
		OutputDirScan DIR_SCAN(InputDirScan) = 2;
	} = 1;
} = 0x20000013;


