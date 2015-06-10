package persistence;

import java.util.ArrayList;
import java.util.List;

import ui.MessageBox;
import communication.Tools;
import dao.RoomDAO;
import dm.Room;

public class RoomDAOImpl implements RoomDAO {

	public static List<Room> _rooms = new ArrayList<Room>();
	public static List<Room> _freeRooms = new ArrayList<Room>();
	
	public static RoomDAOImpl _instance = new RoomDAOImpl();
	
	@Override
	public Room getRoom(int id) {
		for ( Room room : _rooms ){
			if ( room.getId() == id )
				return room;
		}
		return null;
	}

	public boolean createRoom ( Room room ){
		String req = "<type>create_rooms</type>"+ System.getProperty("line.separator") + "<room><name>" + room.getName() + "</name><description>" + room.getDescription() + "</description></room>";
		String res = communication.Server.sendData( req );
		String[] lines = res.split("["+System.getProperty("line.separator")+"]");
		
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("CREATE_ROOMS") != 0 )
		  	return false;
		
		String idRoom, name, line;
		
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"room");
			if ( line == "" )
				continue;
				
			idRoom 	= Tools.getValue( line ,"idRoom");
			name 	= Tools.getValue( line ,"roomname");
			
			if ( room.getName().compareTo(name) == 0 ){
				int id = Integer.parseInt(idRoom.trim());
				room.setId(id);
				_rooms.add(room);
				Tools.LOGGER_INFO("Create room OK");
				return true;
			}
		}
		new MessageBox("Erreur de cr�ation de la salle").setVisible(true);
		Tools.LOGGER_ERROR("Cannot create room");
		return false;
	}
	
	public boolean updateRoom( Room room , boolean updateName, boolean updateDescription){
		if (!updateName && !updateDescription )
			return true;
		
		String req = "<type>update_rooms</type>\n<room><id>" + room.getId() + "</id>";
		if ( updateName )
			req += "<name>" + room.getName() + "</name>";
		if ( updateDescription )
			req += "<description>" + room.getDescription() + " </description>";

		String res = communication.Server.sendData( req + "</room>\n" );
		
		String[] lines = res.split("["+System.getProperty("line.separator")+"]");
		
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("UPDATES_ROOMS") != 0 )
		  	return false;
		
		String idRoom, succes, line;
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"room");
			idRoom 	= Tools.getValue( line ,"idRoom");
			succes 	= Tools.getValue( line ,"succes");
			 
			if ( idRoom.trim().compareTo("" + room.getId()) == 0 ){
				if (succes.trim().compareTo("1") == 0){
					Tools.LOGGER_INFO("Update room OK");
					return true;
				}
			}
		}
		new MessageBox("Erreur de mise à jour de la salle").setVisible(true);
		Tools.LOGGER_ERROR("Cannot update room");
		return false;
	}
	
	@Override
	public List<Room> getRoomList() {
		if ( !_rooms.isEmpty() )
			return _rooms;
		
		_rooms = new ArrayList<Room>();
		_freeRooms = new ArrayList<Room>();
		
		String res = communication.Server.sendData("<type>need_rooms</type>");
		String[] lines = res.split("["+System.getProperty("line.separator")+"]");
		  
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("GET_ROOMS") != 0 )
		  	return null;
	  
		  Room room;
		  String id, name, description, idRecorder;
		  for ( int i = 1; i < lines.length; i++ ){
			 id 	= Tools.getValue( lines[i] ,"id");
			 name 	= Tools.getValue( lines[i] ,"roomname");
			 description = Tools.getValue( lines[i] ,"description");
			 idRecorder = Tools.getValue(lines[i], "idRec");
			 
			 if ( id == "" || name == "")
				 continue;
			 
			 int idRoom = Integer.parseInt(id.trim());
			 room = new Room(name,description);
			 room.setId(idRoom);
			 _rooms.add(room);
			 
			 if ( idRecorder.compareTo("0") == 0 )
				 _freeRooms.add(room);
		  }
		return _rooms;
	}
}
