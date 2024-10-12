const NUMCANDIDATI=10;
const MAX_LENGTH=255;

struct nodo_giudice{
	char name[MAX_LENGTH];
	nodo_giudice * next;
};

struct RIGA{
	char candidato[MAX_LENGTH];
	char giudice[MAX_LENGTH];
	char categoria;
	char nomeFile[MAX_LENGTH];
	char fase;
	int voti;
};

struct MATRICE {
	RIGA riga[NUMCANDIDATI];
};


program FATTOREX {
	version FATTOREXVERS{
		nodo_giudice VISUALIZZA_CLASSIFICA(void) = 1;
		void  AGGIUNGI_VOTO(char*) = 2;
		void  RIMUOVI_VOTO(char*) = 3;
	} = 1;
} = 0x20000013;
