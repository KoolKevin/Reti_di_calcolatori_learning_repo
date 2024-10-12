import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Server extends Remote {

	int elimina_prenotazione(String targa) throws RemoteException;

	Prenotazione[] visualizza_prenotazioni(String tipo) throws RemoteException;

}