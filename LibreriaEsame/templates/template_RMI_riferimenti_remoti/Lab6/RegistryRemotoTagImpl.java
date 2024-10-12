import java.rmi.Naming;
import java.rmi.RMISecurityManager;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RegistryRemotoTagImpl extends UnicastRemoteObject implements RegistryRemotoTagServer {

	// num. entry [nomelogico][ref]
	final int tableSize = 100;
	// tag disponibili con cui un server può associarsi
	final String[] tagDisponibili = {"file", "congresso", "bello"};

	// Tabella: la prima colonna contiene i nomi, la seconda i riferimenti remoti
	Object[][] table = new Object[tableSize][2];
	
	// Tabella: la prima colonna sono i nomi logici dei servizi, la secondo il loro tag associato
	String[][] tagTable = new String[tableSize][2];

	public RegistryRemotoTagImpl() throws RemoteException {
		super();
		int i;
		for (i = 0; i < tableSize; i++) {
			table[i][0] = null;
			table[i][1] = null;
		}
		
		for (i = 0; i < tableSize; i++) {
			tagTable[i][0] = null;
			tagTable[i][1] = null;
		}
	}

	/** Aggiunge la coppia nella prima posizione disponibile */
	public synchronized boolean aggiungi(String nomeLogico, Remote riferimento)
			throws RemoteException {
		// Cerco la prima posizione libera e la riempio
		boolean risultato = false;
		if ((nomeLogico == null) || (riferimento == null))
			return risultato;
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] == null) {
				table[i][0] = nomeLogico;
				table[i][1] = riferimento;
				risultato = true;
				break;
			}
		return risultato;
	}

	/** Restituisce il riferimento remoto cercato, oppure null */
	public synchronized Remote cerca(String nomeLogico) throws RemoteException {
		Remote risultato = null;
		if (nomeLogico == null)
			return null;
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] != null && nomeLogico.equals((String) table[i][0])) {
				risultato = (Remote) table[i][1];
				break;
			}
		return risultato;
	}

	/** Restituisce tutti i riferimenti corrispondenti ad un nome logico */
	public synchronized Remote[] cercaTutti(String nomeLogico)
			throws RemoteException {
		int cont = 0;
		if (nomeLogico == null)
			return new Remote[0];
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] != null && nomeLogico.equals((String) table[i][0]))
				cont++;
		Remote[] risultato = new Remote[cont];
		// Ora lo uso come indice per il riempimento
		cont = 0;
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] != null && nomeLogico.equals((String) table[i][0]))
				risultato[cont++] = (Remote) table[i][1];
		return risultato;
	}

	/** Restituisce tutti i riferimenti corrispondenti ad un nome logico */
	public synchronized Object[][] restituisciTutti() throws RemoteException {
		int cont = 0;
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] != null)
				cont++;
		Object[][] risultato = new Object[cont][2];
		// Ora lo uso come indice per il riempimento
		cont = 0;
		for (int i = 0; i < tableSize; i++)
			if (table[i][0] != null) {
				risultato[cont][0] = table[i][0];
				risultato[cont][1] = table[i][1];
			}
		return risultato;
	}

	/** Elimina la prima entry corrispondente al nome logico indicato */
	public synchronized boolean eliminaPrimo(String nomeLogico)
			throws RemoteException {
		boolean risultato = false;
		if (nomeLogico == null)
			return risultato;
		for (int i = 0; i < tableSize; i++)
			if (nomeLogico.equals((String) table[i][0])) {
				table[i][0] = null;
				table[i][1] = null;
				risultato = true;
				break;
			}
		return risultato;
	}

	public synchronized boolean eliminaTutti(String nomeLogico) throws RemoteException {
		boolean risultato = false;
		if (nomeLogico == null)
			return risultato;
		for (int i = 0; i < tableSize; i++)
			if (nomeLogico.equals((String) table[i][0])) {
				if (risultato == false)
					risultato = true;
				table[i][0] = null;
				table[i][1] = null;
			}
		return risultato;
	}
	
	public synchronized String[] cercaTag(String tag) throws RemoteException {
		int cont = 0;
		if (tag == null) {
			return new String[0];
		}
		
		//trovo prima la dimensione giusta
		for (int i = 0; i < tableSize; i++) {
			if ( tagTable[i][1] != null && tag.equals(tagTable[i][1]) ) {
				cont++;
			}
		}
		
		String[] risultato = new String[cont];
		// Ora lo uso come indice per il riempimento
		cont = 0;
		for (int i = 0; i < tableSize; i++) {
			if ( tagTable[i][1] != null && tag.equals(tagTable[i][1]) ) {
				risultato[cont++] = tagTable[i][0];
			}
		}
		
		return risultato;
	}
	
	public boolean associaTag(String nomeLogico, String tag) throws RemoteException {
		// Cerco la prima posizione libera e la riempio
		boolean risultato = false;
		boolean tagPresente = false;
		
		if ((nomeLogico == null) || (tag == null)) {
			return risultato;
		}
		
		//controllo che il tag non sia presente
		for(String tagCorrente : tagDisponibili) {
			if( tag.equals(tagCorrente) )
				tagPresente=true;
		}
		
		if( !tagPresente ) {
			return risultato;
		}
		
		for (int i = 0; i < tableSize; i++) {
			if (tagTable[i][0] == null) {
				tagTable[i][0] = nomeLogico;
				tagTable[i][1] = tag;
				risultato = true;
				break;
			}
		}
		
		return risultato;
	}

	// Avvio del Server RMI
	public static void main(String[] args) {

		int registryRemotoPort = 1099;
		String registryRemotoHost = "localhost";
		String registryRemotoName = "RegistryRemoto";

		// Controllo dei parametri della riga di comando
		if (args.length != 0 && args.length != 1) {
			System.out.println("Sintassi: ServerImpl [registryPort]");
			System.exit(1);
		}
		if (args.length == 1) {
			try {
				registryRemotoPort = Integer.parseInt(args[0]);
			} catch (Exception e) {
				System.out.println("Sintassi: ServerImpl [registryPort], registryPort intero");
				System.exit(2);
			}
		}

		// Registrazione del servizio RMI
		String completeName = "//" + registryRemotoHost + ":" + registryRemotoPort + "/" + registryRemotoName;
		try {
			RegistryRemotoTagImpl serverRMI = new RegistryRemotoTagImpl();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + registryRemotoName + "\" registrato");
		} catch (Exception e) {
			System.err.println("Server RMI \"" + registryRemotoName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
