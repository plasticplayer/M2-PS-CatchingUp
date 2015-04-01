package persistence;

import java.util.ArrayList;
import java.util.List;

import dao.RecorderDAO;
import dm.ConnectingModule;
import dm.Recorder;
import dm.RecordingModule;
import dm.Room;

public class RecorderDAOImpl implements RecorderDAO {

	@Override
	public Recorder getRecorder(int id) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public List<Recorder> getRecorderList() {
		List<Recorder> recorders = new ArrayList<Recorder>();
			
		ConnectingModule connectingModule = new ConnectingModule(3);
		Room room = new Room("A498","spacieuse, équipée d'ordinateurs");
		RecordingModule recordingModule = new RecordingModule("B6:98:56:E7:AF:D8","bla bla configuration",8);
		Recorder recorder = new Recorder(connectingModule,room,recordingModule);
		recorders.add(recorder);
		recorder = null;
		connectingModule = null;
		connectingModule = new ConnectingModule(7);
		room = null;
		room = new Room("A470","Spécialement pour les TP d'informatique embarquée");
		recordingModule = null;
		recordingModule = new RecordingModule("F6:D2:27:6A:6C:D8","bla bla bla bla configuration",6);
		recorder = new Recorder(connectingModule,room,recordingModule);
		recorders.add(recorder);
		return recorders;
	}

}
