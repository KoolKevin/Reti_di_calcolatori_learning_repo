import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.util.Map;

public class ChatServer extends Thread {

	private DatagramSocket socket = null;
	private DatagramPacket packet = null;
	private byte[] buf = new byte[256];

	private int port = -1;
	private Map<String, InetSocketAddress> mappaUtenti = null;  //TODO: in questa struttura dati dovrei mantenere anche la writing port del client

	public ChatServer(Map<String, InetSocketAddress> mappaUtenti, int p) {
		this.mappaUtenti = mappaUtenti;
		this.port = p;
	}

	public void run() {
		try {
			//apertura Socket
			socket = new DatagramSocket(port);
			packet = new DatagramPacket(buf, buf.length);
			System.out.println("ChatServer avviato con socket port: " + socket.getLocalPort()); 
		} catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.exit(1);
		}

		try {
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
			String messaggio = null;

			while (true) {
				System.out.println("\nChatServer in attesa di richieste...");

				// ricezione di un messaggio
				try {
					packet.setData(buf);
					socket.receive(packet);

				} catch (IOException e) {
					System.err.println("Problemi nella ricezione del datagramma: "+ e.getMessage());
					e.printStackTrace();
					continue;
					// il server continua a fornire il servizio ricominciando dall'inizio
					// del ciclo
				}

				// Forward del messaggio agli altri 
				try {
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);
					messaggio = diStream.readUTF();
						
					System.out.println("messaggio ricevuto da " + packet.getSocketAddress() + ": " + messaggio);
                    System.out.println("evito la porta " + (packet.getPort()+1) + " di " + packet.getAddress());
                    

                    for(InetSocketAddress indirizzo : mappaUtenti.values()) {
                        //TODO: qua la listening port del client è diversa rispetta alla writing port, per adesso assumo che siano distanti uno
                        //      ma dovrei comunicare anche qual'è la porta su cui sto ascoltando
						if( !( indirizzo.getAddress().equals(packet.getAddress()) && indirizzo.getPort()==(packet.getPort()+1) ) ) {
                            System.out.println("indirizzo mappa: " +indirizzo.getAddress() + " indirizzo pacchetto: " + packet.getAddress() +" uguali?"+  indirizzo.getAddress().equals(packet.getAddress()) );
							packet.setSocketAddress(indirizzo);
							socket.send(packet);
						}	
                        else{
                            System.out.println("non faccio il forward a: " + indirizzo);
                        }
					}
				}
				catch (IOException e) {
					System.out.println("Problemi nella lettura della risposta: ");
					e.printStackTrace();
				}


			} // while

		}
		// qui catturo le eccezioni non catturate all'interno del while
		// in seguito alle quali il server termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("SwapServer: termino...");
		socket.close();
	}
}