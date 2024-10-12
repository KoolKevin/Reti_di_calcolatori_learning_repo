const FILE_NAME_DIM=50;
const STRING_DIM=50;
const DATA_DIM=11;

const MAX_NOLEGGI=100;	


/* DEFINIZIONE STRUTTURA DATI */


struct entry{
	char id[STRING_DIM];
	char data[DATA_DIM];
	int giorni_noleggio;
	char modello[STRING_DIM];
	float costo_giornaliero;
	char nome_file_foto[FILE_NAME_DIM];
};

/* per avere una struttura dati dinamica bisogna avere una implementazione a lista linkata */
struct noleggi{
	entry noleggio[MAX_NOLEGGI];
};


/* DEFINIZIONE TIPI di INPUT/OUTPUT delle PROCEDURE */


struct input_elimina_sci{
	char id[STRING_DIM];
};


struct input_noleggia_sci{
	char id[STRING_DIM];
	char data[DATA_DIM];
	int giorni_noleggio;
};


/* PROGRAMMA */


program NOLEGGIA_SCI {
	version NOLEGGIA_SCI_VERS{
		int ELIMINA_SCI(input_elimina_sci) = 1;
		int NOLEGGIO_SCI(input_noleggia_sci) = 2;
	} = 1;
} = 0x20000013;
