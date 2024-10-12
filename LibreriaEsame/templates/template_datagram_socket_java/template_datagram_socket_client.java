import java.io.*;
import java.net.*;

public class DatagramClient {     //cambia il nome della classe

	public static void main(String[] args) {

		InetAddress addr = null;
		int port = -1;
		
		//controllo argomenti e inizializzazione ip e porta server
		try {
			if (args.length == 2) {
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
			} else {
				System.out.println("Usage: java Client serverIP serverPort");
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

		// creazione della socket datagram, settaggio timeout di 30s, e creazione datagram packet
		try {
			socket = new DatagramSocket();
			socket.setSoTimeout(30000);
			packet = new DatagramPacket(buf, buf.length, addr, port);
			System.out.println("\nClient: avviato");
			System.out.println("Creata la socket: " + socket);
		} catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.out.println("Client: interrompo...");
			System.exit(1);
		}

		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		//interazione con l'utente
		System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci XXXXXXX : ");
		try {
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
			byte[] data = null;



            /*----------- QUESTE VARIABILI DIPENDONO DALL'ESERCIZIO RICHIESTO -------------*/
			String nomeFile = null;
			int numLinea = -1;

			String richiesta = null;
			String risposta = null;
            /*-----------------------------------------------------------------------------*/



            //CICLO FINO A EOF PER LEGGERE L'INPUT DELL'UTENTE
			while ((nomeFile = stdIn.readLine()) != null) {
				// interazione con l'utente e costruzione della richiesta
				try {
					System.out.print("Numero della linea? ");
					numLinea = Integer.parseInt(stdIn.readLine());
					richiesta = nomeFile + " " + numLinea;
				} catch (Exception e) {
					System.out.println("Problemi nell'interazione da console: ");
					e.printStackTrace();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci XXXXXXX : ");
					continue;
				}

				// riempimento e invio del pacchetto
				try {
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					doStream.writeUTF(richiesta);
					data = boStream.toByteArray();

					packet.setData(data);
					socket.send(packet);

					System.out.println("Richiesta inviata a " + addr + ", " + port);
				} catch (IOException e) {
					System.out.println("Problemi nell'invio della richiesta: ");
					e.printStackTrace();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci XXXXXXX : ");
					continue;
				}

				//ricezione della risposta
				try {
					packet.setData(buf);
					socket.receive(packet);
				} catch (IOException e) {
					System.out.println("Problemi nella ricezione del datagramma: ");
					e.printStackTrace();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci XXXXXXX : ");
					continue;
				}
                //interpretazione della risposta
				try {
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);
					risposta = diStream.readUTF();
					System.out.println("Risposta: " + risposta);
				} catch (IOException e) {
					System.out.println("Problemi nella lettura della risposta: ");
					e.printStackTrace();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci XXXXXXX : ");
					continue;
				}
			
				System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci XXXXXXX : ");
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