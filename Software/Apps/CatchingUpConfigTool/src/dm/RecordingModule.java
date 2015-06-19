package dm;

public class RecordingModule {//TODO: vérifier que recordingConfiguration est bien un type String
	private int id;
	private String adressMac;
	private String recordingConfiguration;
	private long idNetwork;
	
	public RecordingModule(String adressMac,String recordingConfiguration,long idNetwork){
		this.adressMac = adressMac;
		this.recordingConfiguration = recordingConfiguration;
		this.idNetwork = idNetwork;
	}
	
	public RecordingModule(String adressMac,String recordingConfiguration){
		this.adressMac=adressMac;
		this.recordingConfiguration = recordingConfiguration;
	}
	
	 public int getId() {
	    return id;
	 }

    public void setId(int id) {
        this.id = id;
    }
    
    public String getAdressMac() {
	    return adressMac;
	}

    public void setAdressMac(String adressMac) {
        this.adressMac = adressMac;
    }
    
    public String getRecordingConfiguration() {
	    return recordingConfiguration;
	}

    public void setRecordingConfiguration(String recordingConfiguration) {
        this.recordingConfiguration = recordingConfiguration;
    }
	
    public long getIdNetwork() {
	    return idNetwork;
	 }

    public void setIdNetwork(long idNetwork) {
        this.idNetwork = idNetwork;
    }

}
