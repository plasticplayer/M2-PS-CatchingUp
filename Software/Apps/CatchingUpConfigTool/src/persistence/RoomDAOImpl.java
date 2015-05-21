package persistence;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import communication.Tools;

import dao.RoomDAO;
import dm.Room;
import dm.UserRecorder;

public class RoomDAOImpl implements RoomDAO {

	@Override
	public Room getRoom(int id) {
		// TODO Auto-generated method stub
		return null;
	}

	public boolean createRoom ( Room room ){
		String req = "<type>create_rooms</type>\n<room><namecl> " + room.getName() + " </name><description> " + room.getDescription() + "</description></room>";
		String res = communication.Server.sendData( req );
		String[] lines = res.split(System.getProperty("line.separator"));
		
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("CREATE_ROOMS") != 0 )
		  	return false;
		
		String idRoom, name, line;
		
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"room");
			if ( line == "" )
				continue;
				
			idRoom 	= Tools.getValue( line ,"id");
			name 	= Tools.getValue( line ,"roomname");
			
			if ( room.getName().compareTo(name) == 0 ){
				int id = Integer.parseInt(idRoom.trim());
				room.setId(id);
				return true;
			}
		}
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
		
		String[] lines = res.split(System.getProperty("line.separator"));
		
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("UPDATES_ROOMS") != 0 )
		  	return false;
		
		String idRoom, succes, line;
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"room");
			idRoom 	= Tools.getValue( line ,"idRoom");
			succes 	= Tools.getValue( line ,"succes");
			 
			if ( idRoom.trim().compareTo("" + room.getId()) != 0 ){
				return (succes.trim().compareTo("1") == 0);
			}
		}
		return false;
	}
	
	@Override
	public List<Room> getRoomList() {
		List<Room> rooms = new ArrayList<Room>();
		
		String res = communication.Server.sendData("<type>need_rooms</type>");
		String[] lines = res.split(System.getProperty("line.separator"));
		  
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("GET_ROOMS") != 0 )
		  	return null;
	  
		  Room room;
		  String id, name, description;
		  for ( int i = 1; i < lines.length; i++ ){
			 id 	= Tools.getValue( lines[i] ,"id");
			 name 	= Tools.getValue( lines[i] ,"roomname");
			 description = Tools.getValue( lines[i] ,"description");
			 
			 if ( id == "" || name == "")
				 continue;
			 
			 int idRoom = Integer.parseInt(id.trim());
			 room = new Room(name,description);
			 room.setId(idRoom);
			 rooms.add(room);
		  }
		return rooms;
	}
}
