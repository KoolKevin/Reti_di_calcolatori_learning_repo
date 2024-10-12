/* qua sarebbe più scomodo usare Filename al posto di un array perchè dopo per riferire il nome del direttorio dovrei fare: x.fileName.direttorio al posto di x.direttorio */
struct Input{
	char direttorio[256];
	int soglia;
};

/* è comodo anche aggiungere un codice di errore alle strutture che ritornano */
struct OutputFileScan {
	int caratteri;
	int parole;
	int linee;
  	int codiceErrore;
};

/* gli array non possono venire passati direttamente come argomento/ritorno */
struct FileName{
	char name[256];
};

/* quando abbiamo un risultato che ha dimensione dinamica è più semplice dare una dimensione massima ed aggiungere nella struct la dimensione effettiva */
struct OutputDirScan {
	int nb_files;
	FileName files[8];	
};

program CONTAPROG {
	version CONTAVERS {
    	OutputFileScan FILE_SCAN(FileName) = 1;
		OutputDirScan DIR_SCAN(Input) = 2;
	} = 1;
} = 0x20000015;
