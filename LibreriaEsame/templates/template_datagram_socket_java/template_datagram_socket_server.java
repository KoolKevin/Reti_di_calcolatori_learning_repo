import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.StringTokenizer;

public class DatagramServer {   //cambia il nome della classe

	// porta di default del server
	private static final int PORT = 4445;

	public static void main(String[] args) {
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[256];
		int port = -1;

		// controllo argomenti input: 0 oppure 1 argomento (porta)
		if ((args.length == 0)) {
			port = PORT;
		} 
		else if (args.length == 1) {
			try {
				port = Integer.parseInt(args[0]);
				// controllo che la porta sia nel range consentito 1024-65535
				if (port < 1024 || port > 65535) {
					System.out.println("Usage: java Server [serverPort>1024]");
					System.exit(1);
				}
			} catch (NumberFormatException e) {
				System.out.println("Usage: java Server [serverPort>1024]");
				System.exit(1);
			}
		} 
		else {
			System.out.println("Usage: java Server [serverPort>1024]");
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
			System.out.println("LineServer: avviato "+InetAddress.getLocalHost()+":"+port);
		}
		catch(UnknownHostException e ) {
			System.out.println("Problemi nel reperimento dell'indirizzo ip del server ");
			e.printStackTrace();
			System.exit(1);
		}

		//corpo del server
		try {
            ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;

            /*----------- QUESTE VARIABILI DIPENDONO DALL'ESERCIZIO RICHIESTO -------------*/
			String nomeFile = null;
			int numLinea = -1;
			String richiesta = null;

			StringTokenizer st = null;
			String linea = null;
			byte[] data = null;
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
					diStream = new DataInputStream(biStream);
					richiesta = diStream.readUTF();
					st = new StringTokenizer(richiesta);
                
					nomeFile = st.nextToken();
					numLinea = Integer.parseInt(st.nextToken());
					System.out.println("Richiesta linea " + numLinea + " del file " + nomeFile);
				}
				catch (Exception e) {
					System.err.println("Problemi nella lettura della richiesta: " + nomeFile + " " + numLinea);
					e.printStackTrace();
					continue;
				}

				// preparazione e invio della risposta
				try {
					/*----------- LOGICA DI BUISNESS DEL SERVER -------------*/
					linea = LineUtility.getLine(nomeFile, numLinea);
					/*-------------------------------------------------------*/

					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					doStream.writeUTF(linea);
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
		// qui catturo le eccezioni non catturate all'interno del while
		catch (Exception e) {
			e.printStackTrace();
		}

		System.out.println("Server: termino...");
		socket.close();
	}
}