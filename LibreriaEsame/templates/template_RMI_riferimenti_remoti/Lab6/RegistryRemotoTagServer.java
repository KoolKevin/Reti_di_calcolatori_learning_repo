
/**
 *  Interfaccia remota Registry per il Server. Estende quella per il Client.
 *  	Aggiungi = aggiunge un server remoto, dato 'nomeLogico', 'Riferimento'.
 *  	RestituisciTutti = tutte le coppie [nomelogico][riferimento] registrate.
 *  	EliminaPrimo/Tutti = elimina prima/tutte entry corrispondente al 'nomeLogico'.
 */

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RegistryRemotoTagServer extends RegistryRemotoTagClient, RegistryRemotoServer {
	
	public boolean associaTag(String nome_logico_server, String tag) throws RemoteException;
}
