package dm;

public class Recorder {
	private int id;
	private ConnectingModule connectingModule;
	private Room room;
	private RecordingModule recordingModule;
	
	public Recorder(ConnectingModule connectingModule,Room room,RecordingModule recordingModule){
		this.connectingModule = connectingModule;
		this.room = room;
		this.recordingModule = recordingModule;		
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
	
    

}
