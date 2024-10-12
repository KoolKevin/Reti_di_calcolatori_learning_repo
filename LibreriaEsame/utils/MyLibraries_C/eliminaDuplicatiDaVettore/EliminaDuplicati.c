#ifndef FLAG
	#define FLAG

	#include "EliminaDuplicati.h"
	#include<stdlib.h>
#endif
int compare(TIPO a, TIPO b) {	// da definire in base al tipo
	if (a == b)
		return 1;
	else
		return 0;
}

TIPO* eliminaDuplicati(Spesa* v, int dim, int* dimSenzaDup) {
	TIPO* vSenzaDup;
	int trovato = 0;

	vSenzaDup = (TIPO*)malloc(sizeof(TIPO) * dim);

	*dimSenzaDup = 0;


	for (int i = 0; i < dim; i++) {
		trovato = 0;
		for (int j = 0; j < *dimSenzaDup && !trovato; j++) {
			if (compare(v[i], v[j]) == 1) {
				trovato = 1;
			}
		}

		if (!trovato) {
			vSenzaDup[*dimSenzaDup] = v[i];
			*dimSenzaDup = *dimSenzaDup + 1;
		}
	}

	return vSenzaDup;
}
