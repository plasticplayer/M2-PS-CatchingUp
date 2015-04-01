package dm;

public class ConnectingModule { 
	private int id;
	private int idNetworkRecording;
	
	public ConnectingModule(int idNetworkRecording){
		this.idNetworkRecording = idNetworkRecording;
	}
	
	public int getId() {
	    return id;
	}

    public void setId(int id) {
        this.id = id;
    }
    
    public int getIdNetworkRecording() {
        return idNetworkRecording;
    }

	public void setIdNetworkRecording(int idNetworkRecording) {
	    this.idNetworkRecording = idNetworkRecording;
	}

}
