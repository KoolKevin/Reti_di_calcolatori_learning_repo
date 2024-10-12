import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.List;

public interface Server extends Remote {

	public List<String> lista_file(String dir_name) throws RemoteException;
	
	public int numera_righe(String fileName) throws RemoteException;
}