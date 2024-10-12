

// Consumatore e' un filtro
public class consumatore {
	public static void main(String[] args) {
		String caratteriDaEliminare;
		int i;
		
		FiglioConsumatore[] figli = new FiglioConsumatore[args.length - 1];
		
		if (args.length < 2){
			System.out.println("Utilizzo: consumatore <caratteriDaEliminare> <inputFilename>");
			System.exit(0);
		}
		
		caratteriDaEliminare=args[0];
		
		
		for(i=0; i<args.length-1; i++) {
			figli[i] = new FiglioConsumatore(args[i+1], caratteriDaEliminare);
		}
	
		for(i=0; i<args.length-1; i++) {
			figli[i].start();
		}

		for(i=0; i<args.length-1; i++) {
			try {
				figli[i].join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			} 
		}
			
	}
}
