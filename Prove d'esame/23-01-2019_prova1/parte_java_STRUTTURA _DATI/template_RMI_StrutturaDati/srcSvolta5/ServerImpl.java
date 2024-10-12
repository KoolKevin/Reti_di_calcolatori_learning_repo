import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.io.File;

public class ServerImpl extends UnicastRemoteObject implements Server {
	//dichiarazione della struttura dati nel server
	private static final int MAX_PRENOTAZIONI = 20;
	private static Prenotazione prenotazioni[] = new Prenotazione[MAX_PRENOTAZIONI];

	// Costruttore
	public ServerImpl() throws RemoteException {
		super();
	}


	/* --------------- IMPLEMENTAZIONE DEI METODI ---------------- */


	public synchronized int elimina_prenotazione(String targa) throws RemoteException {
		int esito = -1;
		
		System.out.println("Ricevuta richiesta di eliminazione della prenotazione della targa: " + targa);

		for(int i = 0; i < prenotazioni.length; i++) {
			if( targa.equals(prenotazioni[i].getTarga()) ) {
				esito = 0;
				
				prenotazioni[i].setTarga("L");
				prenotazioni[i].setPatente("L");
				prenotazioni[i].setTipo("L");
				prenotazioni[i].setDirectoryImg("L");
			}
		}

		//elimino anche il direttorio delle immagini relativo
		if(esito==0) {
			File directoryImg = new File(targa+"_img");

			if (directoryImg.exists() && directoryImg.isDirectory()) {
				File[] files = directoryImg.listFiles();

				for(File f : files) {
					f.delete();
				}
			}

			directoryImg.delete();
		}


		//STAMPA DI DEBUG
		System.out.println("TARGA\t\tPATENTE\t\tTIPO\t\tDIRETTORIO");
		for(int i = 0; i < 10; i++) {
			System.out.println(prenotazioni[i].getTarga()+"\t\t"+prenotazioni[i].getPatente()+"\t\t"+prenotazioni[i].getTipo()+"\t\t"+prenotazioni[i].getDirectoryImg());
		}

		return esito;
	}

	// Ritorno il campo
	public Prenotazione[] visualizza_prenotazioni(String tipo) throws RemoteException {
		System.out.println("Ricevuta richiesta di visualizzazione delle prenotazioni di " + tipo);

		//per ottenere la giusta dimensione del risultato dovrei fare prima uno scorrimento
		Prenotazione[] result = new Prenotazione[MAX_PRENOTAZIONI];	


		//inizializzazione risultato
		for (int i = 0; i < MAX_PRENOTAZIONI; i++) {
			result[i] = new Prenotazione();

			result[i].setTarga("L");
			result[i].setPatente("L");
			result[i].setTipo("L");
			result[i].setDirectoryImg("L");
		}

		int indice = 0;

		for(Prenotazione p : prenotazioni) {
			if( tipo.equals(p.getTipo()) && ( p.getTarga().charAt(0) >= 'E' && p.getTarga().charAt(1) >= 'D') ) {
				result[indice].setTarga(p.getTarga());
				result[indice].setPatente(p.getPatente());
				result[indice].setTipo(p.getTipo());
				result[indice].setDirectoryImg(p.getDirectoryImg());

				indice++;
			}
		}


		return result;
	}


	/* ------------------------------------------------------ */


	// Registrazione del Server RMI
	public static void main(String[] args) {

		// inizializzazione struttura dati nel server
		for (int i = 0; i < MAX_PRENOTAZIONI; i++) {
			prenotazioni[i] = new Prenotazione();

			prenotazioni[i].setTarga("L");
			prenotazioni[i].setPatente("L");
			prenotazioni[i].setTipo("L");
			prenotazioni[i].setDirectoryImg("L");
		}

		prenotazioni[0].setTarga("AN745NL");
        prenotazioni[0].setPatente("00000");
        prenotazioni[0].setTipo("auto");
        prenotazioni[0].setDirectoryImg("toGet");

        prenotazioni[1].setTarga("GD111SD");
        prenotazioni[1].setPatente("00001");
        prenotazioni[1].setTipo("camper");
        prenotazioni[1].setDirectoryImg("toGet");

		System.out.println("TARGA\t\tPATENTE\t\tTIPO\t\tDIRETTORIO");
		for(int i = 0; i < 10; i++) {
			System.out.println(prenotazioni[i].getTarga()+"\t\t"+prenotazioni[i].getPatente()+"\t\t"+prenotazioni[i].getTipo()+"\t\t"+prenotazioni[i].getDirectoryImg());
		}


		/* registrazione del server presso RMIRegistry */
		final int REGISTRYPORT = 1099;
		String registryHost = "localhost";
		String serviceName = "Server"; // lookup name...
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