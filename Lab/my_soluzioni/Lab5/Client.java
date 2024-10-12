import java.io.*;
import java.rmi.Naming;
import java.rmi.NotBoundException;

class Client {

	public static void main(String[] args) {
		final int REGISTRYPORT = 12345;
		String registryHost = null; // host remoto con registry
		String serviceName = "";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo dei parametri della riga di comando
		if (args.length != 2) {
			System.out.println("Sintassi: RMI_Registry_IP ServiceName");
			System.exit(1);
		}
		registryHost = args[0];
		serviceName = args[1];

		System.out.println("Invio richieste a " + registryHost + " per il servizio di nome " + serviceName);

		// Connessione al servizio RMI remoto
		try {
			String completeName = "//" + registryHost + ":" + REGISTRYPORT + "/" + serviceName;
			RemOp serverRMI = (RemOp) Naming.lookup(completeName);
			System.out.println("ClientRMI: Servizio \"" + serviceName + "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");

			String service;
			System.out.print("Servizio (C=conta_righe(nomefile, soglia), E=eliminariga(nomefile, indiceriga) ): ");

			/* ciclo accettazione richieste utente */
			while ((service = stdIn.readLine()) != null) {
				//conta righe
				if (service.equals("C")) {
					String nomeFile = "";
					System.out.print("nome file? ");
					nomeFile=stdIn.readLine();
						
					boolean ok = false;
					int soglia = 0;
					System.out.print("soglia? ");
					while (ok != true) {
						try{
							soglia = Integer.parseInt(stdIn.readLine());
							if (soglia < 0) {
								throw new NumberFormatException("soglia minore di zero");
							} else {
								ok = true; // soglia inserita valida
							}
						} catch(NumberFormatException e) {
							System.out.println(e.getMessage());
							e.printStackTrace();
							System.out.print("soglia? ");
						}
					}

					int numRighe = serverRMI.conta_righe(nomeFile, soglia);
					System.out.println("il file "+nomeFile+" ha "+numRighe+" righe con più di "+soglia+" parole");
				} // C=contaRighe

				// elimina riga
				else if (service.equals("E")) {
					String nomeFile = "";
					System.out.print("nome file? ");
					nomeFile=stdIn.readLine();
						
					boolean ok = false;
					int indiceRiga = 1;
					System.out.print("indice riga? ");
					while (ok != true) {
						try{
							indiceRiga = Integer.parseInt(stdIn.readLine());
							if (indiceRiga < 1) {
								throw new NumberFormatException("indiceRiga minore di uno");
							} else {
								ok = true; // soglia inserita valida
							}
						} catch(NumberFormatException e) {
							System.out.println(e.getMessage());
							e.printStackTrace();
							System.out.print("indice riga? ");
						}
					}

					Esito e = serverRMI.elimina_riga(nomeFile, indiceRiga);
					System.out.println(e.toString());
				} // E=eliminaRiga

				else
					System.out.println("Servizio non disponibile");

				System.out.print("Servizio (C=conta_righe(nomefile, soglia), E=eliminariga(nomefile, indiceriga) ): ");
			} // while (!EOF), fine richieste utente

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}