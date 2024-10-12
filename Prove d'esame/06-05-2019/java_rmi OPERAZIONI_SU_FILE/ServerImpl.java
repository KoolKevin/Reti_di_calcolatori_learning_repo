import java.io.*;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.List;
import java.util.ArrayList;

public class ServerImpl extends UnicastRemoteObject implements Server {

	// Costruttore
	public ServerImpl() throws RemoteException {
		super();
	}

	private static final char[] vocali = {'A', 'a', 'E', 'e', 'I', 'i', 'O', 'o', 'U', 'u'};

	//ipotesi semplificativa: considero nomi di file con solo caratteri alfabetici
	private boolean isVocale(char c) {
		for(char vocale : vocali) {
			if(c == vocale) {
				return true;
			}
		}
		
		return false;
	}

	public List<String>lista_file(String dir_name) throws RemoteException {
		List<String> result = new ArrayList<String>();

		File current_dir = new File(dir_name);
		int contaConsonanti = 0;

		if(current_dir.exists() && current_dir.isDirectory()) {
			File[] files = current_dir.listFiles();

			for(File f : files) {
				if(f.isDirectory()) {
					result.addAll( lista_file(f.getAbsolutePath()) );
				}
				else{
					for(int i=0; i<f.getName().length(); i++) {
						if( !isVocale(f.getName().charAt(i)) ) {
							contaConsonanti++;
						}
					}

					if( contaConsonanti >= 3) {
						result.add(f.getAbsolutePath());
					}
				}
			}
		}
		else{
			throw new RemoteException("Direttorio inesistente");
		}

		if( result.isEmpty() ) {
			throw new RemoteException("Direttorio vuoto");
		}
		
		return result;
	}

	public int numera_righe(String fileName) throws RemoteException {
		String outFileName = fileName + ".temp";
		BufferedReader fileIn;
		File out, in;
		PrintWriter fileOut;
		String line;
		int contaRighe = 0;
		int contaRigheDispari = 0;

		//controllo se il file esiste
		in = new File(fileName);

		if( !in.exists() ) {
			return -1;
		}

		//apro i file
		try {
			fileIn = new BufferedReader(new FileReader(fileName));
			out = new File(outFileName);
			fileOut = new PrintWriter(new FileWriter(out));
		} catch (FileNotFoundException e) {
			throw new RemoteException(e.toString());
		} catch (IOException e) {
			throw new RemoteException(e.toString());
		}

		//effettuo il conteggio
		try {
			while ((line = fileIn.readLine()) != null) {
				contaRighe++;

				if (contaRighe%2 == 1) {
					contaRigheDispari++;
					fileOut.println(contaRigheDispari + " - " + line);
				}
				else{
					fileOut.println(line);
				}
			}

			fileIn.close();
			fileOut.close();

			// Replace the original file with the tmp
			in.delete();
			out.renameTo(in);

		} catch (IOException e) {
			throw new RemoteException(e.toString());
		}
		
		return contaRigheDispari;
	}

	public static void main(String[] args) {

		int registryPort = 1099;
		String registryHost = "localhost";
		String serviceName = "Server";

		// Controllo parametri
		if (args.length != 0 && args.length != 1) {
			System.out.println("Sintassi: ServerImpl [registryPort]");
			System.exit(1);
		}
		if (args.length == 1) {
			try {
				registryPort = Integer.parseInt(args[0]);

				if (registryPort < 1024 || registryPort > 65535) {
                    System.out.println("Porta in un range invalido: java PutFileServerCon [serverPort>1024]");
                    System.exit(1);
                }
			} catch (Exception e) {
				System.out.println("Sintassi: ServerImpl [registryPort], registryPort intero");
				System.exit(2);
			}
		}

		// Registrazione del servizio RMI
		String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;

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