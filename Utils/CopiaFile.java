import java.io.BufferedReader;
import java.io.FileReader;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.io.FileWriter;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.nio.file.Files;
import java.nio.file.FileSystems;
import java.nio.file.Path;

// Produttore NON e' un filtro
public class CopiaFile{
	
	public static void copia(String source, String destination) {		
		PrintWriter fout = null;
		BufferedReader in = null;
		
		String riga;
		
		try {
			in = new BufferedReader(new FileReader(source));
		} catch(FileNotFoundException e){
			System.out.println("File non trovato");
			System.exit(1);
		}
		
		try {
			fout = new PrintWriter( new FileWriter(destination));
			
			while ((riga = in.readLine()) != null) { 
				fout.println(riga);
			}
			
			fout.close();
			in.close();
		} catch(IOException ex){
			System.out.println("Errore di input");
			System.exit(2);
		} 
		
	}
	
	public static void move(String source, String destination) {		
		PrintWriter fout = null;
		BufferedReader in = null;
		
		String riga;
		
		try {
			in = new BufferedReader(new FileReader(source));
		} catch(FileNotFoundException e){
			System.out.println("File non trovato");
			System.exit(1);
		}
		
		try {
			fout = new PrintWriter( new FileWriter(destination));
			
			while ((riga = in.readLine()) != null) { 
				fout.println(riga);
			}
			
			fout.close();
			in.close();
			
			//elimino il file temporaneo
			Path sourcePath = FileSystems.getDefault().getPath(source);
			Files.delete(sourcePath);
		} catch(IOException ex){
			System.out.println("Errore di input");
			System.exit(2);
		} 
		
	}
}

