import java.io.Serializable;
import java.util.Date;

public class Noleggio implements Serializable{
    private String id;
    private Date data;
    private int giorni_noleggio;
    private String modello;
    private float costo_giornaliero;
    private String nome_immagine;

    public String getId() {
        return this.id;
    }
    public Date getData() {
        return this.data;
    }
    public int getGiorni_noleggio() {
        return this.giorni_noleggio;
    }
    public String getModello() {
        return this.modello;
    }
    public float getCosto_giornaliero() {
        return this.costo_giornaliero;
    }
    public String getNome_immagine() {
        return this.nome_immagine;
    }

    public void setId(String id) {
        this.id = id;
    }
    public void setData(Date data) {
        this.data = data;
    }
    public void setGiorni_noleggio(int giorni_noleggio) {
        this.giorni_noleggio = giorni_noleggio;
    }
    public void setModello(String modello) {
        this.modello = modello;
    }
    public void setCosto_giornaliero(float costo_giornaliero) {
        this.costo_giornaliero = costo_giornaliero;
    }
    public void setNome_immagine(String nome_immagine) {
        this.nome_immagine = nome_immagine;
    }

    @Override
    public String toString() {
        return "Id="+this.id+"; Data="+this.data+"; Giorni_noleggio="+this.giorni_noleggio+"; Modello="+this.modello
                    +"; Costo_giornaliero="+this.costo_giornaliero+"; Nome immagine="+this.nome_immagine;
    }
}