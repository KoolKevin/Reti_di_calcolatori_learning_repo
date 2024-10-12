// LineClient.java

import java.io.*;
import java.net.*;
import java.util.StringTokenizer;

public class RowSwapClient {

	public static void main(String[] args) {
		
		//args[0] = IpDiscoveryServer
		//args[1] = portaDiscoveryServer
		//args[2] = nome file di interesse
		
		InetAddress addr = null;
		int port = -1;
		
		try {
			if (args.length == 3) {
		    	    addr = InetAddress.getByName(args[0]);
		    	    port = Integer.parseInt(args[1]);
			} else {
			    System.out.println("Usage: java RowSwapClient dsIP dsPort filename");
			    System.exit(1);
			}
		} catch (UnknownHostException e) {
			System.out.println("Problemi nella determinazione dell'endpoint del server : ");
			e.printStackTrace();
			System.out.println("RowSwapClient: interrompo...");
			System.exit(2);
		}
	
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[256];
		

		// creazione della socket datagram, settaggio timeout di 30s
		// e creazione datagram packet
		try {
			socket = new DatagramSocket();
			socket.setSoTimeout(30000);
			packet = new DatagramPacket(buf, buf.length, addr, port);
			System.out.println("\nRowSwapClient: avviato");
			System.out.println("Creata la socket: " + socket);
		} catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.out.println("RowSwapClient: interrompo...");
			System.exit(1);
		}
		
		ByteArrayOutputStream boStream = null;
		DataOutputStream doStream = null;
		byte[] data = null;
		
		// riempimento e invio del pacchetto contenente il nome del file
		try {
			boStream = new ByteArrayOutputStream();
			doStream = new DataOutputStream(boStream);
			doStream.writeUTF(args[2]);
			data = boStream.toByteArray();
			packet.setData(data);
			socket.send(packet);
			System.out.println("Richiesta inviata a " + addr + ", " + port);
		} catch (IOException e) {
			System.out.println("Problemi nell'invio della richiesta: ");
			e.printStackTrace();
			System.exit(1); // il client esce dato che non è riuscito ad inviare la richiesta
		}
		
		// settaggio del buffer di ricezione
		try {
			packet.setData(buf);
			socket.receive(packet);   // sospensiva solo per i millisecondi indicati, dopodich� solleva una SocketException
		} catch (IOException e) {
			System.out.println("Problemi nella ricezione del datagramma: ");
			e.printStackTrace();
			
			System.exit(1); // il client esce dato che non è riuscito a ricevere la risposta
		}
		
		//ricezione porta RowSwapServer
		String risposta = null;
		ByteArrayInputStream biStream = null;
		DataInputStream diStream = null;
		
		try {
			biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
			diStream = new DataInputStream(biStream);
			risposta = diStream.readUTF();
			System.out.println("Risposta: " + risposta);
			
			if( risposta.equals("non ho trovato nessun RowSwapServer che si occupa di quel file") ) {
				System.out.println("RowSwapClient: esco.....");
				System.exit(1);
			}
		} catch (IOException e) {
			System.out.println("Problemi nella lettura della risposta: ");
			e.printStackTrace();
			System.exit(1); // il client esce dato che non è riuscito ad elaborare la risposta
		}
	
		// tutto ok, pronto per iniziare a mandare richieste di rowswap
		System.out.println("\nRowSwapServer trovato sulla porta: " + risposta);
		//cambio il destinatario del pacchetto al RowSwapServer
		packet.setPort(Integer.parseInt(risposta));

		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci il numero delle righe da scambiare separate da UNO spazio(es. \"1 3\"): ");
		
		String richiesta = null;
		StringTokenizer st = null;
		String indice1 = null, indice2 = null;
		
		try {
			while ((richiesta = stdIn.readLine()) != null) {
				// interazione con l'utente e controllo formato richiesta
				try {
					st = new StringTokenizer(richiesta);
					indice1 = st.nextToken();
					indice2 = st.nextToken();
					Integer.parseInt(indice1);
					Integer.parseInt(indice2);
				} catch (Exception e) {
					System.out.println("Formato sbagliato della richiesta! ");
					e.printStackTrace();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci il numero delle righe da scambiare separate da UNO spazio(es. \"1 3\"): ");
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
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci il numero delle righe da scambiare separate da UNO spazio(es. \"1 3\"): ");
					continue;  
				}

				try {
					// settaggio del buffer di ricezione
					packet.setData(buf);
					socket.receive(packet); // sospensiva solo per i millisecondi indicati, dopodich� solleva una SocketException
				} catch (IOException e) {
					System.out.println("Problemi nella ricezione del datagramma: ");
					e.printStackTrace();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci il numero delle righe da scambiare separate da UNO spazio(es. \"1 3\"): ");
					continue; 
				}
				try {
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);
					risposta = diStream.readUTF();
					System.out.println("Risposta: " + risposta);
				} catch (IOException e) {
					System.out.println("Problemi nella lettura della risposta: ");
					e.printStackTrace();
					System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci il numero delle righe da scambiare separate da UNO spazio(es. \"1 3\"): ");
					continue;
				}
			
				// tutto ok, pronto per nuova richiesta
				System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci il numero delle righe da scambiare separate da UNO spazio(es. \"1 3\"): ");
			} // while
		}
		// qui catturo le eccezioni non catturate all'interno del while
		// in seguito alle quali il client termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
		}

		System.out.println("\nRowSwapClient: termino...");
		socket.close();
	}
}