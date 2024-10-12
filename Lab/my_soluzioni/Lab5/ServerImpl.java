import java.io.*;
import java.nio.file.*;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.StringTokenizer;

public class ServerImpl extends UnicastRemoteObject implements RemOp {
	private BufferedReader in = null; 
	private PrintWriter fout = null;
	private String riga;
	private StringTokenizer tokenizer;
	
	// Costruttore
	public ServerImpl() throws RemoteException {
		super();
	}
	
	//implementazioni dei metodi
	public int conta_righe(String nomeFile, int soglia) throws RemoteException {
		int result=0;
		int contaRiga=0;
		
		//apro il file
		try {
			in = new BufferedReader(new FileReader(nomeFile));
		} catch(FileNotFoundException e){
			throw new RemoteException();
		}
		
		System.out.println("conto le righe del file " + nomeFile +" con più di " + soglia + " parole");

		try {
			//leggo il file e conto le parole
			while ((riga = in.readLine()) != null) { 
				System.out.print(riga);
				//TODO: fai il parsing della riga a mano, in modo da non dovermi affidare ad uno StringTokenizer che occupa memoria heap
				tokenizer = new StringTokenizer(riga, " \n");
				contaRiga=tokenizer.countTokens();
				System.out.println(": "+contaRiga+" parole");
				
				if( contaRiga > soglia ) {
					result++;
				}
			}

			in.close();
		} catch(IOException ex){
			System.out.println("Errore nella lettura del file " + nomeFile);
			throw new RemoteException();
		}
		
		return result;
	}

	public synchronized Esito elimina_riga(String nomeFile, int indiceRiga) throws RemoteException {
		String riga;
		int contaRiga=1;
		
		//apro il file
		try {
			in = new BufferedReader(new FileReader(nomeFile));
		} catch(FileNotFoundException e){
			System.out.println("File non trovato");
			System.exit(1);
		}
		
		try {
			//creo un file temporaneo su cui copiare
			fout = new PrintWriter( new FileWriter(nomeFile+"TEMP"));
			
			//copio su file temporaneo tranne la riga da eliminare
			while ((riga = in.readLine()) != null) { 
				if(contaRiga!=indiceRiga) {
					fout.println(riga);
				}
				
				contaRiga++;
			}
			
			fout.close();
			in.close();

			//controllo se sono arrivato alla riga da eliminare
			if(contaRiga <= indiceRiga) {
				System.out.println("il file "+nomeFile+" non ha abbastanza righe");
				throw new RemoteException();
			}
 			
			//sostituisco il file temporaneo con quello originale
			File fileOriginale = new File(nomeFile);
			File fileTemp = new File(nomeFile+"TEMP");
			fileTemp.renameTo(fileOriginale);
		} catch(Exception e){
			System.out.println("Errore nell'eliminare la riga");
			throw new RemoteException();
		} 
		
		Esito e = new Esito();
		e.setNomeFile(nomeFile);
		e.setRigheFile(contaRiga-2);	// -2 perchè contaRiga conta sempre un riga in più
		
		return e;
	}

	// Avvio del Server RMI
	public static void main(String[] args) {
		//TODO: fai anche la versione con la porta del registry come argomento
		final int REGISTRYPORT = 12345;
		String registryHost = "localhost";
		String serviceName = "fileService"; // lookup name...

		// Registrazione del servizio RMI
		String completeName = "//" + registryHost + ":" + REGISTRYPORT + "/" + serviceName;
		try {
			ServerImpl serverRMI = new ServerImpl();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");
		} catch (Exception e) {
			System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
