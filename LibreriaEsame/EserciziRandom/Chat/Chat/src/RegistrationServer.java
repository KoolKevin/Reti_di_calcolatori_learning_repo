import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.ObjectInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.util.Map;
import java.util.HashMap;

public class RegistrationServer {   //cambia il nome della classe

	// porta di default del server
	private static final int REGISTRATION_PORT = 4445;
	private static final int CHAT_PORT = 4446;

	//TODO: in questa struttura dati dovrei mantenere anche la writing port del client
	private static Map<String, InetSocketAddress> mappaUtenti = new HashMap<String, InetSocketAddress>(); 

	public static void main(String[] args) {
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[2048];
		int port = -1;

		// controllo argomenti input: 0 oppure 1 argomento (porta)
		if ((args.length == 0)) {
			port = REGISTRATION_PORT;
		} 
		else if (args.length == 1) {
			try {
				port = Integer.parseInt(args[0]);
				// controllo che la porta sia nel range consentito 1024-65535
				if (port < 1024 || port > 65535) {
					System.out.println("Usage: java RegistrationServer [serverPort>1024]");
					System.exit(1);
				}
			} catch (NumberFormatException e) {
				System.out.println("Usage: java RegistrationServer [serverPort>1024]");
				System.exit(1);
			}
		} 
		else {
			System.out.println("Usage: java RegistrationServer [serverPort>1024]");
			System.exit(1);
		}

        //creazione socket
		try {
			socket = new DatagramSocket(port);
			packet = new DatagramPacket(buf, buf.length);
			System.out.println("Creata la socket: " + socket);
		}
		catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.exit(1);
		}
        
        //messaggio di avvio
        try {
			System.out.println("RegistrationServer: avviato "+InetAddress.getLocalHost()+":"+port);
		}
		catch(UnknownHostException e ) {
			System.out.println("Problemi nel reperimento dell'indirizzo ip del server ");
			e.printStackTrace();
			System.exit(1);
		}

		/* Inizializzazione e attivazione del chat server */
		ChatServer chatServer;
		chatServer = new ChatServer(mappaUtenti, CHAT_PORT);
		chatServer.start();
		
		//corpo del server
		try {
            ByteArrayInputStream biStream = null;
			ObjectInputStream oiStream = null;
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;		

            /*----------- QUESTE VARIABILI DIPENDONO DALL'ESERCIZIO RICHIESTO -------------*/
			MessaggioDiRegistrazione richiesta = null;
			byte[] data = null;
			boolean trovato = false;
            /*-----------------------------------------------------------------------------*/

			while (true) {
				System.out.println("\nIn attesa di richieste...");
				
				// ricezione del datagramma
				try {
					packet.setData(buf);
					socket.receive(packet);
				}
				catch (IOException e) {
					System.err.println("Problemi nella ricezione del datagramma: "+ e.getMessage());
					e.printStackTrace();
					continue;
				}
				
				//parsing della richiesta
				try {
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					oiStream = new ObjectInputStream(biStream);
					richiesta = (MessaggioDiRegistrazione)oiStream.readObject();
				
					System.out.println("ricevuta richiesta di registrazione: " + richiesta.getNome() + ":" + richiesta.getIndirizzo());

					trovato = false;
					for(InetSocketAddress indirizzo: mappaUtenti.values() ) {
						if( indirizzo.equals( richiesta.getIndirizzo() ) ) {
							trovato = true;
							break;
						}
					}

					//registrazione
					if(!trovato) {
						System.out.println("registro " + richiesta.getNome() + ":" + richiesta.getIndirizzo() );
						mappaUtenti.put(richiesta.getNome(), (InetSocketAddress)richiesta.getIndirizzo());
					}	
					else{
						System.out.println(richiesta.getIndirizzo() + " gia presente");
					}					
				}
				catch (Exception e) {
					System.err.println("Problemi nella lettura del messaggio");
					e.printStackTrace();
					continue;
				}

				// preparazione e invio della risposta
				try {
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);

					if(!trovato) {
						doStream.writeInt(CHAT_PORT);
						data = boStream.toByteArray();
						packet.setData(data, 0, data.length);
						
						socket.send(packet);
					}
					else{
						doStream.writeInt(-1);
						data = boStream.toByteArray();
						packet.setData(data, 0, data.length);
						
						socket.send(packet);
					}
				}
				catch (IOException e) {
					System.err.println("Problemi nell'invio della risposta: "+ e.getMessage());
					e.printStackTrace();
					continue;
				}

			} // while

		}
		// qui catturo le eccezioni non catturate all'interno del while
		catch (Exception e) {
			e.printStackTrace();
		}

		System.out.println("Server: termino...");
		socket.close();
	}
}