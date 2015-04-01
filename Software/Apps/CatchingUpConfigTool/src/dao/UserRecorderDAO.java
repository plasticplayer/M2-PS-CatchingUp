package dao;

import java.text.ParseException;
import java.util.List;

import dm.UserRecorder;

public interface UserRecorderDAO {
	/**
	 * Return UserRecorder where userRecorder.id = id
	 * @param id
	 * @return
	 */
	public UserRecorder getUserRecorder(int id);
	
	/**
	 * Return UserRecorder list
	 * @return
	 * @throws ParseException 
	 */
	public List<UserRecorder> getUserRecorderList() throws ParseException;

}
