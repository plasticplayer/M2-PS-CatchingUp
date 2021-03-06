package dao;

import java.util.List;

import dm.Room;


public interface RoomDAO {
	/**
	 * Return Room where Room.id = id
	 * @param id
	 * @return
	 */
	public Room getRoom(int id);
	
	/**
	 * Return Room list
	 * @return
	 */
	public List<Room> getRoomList();
	
	public boolean createRoom ( Room room );
	
	public boolean updateRoom( Room room , boolean updateName, boolean updateDescription);
}
