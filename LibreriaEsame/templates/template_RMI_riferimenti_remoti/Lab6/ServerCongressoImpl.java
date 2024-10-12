import java.rmi.Naming;
import java.rmi.RMISecurityManager;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class ServerCongressoImpl extends UnicastRemoteObject implements ServerCongresso {



  /* IMPLEMENTAZIONE NORMALE DEL SERVIZIO */



  static Programma prog[];

  // Costruttore
  public ServerCongressoImpl() throws RemoteException {
    super();
  }

  // Richiede una prenotazione
  public int registrazione(int giorno, String sessione, String speaker)
      throws RemoteException {
    int numSess = -1;
    System.out.println("Server RMI: richiesta registrazione con parametri");
    System.out.println("giorno   = " + giorno);
    System.out.println("sessione = " + sessione);
    System.out.println("speaker  = " + speaker);

    if (sessione.startsWith("S")) {
      try {
        numSess = Integer.parseInt(sessione.substring(1)) - 1;
      } catch (NumberFormatException e) {
      }
    }

    // Se i dati sono sbagliati significa che sono stati trasmessi male e quindi
    // solleva una eccezione
    if (numSess == -1)
      throw new RemoteException();
    if (giorno < 1 || giorno > 3)
      throw new RemoteException();

    return prog[giorno - 1].registra(numSess, speaker);
  }

  // Ritorno il campo
  public Programma programma(int giorno) throws RemoteException {
    System.out.println("Server RMI: richiesto programma del giorno " + giorno);
    return prog[giorno - 1];
  }




  /* ------------------------- */



  // Avvio del Server RMI
  public static void main(String[] args) {

    //inizializzazione struttura dati
    prog = new Programma[3];
    for (int i = 0; i < 3; i++) {
      prog[i] = new Programma();
    }

    int registryRemotoPort = 1099;
    String registryRemotoName = "RegistryRemoto";   //NB: mi registro su un registry remoto non sul mio rmiregistry
    String serviceName = "ServerCongresso";

    // Controllo argomenti
    if (args.length != 1 && args.length != 2) {
      System.out.println("Sintassi: ServerCongressoImpl NomeHostRegistryRemoto [registryPort], registryPort intero");
      System.exit(1);
    }

    String registryRemotoHost = args[0];   //NB: mi registro su un registry remoto non sul mio rmiregistry

    if (args.length == 2) {
      try {
        registryRemotoPort = Integer.parseInt(args[1]);
      } catch (Exception e) {
        System.out.println("Sintassi: ServerCongressoImpl NomeHostRegistryRemoto [registryPort], registryPort intero");
        System.exit(2);
      }
    } 


    // Registrazione del servizio RMI
    String completeRemoteRegistryName = "//" + registryRemotoHost + ":" + registryRemotoPort + "/" + registryRemotoName;

    try {
      //recupero riferimento al registry remoto
      RegistryRemotoTagServer registryRemoto = (RegistryRemotoTagServer) Naming.lookup(completeRemoteRegistryName);
      //mi registro sul registry remoto NON sul mio RMI_registry
      ServerCongressoImpl serverRMI = new ServerCongressoImpl();
      registryRemoto.aggiungi(serviceName, serverRMI);
      System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");

      ServerCongressoImpl serverRMI2 = new ServerCongressoImpl();
      registryRemoto.aggiungi(serviceName+"_migliore", serverRMI2);
      System.out.println("Server RMI: Servizio \"" + serviceName + "_migliore\" registrato");
      
      boolean ok;
      //associo uno o più tag
      ok=registryRemoto.associaTag(serviceName, "congresso");
      System.out.println("Server RMI: Servizio \"" + serviceName + "\" associato al tag congresso? "+ok);
      ok=registryRemoto.associaTag(serviceName, "bello");
      System.out.println("Server RMI: Servizio \"" + serviceName + "\" associato al tag bello? "+ok);
      //provo anche ad associarmi ad un tag che non va bene
      ok=registryRemoto.associaTag(serviceName, "veloce");
      System.out.println("Server RMI: Servizio \"" + serviceName + "\" associato al tag veloce? "+ok);
    
      ok=registryRemoto.associaTag(serviceName+"_migliore", "congresso");
      System.out.println("Server RMI: Servizio \"" + serviceName + "_migliore\" associato al tag congresso? "+ok);
    } catch (Exception e) {
      System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
      e.printStackTrace();
      System.exit(1);
    }
  }
}
