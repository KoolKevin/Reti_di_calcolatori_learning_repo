import java.io.*;
import java.nio.file.*;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.List;
import java.util.StringTokenizer;

import java.util.ArrayList;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {
	private BufferedReader in = null; 
	private PrintWriter fout = null;
	
	private StringTokenizer tokenizer;
	
	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
	}
	
	public List<String> lista_nomi_file_contenenti_parola_in_linea(String nomeDirettorio, String parolaCercata) throws RemoteException {
		List<String> result = new ArrayList<String>();
		String riga, parola;
		boolean trovato = false;

		//ottengo handle alla directory
		File dir = new File(nomeDirettorio);
		if( !dir.exists() || !dir.isDirectory() )
			throw new RemoteException("directory invalida");
		
		for( File file : dir.listFiles() ) {
			//controllo solo i file; devo controllare che finisca per .txt?
			if( file.isFile() ) {
				System.out.println("file: " + file);
				try {
					in = new BufferedReader( new FileReader(file) );
				}
				catch( FileNotFoundException e ) {
					throw new RemoteException();
				}

				//leggo riga per riga
				try {
					while ((riga = in.readLine()) != null && !trovato) { 
						//devo fare il parsing della riga a mano?
						tokenizer = new StringTokenizer(riga, " \n");
						
						System.out.println("riga: " + riga);
						
						//leggo tutte le parole della riga
						while( tokenizer.hasMoreTokens() && !trovato ) {
							parola = tokenizer.nextToken();
							
							System.out.println("parola: " + parola);
							
							if( parola.equals(parolaCercata) ) {
								trovato = true;
								System.out.println("parola trovata aggiungo il file: " + file.getName());
							}
						}
	
						if(trovato) {
							result.add(file.getName());
						}
					}
					
					in.close();
				}
				catch( IOException e ) {
					throw new RemoteException();
				}
				
				trovato = false;
			}
		} 
		
		return result;
	}

	public int conta_numero_linee(String nomeFile, String parolaCercata) throws RemoteException {
		int result=0;
		String riga, parola;
		boolean trovato = false;
		
		//apro il file
		System.out.println("file: " + nomeFile);
		try {
			in = new BufferedReader( new FileReader(nomeFile) );
		}
		catch( FileNotFoundException e ) {
			return -1;
		}
		
		//leggo riga per riga
		try {
			while ((riga = in.readLine()) != null) { 
				//devo fare il parsing della riga a mano?
				tokenizer = new StringTokenizer(riga, " \n");
				System.out.println("riga: " + riga);
				
				//leggo tutte le parole della riga fino a che non trovo la parola cercata
				while( tokenizer.hasMoreTokens() && !trovato ) {
					parola = tokenizer.nextToken();
					
					System.out.println("parola: " + parola);
					
					if( parola.equals(parolaCercata) ) {
						trovato = true;
						result++;
					}
				}
				
				trovato = false;
			}
			
			in.close();
		}
		catch( IOException e ) {
			throw new RemoteException();
		}
		
		if(result == 0)
			return -1;
		
		return result;
	}
	

	// Avvio del Server RMI
	public static void main(String[] args) {
		final int REGISTRYPORT = 12345;
		String registryHost = "localhost";
		String serviceName = "fileService"; // lookup name...

		// Registrazione del servizio RMI
		String completeName = "//" + registryHost + ":" + REGISTRYPORT + "/" + serviceName;
		try {
			RMI_Server serverRMI = new RMI_Server();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");
		} catch (Exception e) {
			System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
