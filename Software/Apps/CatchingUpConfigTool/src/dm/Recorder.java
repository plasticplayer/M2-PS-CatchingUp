package dm;

public class Recorder {
	public enum RecorderStatus { CONNECTED, UNCONNECTED, UNASSOCIATED };
	
	private int id;
	private ConnectingModule connectingModule;
	private Room room;
	private RecordingModule recordingModule;
	private RecorderStatus status;
	private boolean isRecording = false;
	private long filesInQueue = 0;
	
	public Recorder(ConnectingModule connectingModule,Room room,RecordingModule recordingModule){
		this.connectingModule = connectingModule;
		this.room = room;
		this.recordingModule = recordingModule;		
	}
	
	@Override
	public String toString() {
		switch( status ){
		case CONNECTED:
			if ( isRecording )
				return "En enregistrement";
			else if ( filesInQueue != 0 )
				return "Envoie fichier (" + filesInQueue + ")";
			return "Connecté";
			
		case UNCONNECTED:
			return "Non connecté";
		case UNASSOCIATED:
			return "Non associé";
		}
		return "";
	};
	
	public void setStatus ( RecorderStatus state ){
		this.status = state;
	}
	
	public RecorderStatus getStatus ( ){
		return this.status;
	}
	
	public int getId() {
	    return id;
	}

    public void setId(int id) {
        this.id = id;
    }
    
    public Room getRoom() {
	    return room;
	}

    public void setRoom(Room room) {
        this.room = room;
    }
    
    public ConnectingModule getConnectingModule() {
	    return connectingModule;
	}

    public void setConnectingModule(ConnectingModule ConnectingModule) {
        this.connectingModule = ConnectingModule;
    }
    
    public RecordingModule getRecordingModule() {
	    return recordingModule;
	}

    public void setRecordingModule(RecordingModule recordingModule) {
        this.recordingModule = recordingModule;
    }


	public boolean isRecording() {
		return isRecording;
	}

	public void setRecording(boolean isRecording) {
		this.isRecording = isRecording;
	}

	public long getFilesInQueue() {
		return filesInQueue;
	}

	public void setFilesInQueue(long filesInQueue) {
		this.filesInQueue = filesInQueue;
	}
	
    

}
