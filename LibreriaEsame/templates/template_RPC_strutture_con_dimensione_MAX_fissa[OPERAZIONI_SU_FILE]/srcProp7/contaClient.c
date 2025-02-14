#include "conta.h"
#include <rpc/rpc.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    CLIENT         *clnt;
    int            *ris;
    OutputFileScan *out_fs;
    OutputDirScan  *out_ds;
    char           *server;
    char            car, ok[128];

    /* IMPORTANTE */
    // The variables used for RPC must be NOT stored on the local stack,
    // otherwise they are inaccessible from the stubs. So, we must either
    //  (a) allocate them using malloc (e.g., strings), or
    //  (b) declare them static
    static FileName nome_file;
    static Input    input;

    /* RECUPERO INDIRIZZO DEL SERVER */
    if (argc != 2) {
        fprintf(stderr, "uso: %s nomehost\n", argv[0]);
        exit(1);
    }
    server = argv[1];

    clnt = clnt_create(server, CONTAPROG, CONTAVERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }

    /* INTERAZIONE CON L'UTENTE */
    printf("operazioni:  CS=Conta File maggiori di, SF=Scan File remoto\n");
    while (gets(ok)) {
        printf("Richiesto servizio: %s\n", ok);

        if (strcmp(ok, "CS") == 0) {
            /* RECUPERO I PARAMETRI */
            printf("inserisci il nome direttorio: \n");
            gets(input.direttorio);
            printf("inserisci la soglia: \n");

            // controllo intero
            while (scanf("%d", &input.soglia) != 1) {
                do {
                    car = getchar();
                    printf("%c ", car);
                } while (car != '\n');
                printf("Inserire int");
                continue;
            }
            gets(ok);

            /* INVOCAZIONE */
            out_ds = dir_scan_1(&input, clnt);

            /* ELABORAZIONE DEL RISULTATO */
            if (out_ds == NULL) {
                clnt_perror(clnt, "E' avvenuto un errore lato server");
            } else if (out_ds->nb_files == -1) {
                printf("Il direttorio richiesto non esiste sul server!\n");
            } else {
                printf("Ho contato %d file con dim >= %d:\n", out_ds->nb_files, input.soglia);
                for (int j = 0; j < out_ds->nb_files; j++) {
                    printf("%s\n", out_ds->files[j].name);
                }
            }

        } // CS
        else if (strcmp(ok, "SF") == 0) {
            /* RECUPERO I PARAMETRI */
            printf("inserisci il nome del file: \n");
            gets(nome_file.name);

            /* INVOCAZIONE */
            out_fs = file_scan_1(&nome_file, clnt);

            /* ELABORAZIONE DEL RISULTATO */
            if (out_fs == NULL) {
                clnt_perror(clnt, "E' avvenuto un errore lato server");
            } else if (out_fs->codiceErrore == -1) {
                printf("Il file richiesto non esiste sul server!\n");
            } else {
                printf("Ho contato %d caratteri, %d parole e %d linee !\n", out_fs->caratteri,
                       out_fs->parole, out_fs->linee);
            }

        } // CF
        else {
            printf("Servizio non supportato!\n");
        }
        
        printf("operazioni:  CS=Conta File maggiori di, SF=Scan File remoto\n");
    } // while

    clnt_destroy(clnt);
    printf("Esco dal client\n");
}
