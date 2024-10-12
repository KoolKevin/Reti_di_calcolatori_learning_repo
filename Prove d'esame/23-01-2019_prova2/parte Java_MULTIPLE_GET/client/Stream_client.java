import java.net.*;
import java.io.*;

public class Stream_client {

    public static void main(String[] args) throws IOException {
        InetAddress addr = null;
        int port = -1;

        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

        /* VARIABILI DA CAMBIARE IN BASE ALL'ESERCIZIO */
        String nomeFolder = null;
        int dim_min = 0;
        /* ------------------------------------------- */

        // dimensione del buffer di lettura
        int buffer_size = 4096;

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

        String targaDaEliminare = null;
        int esitoEliminazione = -1;

        // Variables for sockets
        Socket socket = null;
        DataInputStream inSock = null;
        DataOutputStream outSock = null;

        //creazione socket e recpuero dei relativi stream
        try {
            socket = new Socket(addr, port);
            socket.setSoTimeout(30000);
            System.out.println("Creata la socket: " + socket);

            inSock = new DataInputStream(socket.getInputStream());
            outSock = new DataOutputStream(socket.getOutputStream());
        } catch (IOException ioe) {
            System.out.println("Problemi nella creazione degli stream su socket: ");
            ioe.printStackTrace();
            System.exit(1);
        }

        /* INTERAZIONE CON L'UTENTE: cambiare le varibili in base all'esercizio */
        System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti l'operazine desiderata(get; delete): ");
        while ((operazione = stdIn.readLine()) != null) {
            if(operazione.equals("get")) {
                outSock.writeUTF(operazione);
                
                System.out.print("immetti il nome della cartella: ");
                nomeFolder = stdIn.readLine();
                //invio il nome della directory che voglio
                outSock.writeUTF(nomeFolder);

                try {
                    while ( (dimFile = inSock.readLong()) > 0 ) {
                        System.out.println(dimFile + "ricevuti");
                            
                        nomeFile = inSock.readUTF();

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
                            buffer = inSock.read();
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
                    System.out.println("trasferimento terminato");
                }

                if(dimFile == -2) {
                    System.out.println("direttorio non trovato");
                }
            }
            else if(operazione.equals("delete")) {
                outSock.writeUTF(operazione);

                System.out.print("immetti la targa del veicolo che vuoi eliminare: ");
                targaDaEliminare = stdIn.readLine();

                outSock.writeUTF(targaDaEliminare);

                try {
                    esitoEliminazione = inSock.readInt();
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

                if(esitoEliminazione == 0) {
                    System.out.println("eliminazione avvenuta con successo");
                }
                else{
                    System.out.println("targa non trovata");
                }
            }
            else{
                System.out.print("OPERAZIONE INVALIDA! ");
            }

            System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti l'operazine desiderata(get; delete): ");
        }

        socket.close();
    }
}
