import java.io.*;
import java.rmi.Naming;
import java.rmi.NotBoundException;

class RMI_Client {

	public static void main(String[] args) {
		final int REGISTRYPORT = 12345;
		String registryHost = null; // host remoto con registry
		String serviceName = "";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		String nomeDirectory, nomeFile, parola;

		// Controllo dei parametri della riga di comando
		if (args.length != 2) {
			System.out.println("Sintassi: RMI_Registry_IP ServiceName");
			System.exit(1);
		}
		registryHost = args[0];
		serviceName = args[1];

		System.out.println("Invio richieste a " + registryHost + " per il servizio di nome " + serviceName);
		
		try {
			String completeName = "//" + registryHost + ":" + REGISTRYPORT + "/" + serviceName;
			RMI_interfaceFile serverRMI = (RMI_interfaceFile) Naming.lookup(completeName);
			System.out.println("ClientRMI: Servizio \"" + serviceName + "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");

			String service;
			System.out.print("Servizio (L=lista_nomi_file_contenenti_parola_in_linea, C=conta_numero_linee): ");

			/* ciclo accettazione richieste utente */
			while ((service = stdIn.readLine()) != null) {
				if (service.equals("L")) {
					System.out.print("nome directory? ");
					nomeDirectory=stdIn.readLine();
						
					System.out.print("parola? ");
					parola=stdIn.readLine();
					
					System.out.println( serverRMI.lista_nomi_file_contenenti_parola_in_linea(nomeDirectory, parola) );
				}
				else if (service.equals("C")) {
					System.out.print("nome file? ");
					nomeFile=stdIn.readLine();
						
					System.out.print("parola? ");
					parola=stdIn.readLine();
					
					System.out.println( "ho trovate :" + serverRMI.conta_numero_linee(nomeFile, parola) +" righe che contengono " + parola );
				} 
				else
					System.out.println("Servizio non disponibile");

				System.out.print("Servizio (L=lista_nomi_file_contenenti_parola_in_linea, C=conta_numero_linee): ");
			} // while (!EOF), fine richieste utente

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}