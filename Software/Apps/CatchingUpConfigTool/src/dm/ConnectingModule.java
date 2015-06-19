package dm;

public class ConnectingModule { 
	private int id;
	private long idNetworkRecording;
	
	public ConnectingModule(long idNetworkRecording){
		this.idNetworkRecording = idNetworkRecording;
	}
	
	public int getId() {
	    return id;
	}

    public void setId(int id) {
        this.id = id;
    }
    
    public long getIdNetworkRecording() {
        return idNetworkRecording;
    }

	public void setIdNetworkRecording(long idNetworkRecording) {
	    this.idNetworkRecording = idNetworkRecording;
	}

}
