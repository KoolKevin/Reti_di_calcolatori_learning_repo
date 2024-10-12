#define _CRT_SECURE_NO_DEPRECATE

#include<stdio.h>
#include<stdlib.h>

#include "fileBin.h"

void writeDim(int n, char* nomeFile) {
	FILE* f;

	f = fopen(nomeFile, "wb");

	if (f == NULL) {
		perror("scrittura della dimensione andata male");
		exit(-20);
	}

	fwrite(&n, sizeof(int), 1, f);
	fclose(f);
}

void write(int n, char* nomeFile) {
	FILE* f;

	f = fopen(nomeFile, "ab");

	if (f == NULL) {
		perror("scrittura andata male");
		exit(-30);
	}
	else {
		//variabili che ti servono in base all'es
		/*int cod;
		int min;*/

		printf("inserisi le %d richieste:\n", n);

		for (int i = 0; i < n; i++) {
			printf("inserici ||(quello che serve)|| %d ):", i+1);
			//scanf("%d %d", &cod, &min);

			//fwrite(&cod, sizeof(int), 1, f);
			//fwrite(&min, sizeof(int), 1, f);
		}
	}
	
	fclose(f);
}

int readDim(char* nomeFile) {
	FILE* f;
	int dim;

	f = fopen(nomeFile, "rb");

	if (f == NULL) {
		perror("apertura per leggere la dimensione delle richieste andata male");
		exit(-40);
	}

	fread(&dim, sizeof(int), 1, f);
	fclose(f);

	return dim;
}

TIPO* readElementi(char* nomeFile) {
	FILE* f;
	int dim;
	TIPO* elementi;

	f = fopen(nomeFile, "rb");

	if (f == NULL) {
		perror("apertura per leggere gli elementi andata male");
		exit(-50);
	}

	fread(&dim, sizeof(int), 1, f);	//leggo la dimensione per l'allocazione dinamica ed anche perchè così levo la numerosità dallo stream

	elementi = (TIPO*)malloc(sizeof(TIPO) * dim);

	for (int i = 0; i < dim; i++) {
		fread(&elementi[i], sizeof(int), 1, f);

		//se stai lavorando con una struttura fai come sotto

		//fread(&richieste[i].cod, sizeof(int), 1, f);
		//fread(&richieste[i].durata, sizeof(int), 1, f);
	}

	fclose(f);

	return elementi;
}

void printRequests(TIPO* richieste, int dim) {
	for (int i = 0; i < dim; i++) {
		printf("richiesta %d:\n", i+1);

		//anche qua c'è da cambiare di volta in volta
		//printf("codice: %d   durata: %d\n", richieste[i].cod, richieste[i].durata);
	}
}


