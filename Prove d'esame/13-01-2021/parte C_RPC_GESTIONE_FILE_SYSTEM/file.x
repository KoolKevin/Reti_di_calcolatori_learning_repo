const FILE_NAME_DIM=50;

const MAX_SOTTODIRETTORI=6;


/* DEFINIZIONE TIPI di INPUT/OUTPUT delle PROCEDURE */
struct nome_file{
	char nome[FILE_NAME_DIM];
};

struct output_lista_sottodirettori{
	nome_file sottodirettori[MAX_SOTTODIRETTORI];
	int valido;
};


/* PROGRAMMA */
program FILES {
	version FILES_VERS{
		int ELIMINA_OCCORRENZE(nome_file) = 1;
		output_lista_sottodirettori LISTA_SOTTODIRETTORI(nome_file) = 2;
	} = 1;
} = 0x20000013;
