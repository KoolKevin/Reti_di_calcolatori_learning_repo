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
        byte[] buffer = new byte[buffer_size];
        int cont = 0;
        int read_bytes = 0;
        long daTrasferire = 0;
        DataInputStream src_stream = null;

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
        System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti il nome della cartella: ");
        while ((nomeFolder = stdIn.readLine()) != null) {
            File dirCorr = new File(nomeFolder);

            if (dirCorr.exists() && dirCorr.isDirectory()) {
                File[] files = dirCorr.listFiles();

                //scorro tutti i file della directory
                for (int i = 0; i < files.length; i++) {
                    File fileCorr = files[i];
                    
                    // realizzazione protocollo
                    if (fileCorr.isFile() && dim_min <= fileCorr.length()) {
                        //invio nome file al server
                        System.out.println("File con nome: " + fileCorr.getName());
                        outSock.writeUTF(fileCorr.getName());
                        //recupero esito dal server
                        String result = inSock.readUTF();

                        //in base all'esito vedo cosa fare
                        if (result.equals("attiva")) {
                            System.out.println("Il file " + fileCorr.getName() + " NON e' presente sul server: inizio il trasferimento");
                            daTrasferire = fileCorr.length();
                            outSock.writeLong(daTrasferire);

                            /* ------------------ TRASFERIMENTO DEL FILE ------------------ */
                            src_stream = new DataInputStream(new FileInputStream(fileCorr.getAbsolutePath()));

                            try {
                                // esco dal ciclo quando ho letto il numero di byte da trasferire
                                cont = 0;
                                while (cont < daTrasferire) {
                                    read_bytes = src_stream.read(buffer);
                                    outSock.write(buffer, 0, read_bytes);
                                    cont += read_bytes;
                                }
                                
                                outSock.flush();
                                System.out.println("Byte trasferiti: " + cont);
                            }
                            // l'eccezione dovrebbe scattare solo se ci aspettiamo un numero sbagliato di byte da leggere
                            catch (EOFException e) {
                                System.out.println("Problemi, i seguenti: ");
                                e.printStackTrace();
                            }
                            /* ------------------------------------------------------------- */

                        } else if (result.equals("salta file"))
                            System.out.println("Il file " + fileCorr.getName() + " era gia' presente sul server e non e' stato sovrascritto");
                        else {
                            System.out.println("Stream client: violazione protocollo...");
                            System.exit(4);
                        }
                    } else {
                        System.out.println("File saltato");
                    }
                }
            } else {
                System.out.println("Error: the specified directory either does not exist or is not a directory");
            }

            System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti il nome della cartella: ");
        }

        socket.close();
    }
}
