import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.rmi.Naming;
import java.rmi.RMISecurityManager;

class ClientCongresso {

	// Avvio del Client RMI
	public static void main(String[] args) {
		int registryRemotoPort = 1099;
		String registryRemotoHost = null;
		String registryRemotoName = "RegistryRemoto";
		String serviceName = "ServerCongresso";
		String tag = "congresso";

		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo dei parametri della riga di comando
		if (args.length != 1 && args.length != 2) {
			System.out.println("Sintassi: ClientCongresso NomeHostRegistryRemoto [registryPort], registryPort intero");
			System.exit(1);
		}

		registryRemotoHost = args[0];

		if (args.length == 2) {
			try {
				registryRemotoPort = Integer.parseInt(args[1]);
			} catch (Exception e) {
				System.out.println("Sintassi: ClientCongresso NomeHostRegistryRemoto [registryPort], registryPort intero");
				System.exit(1);
			}
		}

		// Impostazione del SecurityManager
		/*if (System.getSecurityManager() == null)
			System.setSecurityManager(new RMISecurityManager());*/

		// Connessione al servizio RMI remoto
		try {
			String completeRemoteRegistryName = "//" + registryRemotoHost + ":"+ registryRemotoPort + "/" + registryRemotoName;
			RegistryRemotoTagClient registryRemoto = (RegistryRemotoTagClient) Naming.lookup(completeRemoteRegistryName);
			
			//scelta del servizio
			int indiceServizio=-1;
			boolean ok = false;
			String[] nomiServizi;

			System.out.println("Inserisci un tag con cui cercare servizi: ");
			tag = stdIn.readLine();
			nomiServizi = registryRemoto.cercaTag(tag);

			System.out.println("ClientRMI: Servizi trovati con tag: "+tag);
			for(int i=0; i<nomiServizi.length; i++) {
				System.out.print( (i+1) + ") " + nomiServizi[i] + "\t");
			}
			System.out.println();
			System.out.println("scegli quale servizio vuoi(1, 2, 3, ..., "+nomiServizi.length+")");
			while (ok != true) {
				indiceServizio = Integer.parseInt(stdIn.readLine());
				if (indiceServizio <= 0 || indiceServizio > (nomiServizi.length) ) {
					System.out.println("input non valido");
					System.out.println("scegli quale servizio vuoi(1, 2, 3, ..., "+nomiServizi.length+")");
					continue;
				} else
					ok = true;
			}

			ServerCongresso serverRMI = (ServerCongresso) registryRemoto.cerca(nomiServizi[indiceServizio-1]);
			System.out.println("ClientRMI: Servizio \"" + serviceName + "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");

			String service;
			System.out.print("Servizio (R=Registrazione, P=Programma del congresso): ");

			while ((service = stdIn.readLine()) != null) {

				if (service.equals("R")) {

					ok = false;
					int g = 0;
					System.out.print("Giornata (1-3)? ");
					while (ok != true) {
						g = Integer.parseInt(stdIn.readLine());
						if (g < 1 || g > 3) {
							System.out.println("Giornata non valida");
							System.out.print("Giornata (1-3)? ");
							continue;
						} else
							ok = true;
					}
					ok = false;
					String sess = null;
					System.out.print("Sessione (S1 - S12)? ");

					while (ok != true) {
						sess = stdIn.readLine();
						if (!sess.equals("S1") && !sess.equals("S2") && !sess.equals("S3")
								&& !sess.equals("S4") && !sess.equals("S5")
								&& !sess.equals("S6") && !sess.equals("S7")
								&& !sess.equals("S8") && !sess.equals("S9")
								&& !sess.equals("S10") && !sess.equals("S11")
								&& !sess.equals("S12")) {
							System.out.println("Sessione non valida");
							System.out.print("Sessione (S1 - S12)? ");
							continue;
						} else
							ok = true;
					}

					System.out.print("Speaker? ");
					String speak = stdIn.readLine();

					// Tutto corretto
					if (serverRMI.registrazione(g, sess, speak) == 0)
						System.out.println("Registrazione di " + speak
								+ " effettuata per giornata " + g + " sessione " + sess);
					else
						System.out.println("Sessione piena: giornata" + g + " sessione " + sess);
				} // R

				else if (service.equals("P")) {
					int g = 0;
					ok = false;
					System.out.print("Programma giornata (1-3)? ");

					while (ok != true) {
						// intercettare la NumberFormatException
						g = Integer.parseInt(stdIn.readLine());
						if (g < 1 || g > 3) {
							System.out.println("Giornata non valida");
							System.out.print("Programma giornata (1-3)? ");
							continue;
						} else
							ok = true;
					}
					System.out.println("Ecco il programma: ");
					serverRMI.programma(g).stampa();

				} // P

				else
					System.out.println("Servizio non disponibile");

				System.out.print("Servizio (R=Registrazione, P=Programma del congresso): ");
			} // !EOF richieste utente

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(2);
		}
	}
}
