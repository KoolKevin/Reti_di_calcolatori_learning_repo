import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

class ServerThread extends Thread {
    
    private List<Noleggio> noleggi = null;
    private Socket clientSocket = null;
    
    private int buffer_size = 4096;

    public ServerThread(Socket clientSocket, List<Noleggio> noleggi) {
        this.clientSocket = clientSocket;
        this.noleggi = noleggi;
    }

    public void run() {
        System.out.println("Attivazione figlio: " + Thread.currentThread().getName());

        // Variables for reading and transferring files
        int cont = 0;
        int read_bytes = 0;
        long daTrasferire;
        byte[] buffer = new byte[buffer_size];

        String operazione;

        String modello_richiesto;
        List<Noleggio> lista_sci_richiesta = null;
        File fileCorr;
        boolean immaginePersa;

        String idDaValutare;
        float risultatoValutazione;

        DataInputStream inSock;
        ObjectOutputStream outSock;
        DataOutputStream dataOutSock;
        FileInputStream src_stream;   

        //recupero gli stream dalla socket
        try {
            inSock = new DataInputStream(clientSocket.getInputStream());
            outSock = new ObjectOutputStream(clientSocket.getOutputStream());
            dataOutSock = new DataOutputStream(clientSocket.getOutputStream());
        } catch (IOException ioe) {
            System.out.println("Problemi nella creazione degli stream di input/output su socket: ");
            ioe.printStackTrace();
            return;
        }

        //ciclo di servizio
        try {
            try {
                while ((operazione = inSock.readUTF()) != null) {
                    //recupero sci di un certo modello
                    if(operazione.equals("recupera")) {
                        System.out.println("Ricevuta richiesta di recupero sci di un certo modello!");
                        modello_richiesto = inSock.readUTF();
                        System.out.println("\tmodello richiesto: "+modello_richiesto);

                        lista_sci_richiesta = new ArrayList<Noleggio>();
                        immaginePersa = false;
                        
                        //costruisco la lista e la invio
                        for(Noleggio n : this.noleggi) {
                            if( n.getModello().equals(modello_richiesto) ) {
                                lista_sci_richiesta.add(n);
                            }
                        }
                        outSock.writeObject( lista_sci_richiesta );
                        
                        //invio le immagini 
                        for(Noleggio n : lista_sci_richiesta) {
                            fileCorr = new File(n.getNome_immagine());

                            if( fileCorr.exists() && fileCorr.isFile() ) {
                                src_stream = new FileInputStream(fileCorr.getAbsolutePath());
                                //invio la lunghezza e nome
                                daTrasferire = fileCorr.length();
                                System.out.println("Invio al cliente il file: " + fileCorr.getName() + " di " + daTrasferire +" bytes" );
                                dataOutSock.writeLong(daTrasferire);
                                dataOutSock.writeUTF(fileCorr.getName());
                                //invio contenuto
                                try {
                                    cont = 0;
                                    while (cont < daTrasferire) {
                                        read_bytes = src_stream.read(buffer);
                                        dataOutSock.write(buffer, 0, read_bytes);
                                        cont += read_bytes;

                                        System.out.println("\tByte trasferiti: " + cont);
                                    }
                                    
                                    src_stream.close();
                                }
                                // l'eccezione dovrebbe scattare solo se ci aspettiamo un numero sbagliato di byte da leggere
                                catch (EOFException e) {
                                    System.out.println("Problemi, i seguenti: ");
                                    e.printStackTrace();
                                }
                            }
                            else{
                                System.out.println("\terrore: immagine: " + fileCorr.getName() + " non trovata");
                                immaginePersa = true;
                                //passo alla prossima foto
                            }
                        }   // for invio immagini
                        

                        /*
                         * daTrasferire = -1 -> ho trasferito tutte le immagini in modo corretto
                         * daTrasferire = -2 -> non ho trasferito tutte le immagini per qualche problema
                         */
                        if(immaginePersa)
                            daTrasferire = -2; 
                        else
                            daTrasferire = -1;


                        System.out.println("ho finito il trasferimento; invio una ultima lunghezza negativa per segnalare la fine al client");
                        dataOutSock.writeLong(daTrasferire);
                    }
                    //valutazione costo di un noleggio
                    else if(operazione.equals("valuta")) {
                        System.out.println("Ricevuta richiesta di valutazione di un costo di un noleggio!");
                        risultatoValutazione = -1F;
                        idDaValutare = inSock.readUTF();
                        System.out.println("\tnoleggio da valutare: "+idDaValutare);

                        for(Noleggio n : this.noleggi) {
                            if( n.getId().equals(idDaValutare) && n.getGiorni_noleggio() != -1) {
                                risultatoValutazione = n.getCosto_giornaliero() * n.getGiorni_noleggio();
                                break;
                            }
                        }
                        System.out.println("\tcosto calcolato: "+risultatoValutazione);

                        dataOutSock.writeFloat(risultatoValutazione);
                    }
                    else{
                        System.out.println("ho ricevuto una operazione invalida: " + operazione + ". NON FACCIO NIENTE");
                    }

                } // while
            } catch (EOFException eof) {    //NB: l'eof viene trattata come una eccezione purtroppo
                System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
                //chiudo la socket solo quando ricevo l'eof dal cliente
                clientSocket.close();
                System.out.println("ServerFiglio: termino...");
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
    private static List<Noleggio> noleggi = new ArrayList<Noleggio>();

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
        Calendar calendar = Calendar.getInstance();

        Noleggio n = new Noleggio();
        n.setId("X12AB");
        calendar.set(2012, 11, 12);
        n.setData(calendar.getTime());
        n.setGiorni_noleggio(14);
        n.setModello("Volki Shark");
        n.setCosto_giornaliero(7F);
        n.setNome_immagine("VolkiShark1.jpg");
        noleggi.add(n);

        n = new Noleggio();
        n.setId("ACD14");
        n.setGiorni_noleggio(-1);
        n.setModello("Fisher Snow");
        n.setCosto_giornaliero(5F);
        n.setNome_immagine("FisherSnow.jpg");
        noleggi.add(n);

        n = new Noleggio();
        n.setId("Y23CC");
        calendar.set(2012, 11, 23);
        n.setData(calendar.getTime());
        n.setGiorni_noleggio(7);
        n.setModello("Volki Shark");
        n.setCosto_giornaliero(7F);
        n.setNome_immagine("VolkiShark2.jpg");
        noleggi.add(n);


        System.out.println("NOLEGGI NELLA STRUTTURA DATI:");
        for(Noleggio noleggio : noleggi) {
            System.out.println(noleggio);
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
                    new ServerThread(clientSocket, noleggi).start();
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
