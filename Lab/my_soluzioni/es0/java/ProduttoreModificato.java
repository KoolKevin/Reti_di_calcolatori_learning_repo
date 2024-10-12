import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;

// Produttore NON e' un filtro
public class ProduttoreModificato {
	public static void main(String[] args) {		
		FileWriter fout;
		BufferedReader in = null;
		char ch;
		int x;
		
		if (args.length < 1){
			System.out.println("Utilizzo: produttore <inputFilename>");
			System.exit(0);
		}
		
		in = new BufferedReader(new InputStreamReader(System.in));
		
		try {
			fout = new FileWriter(args[0]);
			
			while ((x = in.read()) >= 0) { 
				ch = (char) x;
				fout.write(ch);
			}
			
			fout.close();
		} catch(IOException ex){
			System.out.println("Errore di input");
			System.exit(2);
		}
		
	
	}
}

