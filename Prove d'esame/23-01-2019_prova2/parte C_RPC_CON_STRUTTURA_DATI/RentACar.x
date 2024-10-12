const FILE_NAME_DIM=50;
const STRING_DIM=50;
const PATENTE_DIM=6;

const MAX_PRENOTAZIONI=100;	
const MAX_PRENOTAZIONI_CERCATE=6;


/* DEFINIZIONE STRUTTURA DATI */


struct prenotazione{
	char targa[STRING_DIM];
	char patente[PATENTE_DIM];
	char tipo[STRING_DIM];
	char direttorio_img[FILE_NAME_DIM];
};

/* per avere una struttura dati dinamica bisogna avere una implementazione a lista linkata */
struct prenotazioni{
	prenotazione entry[MAX_PRENOTAZIONI];
};


/* DEFINIZIONE TIPI di INPUT/OUTPUT delle PROCEDURE */


struct tipoVeicolo{
	char tipo[STRING_DIM];
};

struct prenotazioni_cercate{
	prenotazione entry[MAX_PRENOTAZIONI_CERCATE];
};

struct input_aggiorna_licenza{
	char targa[STRING_DIM];
	char patente_nuova[PATENTE_DIM];
};


/* PROGRAMMA */


program RENT_A_CAR {
	version RENT_A_CAR_VERS{
		prenotazioni_cercate VISUALIZZA_PRENOTAZIONI(tipoVeicolo) = 1;
		int AGGIORNA_LICENZA(input_aggiorna_licenza) = 2;
	} = 1;
} = 0x20000013;
