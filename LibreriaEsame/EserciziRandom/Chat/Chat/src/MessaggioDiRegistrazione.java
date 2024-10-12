import java.io.Serializable;
import java.net.SocketAddress;

public class MessaggioDiRegistrazione implements Serializable {
    static final long serialVersionUID = 42L;
    
    private SocketAddress indirizzo = null;
    private String nome = null;

    public MessaggioDiRegistrazione(SocketAddress indirizzo, String nome) {
        this.indirizzo = indirizzo;
        this.nome = nome;
    }

    public SocketAddress getIndirizzo() {
        return this.indirizzo;
    }

    public String getNome() {
        return this.nome;
    }
}