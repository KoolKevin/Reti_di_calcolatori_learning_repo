import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;

// Consumatore e' un filtro
public class ConsumatoreModificato {
	public static void main(String[] args) {
		String caratteriDaEliminare;
		char ch;
		int x;
		
		if (args.length < 1 || args.length > 2){
			System.out.println("Utilizzo: consumatore <caratteriDaEliminare> <inputFilename>");
			System.exit(0);
		}
		
		caratteriDaEliminare=args[0];
		
		if(args.length == 2) {
			//apro il file
			FileReader r = null;
			try {
				r = new FileReader(args[1]);
			} catch(FileNotFoundException e){
				System.out.println("File non trovato");
				System.exit(1);
			}
			//elimino i caratteri da filtrare
			try {

				while ((x = r.read()) >= 0) { 
					ch = (char) x;
					
					if(caratteriDaEliminare.indexOf(ch) == -1) {
						System.out.print(ch);
					}
				}
				r.close();
			} catch(IOException ex){
				System.out.println("Errore di input");
				System.exit(2);
			}
	    }
	    else if(args.length == 1) {
	    	BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
	    	
	    	//elimino i caratteri da filtrare
	    	try {

				while ((x = in.read()) >= 0) { 
					ch = (char) x;
					
					if(caratteriDaEliminare.indexOf(ch) == -1) {
						System.out.print(ch);
					}
				}
			} catch(IOException ex){
				System.out.println("Errore di input");
				System.exit(2);
			}
	    }
		
		
		
}}
