// LineServer.java

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.StringTokenizer;

public class DiscoveryServer {

	public static void main(String[] args) {

		DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[256];
		
		int dsPort = -1;
		String[] rsFilenames = new String[(args.length - 1)/2];
		int[] rsPorts = new int[(args.length - 1)/2];
		
		int i=0;  //indice args
		int j=0;  //indice nomiFile;
		int k=0;  //indice numeriPorta
		

		// CONTROLLO ARGOMENTI: 3 o più argomenti, ma sempre dispari( porta DiscServer + coppie (nomeFile, porta) dei RsServer )
		if ((args.length >= 3) && (args.length%2 == 1) ) {
			try {
				// riempio le variabili di supporto
				j=0;
				k=0;
				for( i=0; i<args.length; i++ ) {
					if(i%2 == 0) {
						//args[i] è un numero di porta
						if(i==0) {
							dsPort = Integer.parseInt(args[i]);
						}
						else {
							rsPorts[k] = Integer.parseInt(args[i]);
							k++;
						}
					} 
					else {
						//args[i] è un nome file
						rsFilenames[j] = args[i];
						j++;
					}
				}
				
				//controllo i valori delle porte
				if (dsPort < 1024 || dsPort > 65535) {
					System.out.println("Usage: java DiscoveryServer [dsPort>1024] coppie([filename] [rsPort>1024])");
					System.exit(1);
				}
				
				for(k=0; k<rsPorts.length; k++) {
					if (rsPorts[k] < 1024 || rsPorts[k] > 65535) {
						System.out.println("Usage: java DiscoveryServer [dsPort>1024] coppie([filename] [rsPort>1024])");
						System.exit(1);
					}
				}
				
				//controllare che lo stesso file non sia associato a più di un rsServer
				int j2;
				for(j=0; j<rsFilenames.length; j++) {
					for(j2=0; j2<rsFilenames.length; j2++) {
						if( j!=j2 && rsFilenames[j2].equals(rsFilenames[j]) ) {
							System.out.println("Non ripetere i nomi dei file, ogni rsServer gestisce un file distinto");
							System.exit(1);
						}
					}
				}
				
				//controllare che un numero di porta non venga ripetuto
				int k2;
				for(k=0; k<rsPorts.length; k++) {
					for(k2=0; k2<rsPorts.length; k2++) {
						if( k!=k2 && rsPorts[k2]==rsPorts[k] ) {
							System.out.println("Non ripetere i numeri di porta");
							System.exit(1);
						}
					}
					
					if( rsPorts[k]==dsPort ) {
						System.out.println("Non ripetere i numeri di porta");
						System.exit(1);
					}
				}
				
			} catch (NumberFormatException e) {
				System.out.println("Usage: java DiscoveryServer [dsPort>1024] coppie([filename] [rsPort>1024])");
				System.exit(1);
			}
		} else {
			System.out.println("Usage: java DiscoveryServer [dsPort>1024] coppie([filename] [rsPort>1024])");
			System.exit(1);
		}
		
		System.out.println("Discovery server: avviato");
		
		//prima creiamo i rowSwapServer
		RowSwapServer[] rsServers = new RowSwapServer[(args.length - 1)/2];
		
		for(i=0; i<rsPorts.length; i++) {
			rsServers[i] = new RowSwapServer(rsFilenames[i], rsPorts[i]);
			rsServers[i].start();
		}

		try {
			socket = new DatagramSocket(dsPort);
			packet = new DatagramPacket(buf, buf.length);
			System.out.println("Creata la socket: " + socket);
		}
		catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.exit(1);
		}

		try {
			String nomeFile = null;
			String risposta = null;
			String linea = null;
			boolean trovato;
			
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;
			StringTokenizer st = null;
			byte[] data = null;
			
			
			while (true) {
				System.out.println("\nIn attesa di richieste per rsServer...");
				
				// ricezione del datagramma
				try {
					packet.setData(buf);
					socket.receive(packet);
				}
				catch (IOException e) {
					System.err.println("Problemi nella ricezione del datagramma: "+ e.getMessage());
					e.printStackTrace();
					continue; // il server continua a fornire il servizio ricominciando dall'inizio  del ciclo
				}
				
				//ho ricevuto un nome di file
				try {
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);
					nomeFile = diStream.readUTF();

					System.out.println("Richiesta porta del file " + nomeFile);
				}
				catch (Exception e) {
					System.err.println("Problemi nella lettura della richiesta: "+ nomeFile);
					e.printStackTrace();
					continue;
				}

				// preparazione invio della risposta
				try {
					//cerco di trovare la porta associata al nome del file
					trovato=false;
					
					for(i=0; i<rsPorts.length; i++) {
						if( rsFilenames[i].equals(nomeFile) ){
							trovato=true;
							risposta = Integer.toString(rsPorts[i]);
						}
					}
					
					if(!trovato) {
						risposta = "non ho trovato nessun RowSwapServer che si occupa di quel file";
					}
					
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					doStream.writeUTF(risposta);
					data = boStream.toByteArray();
					packet.setData(data, 0, data.length);
					socket.send(packet);
				}
				catch (IOException e) {
					System.err.println("Problemi nell'invio della risposta: "+ e.getMessage());
					e.printStackTrace();
					continue;
				}

			} // while

		}
		// qui catturo le eccezioni non catturate all'interno del while in seguito alle quali il server termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
		}
		
		System.out.println("DiscoveryServer: termino...");
		socket.close();
	}
}
