const NUM_CANDIDATI=10;
const MAX_LENGTH=255;

struct entry_classifica{
	char name[MAX_LENGTH];
	int num_voti;
};

struct classifica_giudici{
	entry_classifica classifica[NUM_CANDIDATI];
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
	RIGA riga[NUM_CANDIDATI];
};


program FATTOREX {
	version FATTOREXVERS{
		classifica_giudici VISUALIZZA_CLASSIFICA(void) = 1;
		void  AGGIUNGI_VOTO(char*) = 2;
		void  RIMUOVI_VOTO(char*) = 3;
	} = 1;
} = 0x20000013;
