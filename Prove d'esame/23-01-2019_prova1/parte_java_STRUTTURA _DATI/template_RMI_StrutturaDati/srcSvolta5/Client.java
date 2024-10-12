import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.rmi.Naming;
import java.rmi.NotBoundException;

class Client {

	public static void main(String[] args) {
		final int REGISTRYPORT = 1099;
		String registryHost = null; // host remoto con registry
		String serviceName = "Server";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo argomenti
		if (args.length < 1) {
			System.out.println("Sintassi: RMI_Registry_IP [ServiceName]");
			System.exit(1);
		}

		registryHost = args[0];

		if( args.length == 2) {
			serviceName = args[1];
		}

		System.out.println("Invio richieste a " + registryHost + " per il servizio di nome " + serviceName);

		Prenotazione[] prenotazioni;
		String tipo;
		
		try {
			// Connessione al servizio RMI remoto
			String completeName = "//" + registryHost + ":" + REGISTRYPORT + "/" + serviceName;
			Server serverRMI = (Server) Naming.lookup(completeName);
			System.out.println("ClientRMI: Servizio \"" + serviceName + "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");


			/* ciclo accettazione richieste utente */
			String service;
			System.out.print("Servizio (V=visualizzazione prenotazioni di un certo tipo, E=eliminazione di una prenotazione): ");
			
			while ((service = stdIn.readLine()) != null) {

				/* DISPATCHING DEL METODO RICHIESTO */

				if (service.equals("V")) {
					/* CHIEDO ALL'UTENTE I PARAMETRI PER IL METODO */
					System.out.print("Tipologia veicolo(auto, macchina)? ");
					tipo = stdIn.readLine(); // qualsiasi tipo accettato, nessun check

					/* INVOCO IL METODO REMOTO, e controllo esito */
					prenotazioni = serverRMI.visualizza_prenotazioni(tipo);
					
					System.out.println("TARGA\t\tPATENTE\t\tTIPO\t\tDIRETTORIO");
					for(Prenotazione p : prenotazioni) {
						if(p.getTarga().equals("L"))
							break;

						System.out.println(p.getTarga()+"\t\t"+p.getPatente()+"\t\t"+p.getTipo()+"\t\t"+p.getDirectoryImg());
					}
				}
				else if (service.equals("E")) {
					int esito;

					/* CHIEDO ALL'UTENTE I PARAMETRI PER IL METODO */
					System.out.print("targa(esattamente 7 caratteri)? ");
					String targa = stdIn.readLine(); // qualsiasi tipo accettato, nessun check
					/* INVOCO IL METODO REMOTO */
					esito = serverRMI.elimina_prenotazione(targa);

					if(esito < 0 ) {
						System.out.println("Targa non trovata");
					}
					else{
						System.out.println("eliminazione avvenuta con successo");
					}
					
				} // P=Programma
				else {
					System.out.println("Servizio non disponibile");
				}

				System.out.print("Servizio (V=visualizzazione prenotazioni di un certo tipo, E=eliminazione di una prenotazione): ");
			} // while (!EOF), fine richieste utente

		} catch (NotBoundException nbe) {
			System.err.println("ClientRMI: il nome fornito non risulta registrato; " + nbe.getMessage());
			nbe.printStackTrace();
			System.exit(1);
		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}