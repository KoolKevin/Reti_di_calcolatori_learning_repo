// PutFileServer Concorrente

import java.io.*;
import java.net.*;

// Thread lanciato per ogni richiesta accettata
// versione per il trasferimento di file binari
class MultiplePutServerThread extends Thread{
	public static final int DIM_BLOCCO = 400;
	
	private Socket clientSocket = null;

	/**
	 * Constructor
	 * @param clientSocket
	 */
	public MultiplePutServerThread(Socket clientSocket) {
		this.clientSocket = clientSocket;
	}

	public void run() {
		DataInputStream inSock = null;
		DataOutputStream outSock = null;
		FileOutputStream outFile = null;
		
		File curFile = null;
		String nomeFile = null;
		String esito = null;
		
		Long dimFile = -1L;
		byte[] buffer = null;
		
		try {
			inSock = new DataInputStream(clientSocket.getInputStream());
			outSock = new DataOutputStream(clientSocket.getOutputStream());
					
			while( true ) {
				
				try {
					//il ciclo termina solo quando arriva l'eccezione di EOF
					nomeFile = inSock.readUTF();
					//controllo se il file è gia presente e comunico esito
					if (nomeFile == null) {
						System.out.println("Problemi nella ricezione del nome del file: ");
						clientSocket.close();
						continue;
					} else {
						curFile = new File(nomeFile);
						// controllo su file
						if(curFile.exists()) {
							esito = "salta";
						} 
						else {
							esito = "attiva";
						}
						
						//invio esito
						try{
							if( esito.equals("attiva") ) {
								outSock.writeUTF(esito);
							}
							else {
								outSock.writeUTF(esito);
								System.out.println("Il file "+nomeFile+" è gia presente, passo alla prossima richiesta");
								continue;
							}
						}
						catch(Exception e){
							System.out.println("Problemi nell'invio dell'esito");
							e.printStackTrace();
							continue;
						}
						
						outFile = new FileOutputStream(nomeFile);
					}
				}
				catch(EOFException e) {
					System.out.println("Fine trasferimento della directory da parte di "+ clientSocket +"\n");
					clientSocket.close();
					break;
				}
				
				//ricezione file
				try {
					System.out.println("Ricevo il file " + nomeFile + ": \n");
					
					//prima ricevo la dimensione
					dimFile = inSock.readLong();
					
					//TODO: leggo e scrivo a blocchi
					while(dimFile>0) {
				    	
				    	if(dimFile>400) {
				    		buffer=inSock.readNBytes(DIM_BLOCCO);
					    	outFile.write(buffer);
					    	outFile.flush();
					    	
				    		dimFile-=400;
				    	}
				    	else {
				    		buffer=inSock.readNBytes(dimFile.intValue());	//qua posso fare intValue perchè sono sicuro che non taglio niente
					    	outFile.write(buffer);
					    	outFile.flush();
					    	
				    		dimFile=0L;
				    	}
					}
				    
					System.out.println("\nRicezione del file " + nomeFile + " terminata\n");
					outFile.close();				// chiusura file 
				}    
				catch (Exception e) {
					System.err.println("\nProblemi durante la ricezione e scrittura del file: "+ e.getMessage());
					e.printStackTrace();
					clientSocket.close();
					continue;
				}
			} //while( readUTF )
		} 
		catch(IOException e) {
			System.err.println("\nProblemi durante la ricezione del nome file: "+ e.getMessage());
			e.printStackTrace();
		}
	} // run

} // PutFileServerThread class

public class MultiplePutServerCon {
	public static final int PORT = 54321; //default port

	public static void main(String[] args) throws IOException {

		int port = -1;

		/* controllo argomenti */
	    try {
	    	if (args.length == 1) {
	    		port = Integer.parseInt(args[0]);
	    		if (port < 1024 || port > 65535) {
	    			System.out.println("Usage: java MultiplePutServerCon [serverPort>1024]");
	    			System.exit(1);
	    		}
	    	} else if (args.length == 0) {
	    		port = PORT;
	    	} else {
	    		System.out.println("Usage: java MultiplePutServerCon or java MultiplePutServerCon port");
	    		System.exit(1);
	    	}
	    } //try
	    catch (Exception e) {
	    	System.out.println("Problemi, i seguenti: ");
	    	e.printStackTrace();
	    	System.out.println("Usage: java MultiplePutServerCon or java MultiplePutServerCon port");
	    	System.exit(1);
	    }

	    ServerSocket serverSocket = null;
	    Socket clientSocket = null;

	    try {
	    	serverSocket = new ServerSocket(port);
	    	serverSocket.setReuseAddress(true);
	    	System.out.println("MultiplePutServerCon: avviato ");
	    	System.out.println("Server: creata la server socket: " + serverSocket);
	    }
	    catch (Exception e) {
	    	System.err.println("Server: problemi nella creazione della server socket: "+ e.getMessage());
	    	e.printStackTrace();
	    	System.exit(1);
	    }

	    try {

	    	while (true) {
	    		System.out.println("Server: in attesa di richieste...\n");

	    		try {
	    			// bloccante fino ad una pervenuta connessione
	    			clientSocket = serverSocket.accept();
	    			clientSocket.setSoTimeout(30000);
	    			System.out.println("Server: connessione accettata: " + clientSocket);
	    		}
	    		catch (Exception e) {
	    			System.err.println("Server: problemi nella accettazione della connessione: "+ e.getMessage());
	    			e.printStackTrace();
	    			continue;
	    		}

	    		// serizio delegato ad un nuovo thread
	    		try {
	    			new MultiplePutServerThread(clientSocket).start();
	    		}
	    		catch (Exception e) {
	    			System.err.println("Server: problemi nel server thread: "+ e.getMessage());
	    			e.printStackTrace();
	    			continue;
	    		}

	    	} // while
	    }
	    // qui catturo le eccezioni non catturate all'interno del while
	    // in seguito alle quali il server termina l'esecuzione
	    catch (Exception e) {
	    	e.printStackTrace();
	    	// chiusura di stream e socket
	    	System.out.println("MultiplePutServerCon: termino...");
	    	System.exit(2);
	    }
	    
	}
} // PutFileServerCon class
