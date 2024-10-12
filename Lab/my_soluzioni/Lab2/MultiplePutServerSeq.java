// PutFileServer Sequenziale

import java.io.*;
import java.net.*;

public class MultiplePutServerSeq {
	private static final int DIM_BLOCCO = 400;
	public static final int PORT = 54321; // porta default per server

	public static void main(String[] args) throws IOException {
		// Porta sulla quale ascolta il server
		int port = -1;
		

		/* controllo argomenti */
		try {
			if (args.length == 1) {
				port = Integer.parseInt(args[0]);
				// controllo che la porta sia nel range consentito 1024-65535
				if (port < 1024 || port > 65535) {
					System.out.println("Usage: java MultiplePutServerSeq or java MultiplePutServerSeq port");
					System.exit(1);
				}
			} 
			else if (args.length == 0) {
				port = PORT;
			}
			else {
				System.out.println("Usage: java MultiplePutServerSeq or java MultiplePutServerSeq port");
				System.exit(1);
			}
		} //try
		catch (Exception e) {
			System.out.println("Problemi, i seguenti: ");
			e.printStackTrace();
			System.out.println("Usage: java MultiplePutServerSeq or java MultiplePutServerSeq port");
			System.exit(1);
		}

		/* preparazione socket e in/out stream */
		ServerSocket serverSocket = null;
		try {
//			serverSocket = new ServerSocket(port,2);
			serverSocket = new ServerSocket(port);
			serverSocket.setReuseAddress(true);		//chiedi perchè
			System.out.println("MultiplePutServerSeq: avviato ");
			System.out.println("Creata la server socket: " + serverSocket);
		}
		catch (Exception e) {
			System.err.println("Problemi nella creazione della server socket: "+ e.getMessage());
			e.printStackTrace();
			System.exit(2);
		}
		try {
			// oggetti utilizzati dal server per la comunicazione e la scrittura del file locale
			Socket clientSocket = null;
			DataInputStream inSock = null;
			DataOutputStream outSock = null;
			FileOutputStream outFile = null;
			
			File curFile = null;
			String nomeFile = null;
			String esito = null;
			
			Long dimFile = -1L;
			byte[] buffer = null;
				
			//ciclo infinito server
			while (true) {
				System.out.println("\nIn attesa di richieste...");
				try {
					clientSocket = serverSocket.accept();
					clientSocket.setSoTimeout(30000); //timeout altrimenti server sequenziale si sospende
					System.out.println("Connessione accettata: " + clientSocket + "\n");
				}
				catch (SocketTimeoutException te) {
					System.err.println("Non ho ricevuto nulla dal client per 30 sec., interrompo la comunicazione e accetto nuove richieste.");
					// il server continua a fornire il servizio ricominciando dall'inizio
					continue;
				}
				catch (Exception e) {
					System.err.println("Problemi nella accettazione della connessione: "+ e.getMessage());
					e.printStackTrace();
					// il server continua a fornire il servizio ricominciando dall'inizio
					// del ciclo, se ci sono stati problemi
					continue;
				}
				
				//stream I/O e ricezione nome file
				try {
					inSock = new DataInputStream(clientSocket.getInputStream());
					outSock = new DataOutputStream(clientSocket.getOutputStream());
					
		        }
				catch (IOException e) {
					System.out.println("Problemi nella creazione degli stream di input/output su socket: ");
					e.printStackTrace();
					// il server continua l'esecuzione riprendendo dall'inizio del ciclo
					continue;
		        }
				
				//elaborazione della richiesta di trasferimento di un direttorio
				try {
					while( true ) {
						try {
							nomeFile = inSock.readUTF(); 	//il ciclo termina solo quando arriva l'eccezione di EOF
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
				catch(SocketTimeoutException ste){
					System.out.println("Timeout scattato: ");
					ste.printStackTrace();
					clientSocket.close();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
					// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					continue;          
				}
			} // while (true)
			
//			clientSocket.shutdownInput();	//chiusura socket (downstream)
//			// ritorno esito positivo al client
//			outSock.writeUTF(esito + ", file salvato lato server");
//			clientSocket.shutdownOutput();	//chiusura socket (upstream)
//			System.out.println("\nTerminata connessione con " + clientSocket);
//			clientSocket.close();
		}
		// qui catturo le eccezioni non catturate all'interno del while
		// in seguito alle quali il server termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
			// chiusura di stream e socket
			System.out.println("Errore irreversibile, PutFileServerSeq: termino...");
			System.exit(3);
		}
	} // main
} // PutFileServerSeq
