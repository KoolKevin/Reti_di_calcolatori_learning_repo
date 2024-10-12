import java.rmi.*;
import java.io.*;
import java.util.List;
import java.util.ArrayList;

class Client {

	public static void main(String[] args) {
		int registryPort = 1099;
		String registryHost = null;
		String serviceName = "Server";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo parametri
		if (args.length != 1 && args.length != 2) {
			System.out.println("Sintassi: Client RegistryHost [registryPort]");
			System.exit(1);
		} else {
			registryHost = args[0];
			if (args.length == 2) {
				try {
					registryPort = Integer.parseInt(args[1]);
				} catch (Exception e) {
					System.out.println("Sintassi: Client RegistryHost [registryPort]");
					System.exit(1);
				}
			}
		}

		// Connessione al servizio RMI remoto
		try {
			String completeName = "//" + registryHost + ":" + registryPort + "/"+ serviceName;
			Server serverRMI = (Server) Naming.lookup(completeName);
			System.out.println("Client RMI: Servizio \"" + serviceName+ "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");

			String service;
			System.out.print("Servizio (L=lista file, N=numera righe): ");

			String nomeDir;
			List<String> listaFile = null;;

			while ((service = stdIn.readLine()) != null) {

				if (service.equals("L")) {
					System.out.print("Nome directory? ");
					nomeDir = stdIn.readLine();

					// Invocazione remota
					try {
						listaFile = serverRMI.lista_file(nomeDir);
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
						continue;
					}

					System.out.println("Ecco la lista dei file:");
					for(String file : listaFile) {
						System.out.println("\t" + file);
					}
				} 
				else
					if (service.equals("N")) {
						String nomeFile;
						System.out.print("Nome file? ");
						nomeFile = stdIn.readLine();

						int res = 0;
						try {
							res = serverRMI.numera_righe(nomeFile);
							System.out.println("\tHo contato "+res+" righe dispari!\n");
						} catch (RemoteException re) {
							System.out.println("Errore remoto: " + re.toString());
						}
					} // S
					else
						System.out.println("Servizio non disponibile");

				System.out.print("Servizio (L=lista file, N=numera righe): ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}