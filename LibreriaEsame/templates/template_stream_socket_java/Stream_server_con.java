import java.io.*;
import java.net.*;

class PutFileServerThread extends Thread {
    private Socket clientSocket = null;
    
    //private int buffer_size = 4096;

    public PutFileServerThread(Socket clientSocket) {
        this.clientSocket = clientSocket;
    }

    public void run() {
        System.out.println("Attivazione figlio: " + Thread.currentThread().getName());

        // Variables for reading and transferring files
        
        int buf;
        int cont = 0;
        //byte[] buffer = new byte[buffer_size];
        //int read_bytes = 0;

        DataInputStream inSock;
        DataOutputStream outSock;
        DataOutputStream dest_stream = null;

        //recupero gli stream dalla socket
        try {
            inSock = new DataInputStream(clientSocket.getInputStream());
            outSock = new DataOutputStream(clientSocket.getOutputStream());
        } catch (IOException ioe) {
            System.out.println("Problemi nella creazione degli stream di input/output su socket: ");
            ioe.printStackTrace();
            return;
        }

        //ciclo di servizio
        try {
            try {
                /* VARIABILI DA CAMBIARE IN BASE ALL'ESERCIZIO */
                String nomeFileRicevuto;
                long numeroByte;
                File fileCorr;
                FileOutputStream outFileCorr;
                /* ------------------------------------------- */

                while ((nomeFileRicevuto = inSock.readUTF()) != null) {
                    fileCorr = new File(nomeFileRicevuto);

                    //scelgo che esito mandare al client
                    if (fileCorr.exists()) {
                        outSock.writeUTF("salta file");
                    } else {
                        outSock.writeUTF("attiva");
                        
                        //leggo prima la dimensione del file che mi aspetto di ricevere
                        numeroByte = inSock.readLong();
                        System.out.println("Scrivo il file " + nomeFileRicevuto + " di " + numeroByte + " byte");
                        //creo il file su cui scrivere
                        outFileCorr = new FileOutputStream(nomeFileRicevuto);
                        // Ricevo il file 
                        dest_stream = new DataOutputStream(outFileCorr);
                        cont = 0;
                        try {
                            // esco dal ciclo quando ho letto il numero di byte da trasferire
                            while (cont < numeroByte) {
                                /* trasferimento a blocchi */
                                //read_bytes = inSock.read(buffer);     //se leggo a blocchi dalla socket ho il problema di eccedere nell'altro file
                                //dest_stream.write(buffer, 0, read_bytes);
                                //cont += read_bytes;

                                /* trasferimento di un byte alla volta */
                                buf = inSock.read();
                                dest_stream.write(buf);
                                cont++;
                            }

                            dest_stream.flush();
                            System.out.println("Byte trasferiti: " + cont);
                        }
                        // l'eccezione dovrebbe scattare solo se ci aspettiamo un numero sbagliato di byte da leggere
                        catch (EOFException e) {
                            System.out.println("Problemi, i seguenti: ");
                            e.printStackTrace();
                        }

                        outFileCorr.close();
                    }
                } // while
            } catch (EOFException eof) {    //IMPORTATE: l'eof viene trattata come una eccezione purtroppo
                System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
                //chiudo la socket solo quando ricevo l'eof dal cliente
                clientSocket.close();
                System.out.println("PutFileServer: termino...");
                System.exit(0);
            } catch (SocketTimeoutException ste) {
                System.out.println("Timeout scattato: ");
                ste.printStackTrace();
                clientSocket.close();
                System.exit(1);
            } catch (Exception e) {
                System.out.println("Problemi, i seguenti : ");
                e.printStackTrace();
                System.out.println("Chiudo ed esco...");
                clientSocket.close();
                System.exit(2);
            }
        } catch (IOException ioe) {
            System.out.println("Problemi nella chiusura della socket: ");
            ioe.printStackTrace();
            System.out.println("Chiudo ed esco...");
            System.exit(3);
        }
    }

}// thread

public class Stream_server_con {
    private static final int PORT = 4445;

    public static void main(String[] args) throws IOException {
        int port = -1;

        //controllo argomenti
		if ((args.length == 0)) {
			port = PORT;
		} 
		else if (args.length == 1) {
			try {
				port = Integer.parseInt(args[0]);
				// controllo che la porta sia nel range consentito 1024-65535
				if (port < 1024 || port > 65535) {
					System.out.println("Usage: java Stream_Server_com [serverPort>1024]");
					System.exit(1);
				}
			} catch (NumberFormatException e) {
				System.out.println("Usage: java Stream_Server_com [serverPort>1024]");
				System.exit(1);
			}
		} 
		else {
			System.out.println("Usage: java Stream_Server_com [serverPort>1024]");
			System.exit(1);
		}

        //Creazione serverSocket
        ServerSocket serverSocket = null;
        Socket clientSocket = null;

        try {
            serverSocket = new ServerSocket(port);
            serverSocket.setReuseAddress(true);
            System.out.println("PutFileServerCon: avviato ");
            System.out.println("Server: creata la server socket: " + serverSocket);
        } catch (Exception e) {
            System.err.println("Server: problemi nella creazione della server socket: " + e.getMessage());
            e.printStackTrace();
            serverSocket.close();
            System.exit(1);
        }

        //ciclo di accettazione richieste e deleghe ai figli
        try {
            while (true) {
                System.out.println("Server: in attesa di richieste...\n");

                //accettazione
                try {
                    clientSocket = serverSocket.accept(); 
                    System.out.println("Server: connessione accettata: " + clientSocket);
                } catch (Exception e) {
                    System.err.println("Server: problemi nella accettazione della connessione: " + e.getMessage());
                    e.printStackTrace();
                    continue;
                }

                //delega
                try {
                    new PutFileServerThread(clientSocket).start();
                } catch (Exception e) {
                    System.err.println("Server: problemi nel server thread: " + e.getMessage());
                    e.printStackTrace();
                    continue;
                }
            } 
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Server: termino...");
            System.exit(2);
        }
    }
}
