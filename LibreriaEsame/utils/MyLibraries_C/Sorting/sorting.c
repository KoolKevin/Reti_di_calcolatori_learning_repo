#include "sorting.h"

//stampa vettore
void stampaVettore(TIPO v[], int dim) {
	for (int i = 0; i < dim; i++) {
		//devi cambiarlo ogni volta
		printf("elemento numero %d del vettore: %d\n", i + 1, v[i]);
	}
}

//scambia per naive sort, e bubble sort
void scambia(TIPO* a, TIPO* b) {
	//per tipi più sofisticati c'è da lavorarci
	TIPO tmp = *a;
	*a = *b;
	*b = tmp;
}

//compare
int compare(TIPO t1, TIPO t2) {	//dato che ordino in ordine crescente ritorna 1 se t1 è minore del secondo, per ordinare in modo decrescente fai t1 > t2
	//se cambi tipo dovrai anche cambiare la logica del compare ovviamente
	if (t1 < t2) {
		return 1;
	}
	else {
		return 0;
	}
}



//naiveSort
int trovaPosMax(TIPO v[], int n) {
	int i, posMax = 0;

	for (i = 1; i < n; i++) {
		if ( compare(v[posMax], v[i]) ) {
			posMax = i;
		}
	}

	return posMax;
}

void naiveSort(TIPO v[], int dim) {
	int p;

	while (dim > 1) {
		p = trovaPosMax(v, dim);

		if (p < dim - 1) {
			scambia(&v[p], &v[dim - 1]);
		}

		dim--;
	}
}

//bubbleSort
void bubbleSort(TIPO v[], int dim) {
	int i, ordinato = 0;

	while (dim > 1 && !ordinato) {
		ordinato = 1;

		for (i = 0; i < dim - 1; i++) {
			if ( compare(v[i], v[i + 1]) ) {	//per il bubble sort il compare funziona al contrario non ho voglia di farne uno apposta quindi mi arrangio così
				scambia(&v[i], &v[i + 1]);
				ordinato = 0;
			}
		}

		dim--;
	}
}

//insertSort
void insOrd(TIPO v[], int pos) {
	int i = pos - 1;
	int x = v[pos];

	while (i >= 0 && compare(x, v[i]) ) {
		v[i + 1] = v[i]; /* crea lo spazio */
		i--;
	}

	v[i + 1] = x; /* inserisce l’elemento */
}

void insertSort(TIPO v[], int dim) {
	int k;

	for (k = 1; k < dim; k++) {
		insOrd(v, k);
	}
}


//mergeSort
void merge(TIPO v[], int i1, int i2, int fine, TIPO vTemp[]) {
	int i = i1, j = i2, k = i1;

	while (i <= i2 - 1 && j <= fine) {
		if (compare(v[i], v[j])) {
			vTemp[k] = v[i];
			i++;
		}
		else {
			vTemp[k] = v[j];
			j++;
		}

		k++;
	}

	while (i <= i2 - 1) { vTemp[k] = v[i++]; k++; }
	while (j <= fine) { vTemp[k] = v[j++]; k++; }

	for (i = i1; i <= fine; i++)
		v[i] = vTemp[i];
}

void mergeSort(TIPO v[], int first, int last, TIPO vTemp[]) {
	int mid;

	if (first < last) {
		mid = (last + first) / 2;
		mergeSort(v, first, mid, vTemp);
		mergeSort(v, mid + 1, last, vTemp);
		merge(v, first, mid + 1, last, vTemp);
	}
}

//quick sort
void quickSort(TIPO a[], int dim) {
	quickSortR(a, 0, dim - 1);
}

void quickSortR(TIPO a[], int iniz, int fine) {
	int i, j, iPivot;
	TIPO pivot;

	if (iniz < fine) {
		i = iniz;
		j = fine;
		iPivot = fine;
		pivot = a[iPivot];

		do{  /* trova la posizione del pivot */
			while (i < j && compare(a[i], pivot) ) //se vuoi invertire il senso dell' ordinamento scambia questo !
				i++;
			while (j > i && !compare(a[j], pivot) ) 
				j--;

			if (i < j) 
				scambia(&a[i], &a[j]);
		} while (i < j);

		/* posiziona il pivot */
		if (i != iPivot && a[i] != a[iPivot]) {
			scambia(&a[i], &a[iPivot]);
			iPivot = i;
		}

		/* ricorsione sulle sottoparti, se necessario */
		if (iniz < iPivot - 1)
			quickSortR(a, iniz, iPivot - 1);
		if (iPivot + 1 < fine)
			quickSortR(a, iPivot + 1, fine);
	} 
} 


