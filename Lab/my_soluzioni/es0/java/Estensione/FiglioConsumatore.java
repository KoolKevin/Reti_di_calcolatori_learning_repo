import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.FileWriter;
import java.io.File;

public class FiglioConsumatore extends Thread {
	private String nomeFile, caratteriDaEliminare;
	private int x;
	private char ch;
	private FileReader r = null;
	private FileWriter temp = null;
	
	public FiglioConsumatore(String nomeFile, String caratteriDaEliminare) {
		this.nomeFile=nomeFile;
		this.caratteriDaEliminare=caratteriDaEliminare;
	}

	public void run() {	
		//apro il file da leggere
		try {
			r = new FileReader(this.nomeFile);
		} catch(FileNotFoundException e){
			System.out.println("File non trovato");
			System.exit(1);
		}
		
		//elimino i caratteri da filtrare
		try {
			temp = new FileWriter("temp."+this.threadId());
			
			while ((x = r.read()) >= 0) { 
				ch = (char) x;
				
				if(caratteriDaEliminare.indexOf(ch) == -1) {
					temp.write(ch);
				}
			}
			r.close();
			temp.close();
			
			
			//rinomino e sostituisco
			File temp = new File("temp."+this.threadId());
			File file = new File(this.nomeFile);
			temp.renameTo(file);
		} catch(IOException ex){
			System.out.println("Errore di input");
			System.exit(2);
		}

	}
	
}
