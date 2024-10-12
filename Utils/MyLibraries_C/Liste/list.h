#include"elements.h"

typedef struct list_element {
	element value;
	struct list_element* next;
}item;

typedef item* list;


list emptyList();	//ritorna la lista vuota
Boolean empty(list l);	//vede se la lista è vuota
element head(list l);	//ritorna il valore della testa
list tail(list l);	//ritorna la coda
list cons(list l, element e);	//aggiunge un elemento in testa

void showlist(list l);	//stampa lista
int length(list l);		//ritorna la lunghezza della lista
list copy(list l);	//copia la lista(utile per evitare structure sharing)
list deleteElement(list l, element e);	//cancella un elemento dato
void deleteList(list l);	//questa funzione va integrata eguagliando la root della lista cancellata ad emptyList() dato che la free fa rimanere roba

