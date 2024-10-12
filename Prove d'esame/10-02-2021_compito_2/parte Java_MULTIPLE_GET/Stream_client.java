import java.net.*;
import java.io.*;
import java.util.List;

public class Stream_client {

    public static void main(String[] args) throws IOException {
        InetAddress addr = null;
        int port = -1;

        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        
        //controllo argomenti
        try {
            if (args.length == 1 || args.length == 2) {
                addr = InetAddress.getByName(args[0]);
                port = Integer.parseInt(args[1]);
                if (port < 1024 || port > 65535) {
                    System.out.println("Usage: java MPutFileClient serverAddr serverPort minFileSize [transferBufferSize]");
                    System.exit(1);
                }
            } else {
                System.out.println("Usage: java stream_client serverAddr serverPort");
                System.exit(1);
            }
        } catch (Exception e) {
            System.out.println("Problemi, i seguenti: ");
            e.printStackTrace();
            System.out.println("Usage: java stream_client serverAddr serverPort");
            System.exit(2);
        }

        // Variables for reading and transferring files
        String operazione;
        int buffer = 0;
        int cont = 0;
        long dimFile = 0;
        String nomeFile = null;
        FileOutputStream destination_stream = null;

        String modello = null;
        List<Noleggio> lista_risultato = null;

        String idDaValutare = null;
        float risultatoValutazione = -1;

        // Variables for sockets
        Socket socket = null;
        ObjectInputStream inSock = null;
        DataInputStream dataInSock = null;
        DataOutputStream outSock = null;

        //creazione socket e recpuero dei relativi stream
        try {
            socket = new Socket(addr, port);
            socket.setSoTimeout(30000);
            System.out.println("Creata la socket: " + socket);

            inSock = new ObjectInputStream(socket.getInputStream());
            dataInSock = new DataInputStream(socket.getInputStream());
            outSock = new DataOutputStream(socket.getOutputStream());
        } catch (IOException ioe) {
            System.out.println("Problemi nella creazione degli stream su socket: ");
            ioe.printStackTrace();
            System.exit(1);
        }

        /* INTERAZIONE CON L'UTENTE: cambiare le varibili in base all'esercizio */
        System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti l'operazine desiderata(recupera; valuta): ");
        while ((operazione = stdIn.readLine()) != null) {
            if(operazione.equals("recupera")) {
                try {
                    outSock.writeUTF(operazione);
                
                    System.out.print("immetti il modello di sci desiderato: ");
                    modello = stdIn.readLine();
                    //invio il modello
                    outSock.writeUTF(modello);
                    lista_risultato = (List<Noleggio>)inSock.readObject();

                    System.out.println("\nEcco i risultati: ");
                    for(Noleggio n : lista_risultato) {
                        System.out.println(n);
                    }
                    System.out.println();

                    //scarico le foto
                    while ( (dimFile = dataInSock.readLong()) > 0 ) {
                        nomeFile = dataInSock.readUTF();

                        System.out.println("\tscarico il file "+nomeFile+" di "+dimFile+" byte");

                        //creo il file
                        destination_stream = new FileOutputStream(nomeFile);

                        cont=0;
                        // esco dal ciclo quando ho letto il numero di byte da trasferire
                        while (cont < dimFile) {
                            /* trasferimento a blocchi */
                            //read_bytes = inSock.read(buffer);     //se leggo a blocchi dalla socket ho il problema di eccedere nell'altro file
                            //dest_stream.write(buffer, 0, read_bytes);
                            //cont += read_bytes;

                            /* trasferimento di un byte alla volta */
                            buffer = dataInSock.read();
                            destination_stream.write(buffer);
                            cont++;
                        }

                        destination_stream.flush();

                        System.out.println("Byte trasferiti: " + cont);
                    }
                } 
                // l'eccezione dovrebbe scattare solo se ci aspettiamo un numero sbagliato di byte da leggere
                catch (EOFException e) {
                    System.out.println("Problemi, i seguenti: ");
                    e.printStackTrace();
                    socket.close();
                    System.exit(1);
                } catch (SocketTimeoutException ste) {
                    System.out.println("Timeout scattato: ");
                    ste.printStackTrace();
                    socket.close();
                    System.exit(1);
                } catch (Exception e) {
                    System.out.println("Problemi, i seguenti : ");
                    e.printStackTrace();
                    System.out.println("Chiudo ed esco...");
                    socket.close();
                    System.exit(2);
                }
                

                if(dimFile == -1) {
                    System.out.println("trasferimento di tutte le foto terminato con successo");
                }

                if(dimFile == -2) {
                    System.out.println("errore nel trasferimento delle foto");
                }
            }
            else if(operazione.equals("valuta")) {
                outSock.writeUTF(operazione);

                System.out.print("immetti l'id del noleggio che vuoi valutare: ");
                idDaValutare = stdIn.readLine();

                outSock.writeUTF(idDaValutare);

                try {
                    risultatoValutazione = dataInSock.readFloat();
                } 
                // l'eccezione dovrebbe scattare solo se ci aspettiamo un numero sbagliato di byte da leggere
                catch (EOFException e) {
                    System.out.println("Problemi, i seguenti: ");
                    e.printStackTrace();
                    socket.close();
                    System.exit(1);
                } catch (SocketTimeoutException ste) {
                    System.out.println("Timeout scattato: ");
                    ste.printStackTrace();
                    socket.close();
                    System.exit(1);
                } catch (Exception e) {
                    System.out.println("Problemi, i seguenti : ");
                    e.printStackTrace();
                    System.out.println("Chiudo ed esco...");
                    socket.close();
                    System.exit(2);
                }

                if(risultatoValutazione != -1) {
                    System.out.println("il noleggio "+idDaValutare+" costerà "+risultatoValutazione+" euro!");
                }
                else{
                    System.out.println("noleggio non trovato o invalido");
                }
            }
            else{
                System.out.print("OPERAZIONE INVALIDA! ");
            }

            System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti l'operazine desiderata(recupera; valuta): ");
        }

        socket.close();
    }
}
