import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.StringTokenizer;


public class RowSwapServer extends Thread {
	private String nomeFile;
	private int porta;
	
	public RowSwapServer(String nomeFile, String porta) {
		this.nomeFile=nomeFile;
		this.porta=Integer.parseInt(porta);
	}
	
	public RowSwapServer(String nomeFile, int porta) {
		this.nomeFile=nomeFile;
		this.porta=porta;
	}

	public void run() {	
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		byte[] buf = new byte[256];
		int i;
		//apro il file da leggere
		System.out.println("Sono il RowSwapServer " + this.getId() + ": " + this.nomeFile + " " + this.porta);
		
		try {
			socket = new DatagramSocket(this.porta);
			packet = new DatagramPacket(buf, buf.length);
			System.out.println("Creata la socket: " + socket);
		}
		catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.exit(1);
		}

		try {
			int indice1 = -1;
			int indice2 = -1;
			int contatoreRiga = 0;
			String riga = null;
			String linea1 = null;
			String linea2 = null;
			String indici = null;
			String esito = null;
			BufferedReader inFile = null;
			PrintWriter outTemp = null;
			
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;
			StringTokenizer st = null;
			byte[] data = null;
			
			
			while (true) {
				// ricezione degli indici di riga
				try {
					packet.setData(buf);
					socket.receive(packet);
				}
				catch (IOException e) {
					System.err.println("Problemi nella ricezione del datagramma: "+ e.getMessage());
					e.printStackTrace();
					continue;
				}
				
				//ho ricevuto i due indiciDiRiga
				try {
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);
					indici = diStream.readUTF();
					
					st = new StringTokenizer(indici);
					indice1 = Integer.parseInt( st.nextToken() );
					indice2 = Integer.parseInt( st.nextToken() );
					
					if(indice1 < 1 || indice2 < 1 ) {
						esito = "Indici di riga non validi, devono essere entrambi >= 1. Rimanda";
						
						boStream = new ByteArrayOutputStream();
						doStream = new DataOutputStream(boStream);
						doStream.writeUTF(esito);
						//doStream.writeInt(-1);  //se voglio mandare come esito un codice di errore numerico
						data = boStream.toByteArray();
						packet.setData(data, 0, data.length);
						socket.send(packet);

						continue;
					}

					System.out.println("Richiesto scambio delle righe "+ indice1 + " e "+ indice2 +" del file:" + nomeFile);
				}
				catch (Exception e) {
					esito = "problema nella lettura della rischiesta: "+ indici;
					
					try {
						boStream = new ByteArrayOutputStream();
						doStream = new DataOutputStream(boStream);
						doStream.writeUTF(esito);
						data = boStream.toByteArray();
						packet.setData(data, 0, data.length);
						socket.send(packet);
					}
					catch (IOException e2) {
						System.err.println("Problemi nell'invio dell'esito: "+ e2.getMessage());
						e2.printStackTrace();
						inFile.close();
					}
					
					System.err.println("Problemi nella lettura della richiesta: "+ indici );
					e.printStackTrace();
					continue;
				}

				// scambio delle righe e invio dell'esito
				try {
//					linea1 = LineUtility.getLine(this.nomeFile, indice1);	//meglio fare inline
//					linea2 = LineUtility.getLine(this.nomeFile, indice2);
					
					inFile = new BufferedReader( new FileReader(this.nomeFile) );
					
					contatoreRiga=0;
					
					while( (riga=inFile.readLine()) != null ) {
						contatoreRiga++;
						if(contatoreRiga==indice1) {
							System.out.println("linea1: "+riga );
							linea1 = riga;
						}
						
						if(contatoreRiga==indice2) {
							System.out.println("linea2: "+riga );
							linea2 = riga;
						}
					}
					
					if( linea1==null || linea2==null ) {
						esito = "linea da scambiare non trovate, il file ha "+ contatoreRiga +" righe";
						
						System.out.println(esito);
						
						boStream = new ByteArrayOutputStream();
						doStream = new DataOutputStream(boStream);
						doStream.writeUTF(esito);
						data = boStream.toByteArray();
						packet.setData(data, 0, data.length);
						socket.send(packet);
						
						inFile.close();
						continue;
					}
					
					//chiudo e riapro per resettare l'i/o pointer
					inFile.close();
					inFile = new BufferedReader( new FileReader(this.nomeFile) );
					//creo il file temporaneo
					outTemp = new PrintWriter( new FileWriter("temp."+this.getId()) );
					
					contatoreRiga=0;
					while( (riga=inFile.readLine()) != null ) {
						contatoreRiga++;
						
						if(contatoreRiga==indice1) {
							outTemp.println(linea2);
							//System.out.println(linea2);
						}
						else if(contatoreRiga==indice2) {
							outTemp.println(linea1);
							//System.out.println(linea1);
						}
						else {
							outTemp.println(riga);
							//System.out.println(riga);
						}
						
					}
					
					esito = "successo, scambio delle righe riuscito";
					
					//rinomino e sostituisco
					File temp = new File("temp."+this.getId());
					File file = new File(this.nomeFile);
					temp.renameTo(file);
					
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					doStream.writeUTF(esito);
					data = boStream.toByteArray();
					packet.setData(data, 0, data.length);
					socket.send(packet);
					
					inFile.close();
					outTemp.close();
				}
				catch (IOException e) {
					esito = "scambio delle righe non riuscito, riprova";
					
					boStream = new ByteArrayOutputStream();
					doStream = new DataOutputStream(boStream);
					doStream.writeUTF(esito);
					data = boStream.toByteArray();
					packet.setData(data, 0, data.length);
					socket.send(packet);
					
					inFile.close();
					outTemp.close();
					System.err.println("Problemi nell'invio della risposta: "+ e.getMessage());
					e.printStackTrace();
					continue;
				}

			} // while

		}
		// qui catturo le eccezioni non catturate all'interno del while
		// in seguito alle quali il server termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("DiscoveryServer: termino...");
		socket.close();
	}
	
}
