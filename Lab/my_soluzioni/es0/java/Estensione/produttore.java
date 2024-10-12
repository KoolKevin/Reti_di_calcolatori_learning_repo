import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.StringTokenizer;

// Produttore NON e' un filtro
public class produttore{
	public static void main(String[] args) {		
		PrintWriter[] fout = new PrintWriter[args.length];
		BufferedReader in = null;
		String riga;
		StringTokenizer tokenizer;
		int i, indiceFile;
		
		if (args.length < 1){
			System.out.println("Utilizzo: produttore <inputFilename>");
			System.exit(0);
		}
		
		in = new BufferedReader(new InputStreamReader(System.in));
		
		try {
			//apro tutti i file
			for(i=0; i<args.length; i++) {
				fout[i] = new PrintWriter( new FileWriter(args[i]) );
			}
			
			System.out.println("scrivi delle righe nel seguente formato -> numFile:testo; [EOF per terminare]");
			//scrivo sul file giusto
			while ((riga = in.readLine()) != null) { 
				tokenizer = new StringTokenizer(riga, ":");
				try {
					indiceFile = Integer.parseInt(tokenizer.nextToken());
					if(indiceFile > args.length || indiceFile < 1) {
						throw new NumberFormatException();
					}
					
					riga = tokenizer.nextToken();

					fout[indiceFile-1].println(riga);
				} 
				catch(NumberFormatException ex) {
					System.out.println("Errore indice riga");
				}
			}
			
			//chiudo tutti i file
			for(i=0; i<args.length; i++) {
				fout[i].close();
			}
		} catch(IOException ex){
			System.out.println("Errore di input");
			System.exit(2);
		} 
		
	
	}
}

