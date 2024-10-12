const MAXNAMELEN = 255;
const NUM_GIUDICI = 12;

typedef string nametype <MAXNAMELEN>;

/* STRUTTURA CLASSIFICA  */
struct classifica_entry{
	nametype nomeGiudice; 
	int punteggioTot;
}; 

struct classifica_giudici{
	classifica_entry classifica[NUM_GIUDICI]; 
}; 

/* STRUTTURA LISTA DEI CANDIDATI */
struct candidato_entry{
	nametype nomeCandidato; 
	nametype nomeGiudice; 
	char categoria;
	nametype nomeFile;
	char fase;
	int voti;
}; 

typedef struct candidati_node *candidati_list;
struct candidati_node {candidato_entry entry ; candidati_list next;};

struct input_espressione_voto{
	nametype nomeCandidato;
	char tipoOp;
};
  
program OPERATION {
	version OPERATIONVERS {         
		classifica_giudici CLASSIFICA_GIUDICI(void) = 1;        
		int ESPRIMI_VOTO(input_espressione_voto) = 2;
		candidati_list VISUALIZZA_CANDIDATI(void) = 3;
		int AGGIUNGI_CANDIDATO(candidato_entry) = 4;
		int RIMUOVI_CANDIDATO(nametype) = 5;	/* uso il nome del candidato come chiave */
		candidati_list GET_CANDIDATI_OF_FASE(char) = 6;
		candidati_list GET_CANDIDATI_OF_CATEGORIA(char) = 7;
		candidati_list GET_CANDIDATI_WITH_VOTI(int) = 8;
	} = 1;
} = 0x20000013;

