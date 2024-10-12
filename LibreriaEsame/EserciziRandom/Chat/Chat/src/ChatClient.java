import java.io.*;
import java.net.*;



public class ChatClient {     //cambia il nome della classe

	public static void main(String[] args) {

		InetAddress addr = null;
		int port = -1;
		int writePort = -1;
		int listenPort = -1;
		int chatPort = -1;
		
		//controllo argomenti e inizializzazione ip e porta server
		try {
			if (args.length == 4) {
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
				writePort = Integer.parseInt(args[2]);
				listenPort = Integer.parseInt(args[3]);
			} else {
				System.out.println("Usage: java Client serverIP serverPort writeClientPort listenClientPort");
			    System.exit(1);
			}
		} catch (UnknownHostException e) {
			System.out.println("Problemi nella determinazione dell'endpoint del server : ");
			e.printStackTrace();
			System.out.println("Client: interrompo...");
			System.exit(2);
		}
	
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[256];

		try {
			socket = new DatagramSocket(writePort);
			//socket.setSoTimeout(30000);
			packet = new DatagramPacket(buf, buf.length, addr, port);
			System.out.println("Client: avviato");
			System.out.println("Creata la socket: " + socket);
		} catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.out.println("Client: interrompo...");
			System.exit(1);
		}

		/* Inizializzazione e attivazione del chat server */
		ListeningClient listeningClient;
		listeningClient = new ListeningClient(listenPort);
		listeningClient.start();

		ByteArrayOutputStream boStream = null;
		DataOutputStream doStream = null;
		ObjectOutputStream ooStream = null;
		ByteArrayInputStream biStream = null;
		DataInputStream diStream = null;
		byte[] data = null;
		String nome = null;

		/* interazione con l'utente */
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		System.out.print("Inserisci il nome con cui ti vuoi registrare: ");
		try {
			nome = stdIn.readLine();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}

		//invio richiesta
		try {
			boStream = new ByteArrayOutputStream();
			ooStream = new ObjectOutputStream(boStream);
			InetSocketAddress ricevente = new InetSocketAddress(InetAddress.getLocalHost(), listenPort);
			MessaggioDiRegistrazione messaggioDiRegistrazione = new MessaggioDiRegistrazione(ricevente, nome);
			ooStream.writeObject(messaggioDiRegistrazione);
			data = boStream.toByteArray();

			packet.setData(data);
			socket.send(packet);
		} catch (IOException e) {
			System.out.println("Problemi nell'invio della richiesta: ");
			e.printStackTrace();
			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti scrivi un messaggio: ");
			System.exit(1);
		}

		//ricezione porta di risposta
		try {
			packet.setData(buf);
			socket.receive(packet);
		} catch (IOException e) {
			System.out.println("Problemi nella ricezione del datagramma: ");
			e.printStackTrace();
			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti scrivi un messaggio: ");
			System.exit(1);
		}
		//interpretazione della risposta
		try {
			biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
			diStream = new DataInputStream(biStream);
			chatPort = diStream.readInt();
			System.out.println("devo scrivere sulla porta: " + chatPort);

			packet.setPort(chatPort);
		} catch (IOException e) {
			System.out.println("Problemi nella lettura della risposta: ");
			e.printStackTrace();
			System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti scrivi un messaggio: ");
			System.exit(1);
		}

		/* CHAT */

		System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti scrivi un messaggio\n"+nome+">: ");
		try {
            /*----------- QUESTE VARIABILI DIPENDONO DALL'ESERCIZIO RICHIESTO -------------*/
			String messaggio = null;
            /*-----------------------------------------------------------------------------*/


            //CICLO FINO A EOF PER LEGGERE L'INPUT DELL'UTENTE
			while ((messaggio = stdIn.readLine()) != null) {
			
				// riempimento e invio del pacchetto
				try {
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					messaggio = nome+">: "+messaggio;
					doStream.writeUTF(messaggio);
					data = boStream.toByteArray();

					packet.setData(data);
					socket.send(packet);
				} catch (IOException e) {
					System.out.println("Problemi nell'invio della richiesta: ");
					e.printStackTrace();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti scrivi un messaggio\n"+nome+">: ");
					continue;
				}

				System.out.print(nome+">: ");
			} // while
		}
		// qui catturo le eccezioni non catturate all'interno del while in seguito alle quali il client termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
		}

		System.out.println("Client: termino...");
		socket.close();
	}
}