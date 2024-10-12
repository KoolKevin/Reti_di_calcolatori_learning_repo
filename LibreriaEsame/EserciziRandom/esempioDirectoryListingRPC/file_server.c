#include <sys/dir.h>

#include "file.h"

//extern int errno; //se la dichiaro extern poi la devo anche definire da qualche parte, se no il linker mi da errore
int errno;

readdir_res * readdir_1_svc (nametype * dirname, struct svc_req * rd) {
    DIR *dirp; struct direct *d; namelist nl;
    namelist * nlp; // NB: questo è un puntatore ad un puntatore di un nodo

    static readdir_res res;

    /* si libera la memoria della chiamata precedente */
    xdr_free((xdrproc_t) xdr_readdir_res, (caddr_t) &res);
    /* apertura di una directory */
    dirp = opendir(*dirname);
    
    if( dirp==NULL ) {
        res.errno=errno;
        return &res;
    }

    //nlp, puntatore all'ultimo nodo della lista; all'inizio punta alla lista del risultato
    nlp = &res.readdir_res_u.list;

    while (d=readdir(dirp)) {
        /* 
            Alloco memoria per un nuovo nodo della lista;
            Dato che uso *nlp:
                Se è il primo nodo creato verra puntato dal risultato;
                Se è un nodo successivo verra puntato da un nodo precedente;
         */
        nl = *nlp = (namenode*) malloc(sizeof(namenode));

        //alloco memoria per il nome della directory
        nl->name = malloc(strlen(d->d_name)+1);

        //popolo il nodo con il nome della directory
        strcpy(nl->name, d->d_name);
        //sposto nlp al prossimo elemento della lista
        nlp = &nl->next;
    }

    /* chiusura della lista con un puntatore a NULL */
    *nlp=NULL;
    res.errno=0;
    /* chiusura directory */
    closedir(dirp);

    return &res;
    
}