
/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;

import java.util.List;

public interface RMI_interfaceFile extends Remote {

	List<String> lista_nomi_file_contenenti_parola_in_linea(String nomeDirettorio, String parolaCercata) throws RemoteException;

	int conta_numero_linee(String nomeFile, String parolaCercata) throws RemoteException;

}
