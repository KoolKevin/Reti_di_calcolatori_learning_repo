// PutFileClient.java

import java.net.*;
import java.io.*;

public class MultiplePutClient {

	public static void main(String[] args) throws IOException {
	
		InetAddress addr = null;
		int port = -1;
		
		try{ //check args [ip, porta]
			if(args.length == 2){
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
			} else{
				System.out.println("Usage: java MultiplePutClient serverAddr serverPort");
				System.exit(1);
			}
			
			//controllo l'intervallo della porta
			if( port<1024 || port>65535) {
				System.out.println("scrivi una porta valida");
				System.exit(1);
			}
		} //try
		// Per esercizio si possono dividere le diverse eccezioni
		catch(Exception e){
			System.out.println("Problemi, i seguenti: ");
			e.printStackTrace();
			System.out.println("Usage: java MultiplePutClient serverAddr serverPort");
			System.exit(2);
		}
		
		// oggetti utilizzati dal client per la comunicazione e la lettura del file locale
		Socket socket = null;
		FileInputStream inFile = null;
		DataInputStream inSock = null;
		DataOutputStream outSock = null;
		
		Long soglia = 0L;
		Long dimensioneDirectory = 0L;
		String nomeDirectory = null;
		String[] contenuto = null;
		String esito = null;
		
		File directory = null;
		File file = null;
		
		// creazione stream di input da tastiera e interazione con l'utente
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		System.out.print("Immetti soglia dimensione file minima: ");
		try {
			soglia=Long.parseLong(stdIn.readLine());
		}
		catch(NumberFormatException e) {
			System.out.println("Immetti una soglia valida");
			System.exit(2);
		}
		
		
		// creazione socket e connessione
		try{
			socket = new Socket(addr, port);
			socket.setSoTimeout(30000);
			System.out.println("Creata la socket: " + socket);
		}
		catch(Exception e){
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
		}
		
		// creazione stream di input/output su socket
		try{
			inSock = new DataInputStream(socket.getInputStream());
			outSock = new DataOutputStream(socket.getOutputStream());
		}
		catch(IOException e){
			System.out.println("Problemi nella creazione degli stream su socket: ");
			e.printStackTrace();
		}
		
		System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure path del direttorio da trasferire: ");
		try{
			while ( (nomeDirectory=stdIn.readLine()) != null ) {
				directory = new File(nomeDirectory);
				
				// se la directory esiste ed è valida creo la socket
				if( directory.exists() && directory.isDirectory() ) {
					System.out.println("Directory esistente e valida!");				
					
					System.out.println("Contenuto di " + nomeDirectory + ": ");
					contenuto=directory.list();
					
					for(String nomeFile : contenuto) {
						System.out.println(nomeFile);
						
						/* Invio nomefile e attesa esito dal server */
						try{
							//invio il nome del file solo se la sua dimensione supera la soglia
							if( new File(nomeDirectory+"/"+nomeFile).length() > soglia ) {
								outSock.writeUTF(nomeFile);
								System.out.println("Inviato il nome del file " + nomeFile);
							}
							else {
								System.out.println(nomeFile + " non inviato perchè troppo piccolo");
								continue;
							}
							
						}
						catch(Exception e){
							System.out.println("Problemi nell'invio del nome di " + nomeFile+ ": ");
							e.printStackTrace();
							System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure path del direttorio da trasferire: ");
							continue;
						}
						
						// ricezione esito
						try{
							esito = inSock.readUTF();
							System.out.println("Esito: " + esito);
							// chiudo la socket in downstream
						}
						catch(SocketTimeoutException ste){
							System.out.println("Timeout scattato: ");
							ste.printStackTrace();
							socket.close();
							System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure path del direttorio da trasferire: ");
							continue;          
						}
						catch(Exception e){
							System.out.println("Problemi nella ricezione dell'esito, i seguenti: ");
							e.printStackTrace();
							socket.close();
							System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure path del direttorio da trasferire: ");
							continue;
						}
						
						if( esito.equals("attiva") ) {
							//trasferisco il file
							System.out.print("trasferisco il file: " + nomeFile );
							
							// creazione stream di input da file
							try{
								inFile = new FileInputStream(nomeDirectory+"/"+nomeFile);
							}
							/*
							 * abbiamo gia' verificato che esiste, a meno di inconvenienti, es.
							 * cancellazione concorrente del file da parte di un altro processo, non
							 * dovremmo mai incorrere in questa eccezione.
							 */
							catch(FileNotFoundException e) {
								System.out.println("Problemi nella creazione dello stream di input da "+ nomeFile + ": ");
								e.printStackTrace();
								System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure path del direttorio da trasferire: ");
								continue;
							}
							
							System.out.println("Inizio la trasmissione di " + nomeFile);
							
							// trasferimento file
							try{
								//prima mando la dimensione
								outSock.writeLong( new File(nomeDirectory+"/"+nomeFile).length() );
								//poi mando il contenuto byte per byte
								//TODO: trasferisci a blocchi
								FileUtility.trasferisci_a_byte_file_binario(new DataInputStream(inFile), outSock);
								inFile.close(); 			// chiusura file
								System.out.println("Trasmissione di " + nomeFile + " terminata ");
							}
							catch(SocketTimeoutException ste){
								System.out.println("Timeout scattato: ");
								ste.printStackTrace();
								socket.close();
								System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
								// il client continua l'esecuzione riprendendo dall'inizio del ciclo
								continue;          
							}
							catch(Exception e){
								System.out.println("Problemi nell'invio di " + nomeFile + ": ");
								e.printStackTrace();
								socket.close();
								System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
								// il client continua l'esecuzione riprendendo dall'inizio del ciclo
								continue;
							}
							
						}
						else if( esito.equals("salta") ) {
							//provo a trasferire il prossimo file
							System.out.println(nomeFile +" non verrà trasferito perchè gia presente sul server");
							continue;
						}
						else {
							System.out.print("esito inaspettato, provo a trasferire il prossimo file: ");
							continue;
						}
				
					}	//for
				}	//if
				else {
					System.out.println("Directory non valida!");
					continue;
				}
				
				System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure path del direttorio da trasferire: ");
			}   //fine while
			
			//finito di trasferire, non c'è bisogno di chiusura dolce
			socket.close();
			System.out.println("MultiplePutClient: termino...");
		}
		// qui catturo le eccezioni non catturate all'interno del while
		// quali per esempio la caduta della connessione con il server
		// in seguito alle quali il client termina l'esecuzione
		catch(Exception e){
			System.err.println("Errore irreversibile, il seguente: ");
			e.printStackTrace();
			System.err.println("Chiudo!");
			System.exit(3); 
	    }
	} // main
} // MultiplePutClient
