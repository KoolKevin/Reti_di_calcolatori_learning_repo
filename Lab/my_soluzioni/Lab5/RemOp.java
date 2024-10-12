
/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RemOp extends Remote {

	int conta_righe(String nomeFile, int soglia) throws RemoteException;

	Esito elimina_riga(String nomeFile, int indiceRiga) throws RemoteException;

}
