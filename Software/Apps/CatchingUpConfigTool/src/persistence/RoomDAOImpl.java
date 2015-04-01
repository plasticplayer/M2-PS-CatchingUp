package persistence;

import java.util.ArrayList;
import java.util.List;

import dao.RoomDAO;
import dm.Room;

public class RoomDAOImpl implements RoomDAO {

	@Override
	public Room getRoom(int id) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public List<Room> getRoomList() {
		List<Room> rooms = new ArrayList<Room>();		
		//String name, String description
		Room room = new Room("A498","spacieuse, équipée d'ordinateurs");
		rooms.add(room);
		room = null;
		room = new Room("A470","Spécialement pour les TP d'informatique embarquée");
		rooms.add(room);
		return rooms;
	}

}
