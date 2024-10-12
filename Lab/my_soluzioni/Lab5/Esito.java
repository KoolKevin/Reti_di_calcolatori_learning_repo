
import java.io.Serializable;

public class Esito implements Serializable {

	private String nomeFile;
	private int righeFile;

	public Esito() {
		
	}

	public Esito(String nomeFile, int righeFile) {
		this.nomeFile=nomeFile;
		this.righeFile=righeFile;
	}

	public String getNomeFile() {
		return this.nomeFile;
	}
	
	public void setNomeFile(String nomeFile) {
		this.nomeFile=nomeFile;
	}

	public int getRigheFile() {
		return this.righeFile;
	}
	
	public void setRigheFile(int righeFile) {
		this.righeFile=righeFile;
	}

	@Override
	public String toString() {
		return "Esito [nomeFile=" + nomeFile + ", righeFile=" + righeFile + "]";
	}
}
