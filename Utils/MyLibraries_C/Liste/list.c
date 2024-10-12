#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "list.h"


list emptyList() {
	return NULL;
}

Boolean empty(list l) {
	if (l == NULL) {
		return TRUE;
	}
	else return FALSE;
}

element head(list l) {
	if (empty(l))
		exit(-1);
	else
		return l->value;
}

list tail(list l) {
	if (empty(l))
		exit(-1);
	else
		return l->next;
}

list cons(list l, element e) {
	list temp;

	temp = (list)malloc(sizeof(item));

	temp->value = e;
	temp->next = l;

	return temp;
}

void showlist(list l) {
	if (empty(l)) {
		printf("lista vuota else");
	}
	else {
		while (!empty(l)) {
			printf("%d\n", head(l));
			l = tail(l);
		}
	}
}

int length(list l) {
	if (empty(l)) {
		return 0;
	}
	else {
		return 1 + length( l->next );
	}
}

list copy(list l) {
	if (empty(l)) {
		return l;
	}
	else {
		return cons(copy(tail(l)), head(l));
	}
}

list deleteElement(list l, element e) {
	if (l == NULL) {
		return NULL;
	}
	else {
		if (e == head(l)) {	//per element particolari c'è bisogno di una funzione equals
			list restoDellaLista = copy(tail(l));
			return restoDellaLista;
		}
		else {
			return cons(deleteElement(tail(l), e), head(l));
		}
	}
}

void deleteList(list l) {
	if (l == NULL) {
		return;
	}
	else {
		deleteList(tail(l));
		free(l);
	}
}