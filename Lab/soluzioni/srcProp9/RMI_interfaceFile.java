// Interfaccia remota
import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RMI_interfaceFile extends Remote {
  public int conta_numero_linee(String file, String parola)
    throws RemoteException;

  public String[] lista_nomi_file_contenenti_parola_in_linea(
    String direttorio,
    String parola
  ) throws RemoteException;
}
