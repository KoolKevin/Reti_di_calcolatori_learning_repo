import java.io.Serializable;

public class Prenotazione implements Serializable{
    private String targa;
    private String patente;
    private String tipo;
    private String directoryImg;

    public String getTarga() {
        return this.targa;
    }

    public String getPatente() {
        return this.patente;
    }

    public String getTipo() {
        return this.tipo;
    }

    public String getDirectoryImg() {
        return this.directoryImg;
    }

    public void setTarga(String targa) {
        this.targa = targa;
    }

    public void setPatente(String patente) {
        this.patente = patente;
    }

    public void setTipo(String tipo) {
        this.tipo = tipo;
    }

    public void setDirectoryImg(String directoryImg) {
        this.directoryImg = directoryImg;
    }
}