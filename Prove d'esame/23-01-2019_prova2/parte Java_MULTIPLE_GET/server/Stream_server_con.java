import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;

class ServerThread extends Thread {
    
    private List<Prenotazione> prenotazioni = null;
    private Socket clientSocket = null;
    
    private int buffer_size = 4096;

    public ServerThread(Socket clientSocket, List<Prenotazione> prenotazioni) {
        this.clientSocket = clientSocket;
        this.prenotazioni = prenotazioni;
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

        String targaDaEliminare;
        int esitoEliminazione;

        DataInputStream inSock;
        DataOutputStream outSock;
        FileInputStream src_stream;   

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
                        nomeDirectory = inSock.readUTF();
                        dirCorr = new File(nomeDirectory);

                        if (dirCorr.exists() && dirCorr.isDirectory()) {
                            files = dirCorr.listFiles();     

                            //scorro tutti i file della directory
                            for (int i = 0; i < files.length; i++) {
                                fileCorr = files[i];

                                if (fileCorr.isFile() ) {
                                    src_stream = new FileInputStream(fileCorr.getAbsolutePath());
                                    //invio la lunghezza e nome
                                    daTrasferire = fileCorr.length();
                                    System.out.println("Invio al cliente il file: " + fileCorr.getName() + " di " + daTrasferire +" bytes" );
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
                                        

                                        System.out.println("Byte trasferiti: " + cont);
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
                    else if(operazione.equals("delete")) {
                        esitoEliminazione = -1;
                        targaDaEliminare = inSock.readUTF();
                        
                        //elimino la prenotazione dalla struttura dati
                        for(Prenotazione pre : prenotazioni) {
                            if(pre.getTarga().equals(targaDaEliminare)) {
                                prenotazioni.remove(pre);
                                esitoEliminazione = 0;
                                break;
                            }
                        }

                        System.out.println("PRENOTAZIONI NELLA STRUTTURA DATI DOPO L'ELIMINAZIONE:");
                        for(Prenotazione pre : prenotazioni) {
                            System.out.println(pre.getTarga() + "\t" + pre.getPatente() + "\t" + pre.getTipo() + "\t" + pre.getDirectoryImg());
                        }

                        //cancello la cartella delle immagini
                        dirCorr = new File("toGet");
                        
                        //devo prima eliminare il contenuto
                        if (dirCorr.exists() && dirCorr.isDirectory()) {
                            files = dirCorr.listFiles();

                            for(File file : files) {
                                file.delete();
                            }
                        }

                        dirCorr.delete();

                        outSock.writeInt(esitoEliminazione);
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
    private static List<Prenotazione> prenotazioni = new ArrayList<Prenotazione>();

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

        //inizializzazione struttura dati
        Prenotazione p = new Prenotazione();
        p.setTarga("AN745NL");
        p.setPatente("00000");
        p.setTipo("auto");
        p.setDirectoryImg("toGet");
        prenotazioni.add(p);
        p = new Prenotazione();
        p.setTarga("GD111SD");
        p.setPatente("00001");
        p.setTipo("camper");
        p.setDirectoryImg("toGet");
        prenotazioni.add(p);

        System.out.println("PRENOTAZIONI NELLA STRUTTURA DATI:");
        for(Prenotazione pre : prenotazioni) {
            System.out.println(pre.getTarga() + "\t" + pre.getPatente() + "\t" + pre.getTipo() + "\t" + pre.getDirectoryImg());
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
                    new ServerThread(clientSocket, prenotazioni).start();
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
