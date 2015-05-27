package persistence;

import java.util.ArrayList;
import java.util.List;

import communication.Tools;
import dao.RecorderDAO;
import dm.Card;
import dm.ConnectingModule;
import dm.Recorder;
import dm.Recorder.RecorderStatus;
import dm.RecordingModule;
import dm.Room;
import dm.User;

public class RecorderDAOImpl implements RecorderDAO {

	private static List<Recorder> _recorders = new ArrayList<Recorder>();
	public static RecorderDAOImpl _instance = new RecorderDAOImpl();
	
	public boolean createRecorder ( Recorder rec ){
		String req = "<type>create_recorders</type><recorders><recorder><mac>" + rec.getRecordingModule().getAdressMac() + "</mac><idroom>" + rec.getRoom().getId() + "</idroom>"
				+ "</recorder></recorders>";
		
		String res = communication.Server.sendData(req);
		String[] lines = res.split(System.getProperty("line.separator"));
		  
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("CREATE_RECORDERS") != 0 )
		  	return false;

		String idrecorder, mac, line, idCModule, idRModule;
		
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"recorder");
			if ( line == "" )
				continue;
			
			idrecorder 	= Tools.getValue( line ,"idrecorder");
			idRModule 	= Tools.getValue( line ,"idRModule");
			idCModule 	= Tools.getValue( line ,"idCModule");
			mac 	= Tools.getValue( line ,"mac");
			
			if ( mac.compareTo(rec.getRecordingModule().getAdressMac() ) == 0 ){
				int idR = Integer.parseInt(idrecorder.trim());
				int idRMod = Integer.parseInt(idRModule.trim());
				int idCMod = Integer.parseInt(idCModule.trim());
				
				if ( idR == 0 || idRMod == 0 || idCMod == 0 )
					return false;
				rec.setId(idR);
				if ( rec.getConnectingModule() != null)
					rec.getConnectingModule().setId(idCMod);
				else{
					ConnectingModule c = new ConnectingModule(0);
					c.setId(idCMod);
					rec.setConnectingModule(c);
				}
				rec.getRecordingModule().setId(idRMod);
				return true;
			}
		}
		return false;
	}
	
	public boolean parringRecorder ( Recorder rec ){	
		if ( rec.getRecordingModule() == null || rec.getRecordingModule().getId() == 0 )
			return false;
		
		String req = "<type>parring_recorders</type><recorders><recorder><id>" + rec.getId() + "</id></recorder></recorders>";
		
		String res = communication.Server.sendData(req);
		String[] lines = res.split(System.getProperty("line.separator"));
		  
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("PARRING_RECORDERS") != 0 )
		  	return false;

		String idrecorder, state, line ;
		
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"recorder");
			if ( line == "" )
				continue;
			
			idrecorder = Tools.getValue( lines[i] ,"id");
			if ( idrecorder.trim().compareTo(""+rec.getId()) == 0 ){
				state = Tools.getValue( lines[i] ,"state");
				System.out.println("Parring state: " + state);
				return (state.trim().compareTo("1") == 0);
			}
			 
			
		}
		return false;
	}
	
	@Override
	public Recorder getRecorder(int id) {
		for ( Recorder rec : _recorders ){
			if ( rec.getId() == id )
				return rec;
		}
		return null;
	}

	@Override
	public List<Recorder> getRecorderList() {
		_recorders = new ArrayList<Recorder>();
		
		String res = communication.Server.sendData("<type>need_recorders</type>");
		String[] lines = res.split(System.getProperty("line.separator"));
		  
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("GET_RECORDERS") != 0 )
		  	return null;
	  
		ConnectingModule connectingModule = null;
		Room room = null;
		RecordingModule recordingModule = null;
		Recorder recorder;
		
		String id, status, mac, idCmodule, idRModule, idRoom, filesinqueue, isRecording;
		  
		  for ( int i = 1; i < lines.length; i++ ){
			 id 	= Tools.getValue( lines[i] ,"id");
			 status = Tools.getValue( lines[i] ,"status");
			 mac = Tools.getValue( lines[i] ,"mac");
			 idCmodule = Tools.getValue( lines[i] ,"idCModule");
			 idRModule = Tools.getValue( lines[i] ,"idRModule");
			 idRoom = Tools.getValue( lines[i] ,"roomid");
			 
			 if ( status.compareTo("CONNECTED") == 0 || status.compareTo("UNCONNECTED") == 0 ){
				 connectingModule = new ConnectingModule( 0 );
				 connectingModule.setId(Integer.parseInt(idCmodule.trim()));
				 
				 recordingModule = new RecordingModule(mac, "0");
				 recordingModule.setId(Integer.parseInt(idRModule.trim()));
				 room = RoomDAOImpl._instance.getRoom(Integer.parseInt(idRoom.trim()) );
				 
				 recorder = new Recorder(connectingModule, room, recordingModule);
				 recorder.setId(Integer.parseInt(id.trim()));
				 
				 if ( status.compareTo("CONNECTED") == 0 ){
					 filesinqueue = Tools.getValue( lines[i] ,"roomid");
					 isRecording = Tools.getValue( lines[i] ,"roomid");
					 recorder.setRecording( ( isRecording.compareTo("1") == 0) );
					 recorder.setFilesInQueue( Integer.parseInt(filesinqueue.trim() ));
					 recorder.setStatus(RecorderStatus.CONNECTED);
					 parringRecorder(recorder);
				 }
				 else 
					 recorder.setStatus(RecorderStatus.UNCONNECTED);
				 
				 
				 _recorders.add(recorder);
			 }
			 else if ( status.compareTo("UNASSOCIATED") == 0){
				 recordingModule = new RecordingModule(mac, "0");
				 recorder = new Recorder(null, null, recordingModule);
				 recorder.setStatus(RecorderStatus.UNASSOCIATED);
				 
				 _recorders.add(recorder);
			 }
		  }
		return _recorders;
	}

}
