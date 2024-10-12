import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class ListeningClient extends Thread{
    private int port;

    public ListeningClient(int port) {
        this.port = port;
    }

    public void run(){
        DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[256];

        // creazione della socket datagram, settaggio timeout di 30s, e creazione datagram packet
		try {
			socket = new DatagramSocket(port);
			//socket.setSoTimeout(30000);
			packet = new DatagramPacket(buf, buf.length, InetAddress.getLocalHost(), port);
			System.out.println("ListeningClient: avviato");
			System.out.println("Creata la socket: " + socket);
		} catch (Exception e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.out.println("Client: interrompo...");
            socket.close();
			System.exit(1);
		}

		ByteArrayInputStream biStream = null;
		DataInputStream diStream = null;
        String messaggio = null;

        while(true) {
            //ricezione della risposta
            try {
                packet.setData(buf);
                socket.receive(packet);
            } catch (IOException e) {
                System.out.println("Problemi nella ricezione del messaggio: ");
                e.printStackTrace();
                continue;
            }
            //interpretazione della risposta
            try {
                biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
                diStream = new DataInputStream(biStream);
                messaggio = diStream.readUTF();
                System.out.println(messaggio);
            } catch (IOException e) {
                System.out.println("Problemi nella lettura della risposta: ");
                e.printStackTrace();
                continue;
            }
        }

        
    }
}
