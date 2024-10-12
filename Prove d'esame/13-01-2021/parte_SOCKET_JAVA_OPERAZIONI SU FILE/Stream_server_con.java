import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;

class ServerThread extends Thread {
    private Socket clientSocket = null;
    
    private int buffer_size = 4096;

    public ServerThread(Socket clientSocket) {
        this.clientSocket = clientSocket;
    }

    private boolean isTxt(String filename) {
        //il file deve essere lungo almeno 5 caratteri: x.txt
        if(filename.length() < 5) {
            return false;
        }

        String estensione = filename.substring(filename.length() - 4) ;
        //System.out.println("\tDEBUG\t filename: " + filename +"; estensione: " + estensione);

        if(estensione.equals(".txt")) {
            return true;
        }
        else {
            return false;
        }
    }

    public void run() {
        System.out.println("Attivazione figlio: " + Thread.currentThread().getName());

        // Variables for reading and transferring files
        int cont = 0;
        int read_bytes = 0;
        long daTrasferire;
        byte[] buffer = new byte[buffer_size];

        String operazione;

        String nomeDirectory;
        File dirCorr;
        File[] files;
        File fileCorr;

        char carattere;
        int soglia_occorrenze_carattere;
        int conta_occorrenze_carattere;
        int contatore_linee;
        String linea;

        DataInputStream inSock;
        DataOutputStream outSock;
        FileInputStream src_stream; 
        BufferedReader reader;  

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
                while ((operazione = inSock.readUTF()) != null) {
                    if(operazione.equals("get")) {
                        //multiple get dei file binari di un direttorio

                        //leggo il nome del direttorio
                        nomeDirectory = inSock.readUTF();
                        dirCorr = new File(nomeDirectory);

                        System.out.println("Ricevuta richiesta di recupero dei file binari del direttorio" + nomeDirectory);

                        //controllo se il direttorio è valido
                        if (dirCorr.exists() && dirCorr.isDirectory()) {
                            System.out.println("\t"+nomeDirectory+"è un direttorio valido");

                            files = dirCorr.listFiles();     

                            //scorro tutti i file della directory
                            for (int i = 0; i < files.length; i++) {
                                fileCorr = files[i];

                                if (fileCorr.isFile() ) {
                                    //controllo se è un file binario o meno
                                    if( isTxt(fileCorr.getName()) ) {
                                        System.out.println("\tsalto "+nomeDirectory+"/"+fileCorr.getName());
                                        continue;
                                    }

                                    src_stream = new FileInputStream(fileCorr.getAbsolutePath());
                                    //invio la lunghezza e nome
                                    daTrasferire = fileCorr.length();
                                    System.out.println("\tInvio al cliente il file: " + fileCorr.getName() + " di " + daTrasferire +" bytes" );
                                    outSock.writeLong(daTrasferire);
                                    outSock.writeUTF(fileCorr.getName());
                                    //invio contenuto
                                    try {
                                        cont = 0;
                                        while (cont < daTrasferire) {
                                            read_bytes = src_stream.read(buffer);
                                            outSock.write(buffer, 0, read_bytes);
                                            cont += read_bytes;
                                        }
                                        

                                        System.out.println("\tByte trasferiti: " + cont);
                                        src_stream.close();
                                    }
                                    // l'eccezione dovrebbe scattare solo se ci aspettiamo un numero sbagliato di byte da leggere
                                    catch (EOFException e) {
                                        System.out.println("Problemi, i seguenti: ");
                                        e.printStackTrace();
                                    }
                                }
                            }

                            daTrasferire = -1; 
                            System.out.println("ho finito il trasferimento; invio una ultima lunghezza negativa per segnalare la fine al client");
                            outSock.writeLong(daTrasferire);
                        }
                        else{
                            daTrasferire = -2; 
                            System.out.println("il nome di direttorio passato non è valido, segnalo l'errore al cliente");
                            outSock.writeLong(daTrasferire);
                        }
                    }
                    else if(operazione.equals("conta")) {
                        contatore_linee = 0;

                        carattere = inSock.readChar();
                        soglia_occorrenze_carattere = inSock.readInt();

                        System.out.println("Ricevuta richiesta di conteggio delle linee che iniziano con una maiuscola e con almeno"
                                            +soglia_occorrenze_carattere+ " occorrenze di "+carattere+" all'interno di tutti i file di testo del direttorio del server");

                        //handle alla directory root del server
                        dirCorr = new File(".");

                        files = dirCorr.listFiles();     
                        //scorro tutti i file della directory
                        for (int i = 0; i < files.length; i++) {
                            fileCorr = files[i];

                            if (fileCorr.isFile() ) {
                                //controllo se è un file di testo o meno
                                if( !isTxt(fileCorr.getName()) ) {
                                    System.out.println("\tsalto " + fileCorr.getName());
                                    continue;
                                }


                                //analizzo il file
                                reader = new BufferedReader( new FileReader(fileCorr.getAbsolutePath()) );

                                while( (linea=reader.readLine()) != null ) {
                                    //controllo se la prima lettera è maiuscola
                                    if( Character.isUpperCase(linea.charAt(0)) ) {
                                        //controllo tutti i caratteri della linea
                                        conta_occorrenze_carattere = 0;

                                        for(int j=0; j<linea.length(); j++) {
                                            if( linea.charAt(j) == carattere ) {
                                                conta_occorrenze_carattere++;
                                            }
                                        }

                                        if(conta_occorrenze_carattere >= soglia_occorrenze_carattere) {
                                            contatore_linee++;
                                        }
                                    }
                                }
                            }
                        }

                        outSock.writeInt(contatore_linee);
                    }
                    else{
                        System.out.println("ho ricevuto una operazione invalida: " + operazione + ". NON FACCIO NIENTE");
                    }

                } // while
            } catch (EOFException eof) {    //NB: l'eof viene trattata come una eccezione purtroppo
                System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
                //chiudo la socket solo quando ricevo l'eof dal cliente
                clientSocket.close();
                System.out.println("GetFileServer: termino...");
                //System.exit(0);
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
            System.out.println("ServerCon: avviato ");
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
                    new ServerThread(clientSocket).start();
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
