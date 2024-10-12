#include <stdio.h>
#include <rpc/rpc.h>
#include "file.h"

int main (int argc, char **argv) {
    CLIENT *cl; namelist nl;
    char *server; char *dir;
    readdir_res *result;

    if(argc!=3) { 
        fprintf(stderr,"uso: %s <host> <dir>\n",argv[0]);
        exit(1);
    }

    server = argv[1];
    dir = argv[2];

    cl = clnt_create(server, RLSPROG, RLSVERS, "tcp");  //tcp perchè potrebbero essere tanti
    if (cl==NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }

    result= readdir_1(&dir,cl);
    if (result==NULL) {
        clnt_perror(cl,server);
        exit(1);
    }

    if (result->errno!=0) {
        perror (dir);
        exit(1);
    }
    /* altrimenti stampa risultati */
    for (nl=result->readdir_res_u.list; nl != NULL; nl = nl->next ) {
        printf("%s\n",nl->name);
    }

    
}
