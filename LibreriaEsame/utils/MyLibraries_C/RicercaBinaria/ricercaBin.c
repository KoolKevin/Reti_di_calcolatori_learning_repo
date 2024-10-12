#include"ricercaBin.h"

int compare(TIPO a, TIPO b) {	//positivo se a > b, negativo per a < b, null per a == b
	//questo è da modificare per tipi diversi
	if (a > b) {
		return 1;
	}
	else if (a == b) {
		return 0;
	}
	else {
		return -1;
	}
}


int ricercaBin(TIPO vet[], int first, int last, TIPO el) {
	int med;

	if (first > last)
		return -1;
	else {
		med = (first + last) / 2;

		if ( compare(el, vet[med]) == 0 ) {
			return med;
		}
		else {
			if ( compare(el, vet[med]) > 0 )
				return ricercaBin(vet, med + 1, last, el);
			else
				return ricercaBin(vet, first, med - 1, el);
		}
	}
}