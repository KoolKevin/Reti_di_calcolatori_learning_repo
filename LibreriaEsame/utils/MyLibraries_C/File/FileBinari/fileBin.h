#define TIPO int

void writeDim(int n, char* nomeFile);	//scrive la numerosità degli elementi nel file come primo campo
void write(int n, char* nomeFile);		//scrive nel file della roba presa da input
int readDim(char* nomeFile);			//legge la numerosità
TIPO* readElementi(char* nomeFile);
void printRequests(TIPO* richieste, int dim);
