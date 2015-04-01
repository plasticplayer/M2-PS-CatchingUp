package dao;

import java.util.List;

import dm.User;

public interface UserDAO {
	
	 /**
     * Create a new user
     * @param user
     * @return 1 in success; -1 User not found; -2 otherwise
     */
    public int createUser(User user);

    /**
     * Delete an user
     * 
     * @param user
     * @return 1 in success; -1 user not found; -2 otherwise
     */
    public int deleteUser(User user);

    /**
     * Return User where user.id = id
     * 
     * @param id
     * @return User if found; null otherwise
     */
    public User getUser(int id);

    /**
	 * Return users'list where user.name = name
	 * @param name
	 * @return
	 */
	public List<User> searchUserByName(String lastName);
	
	/**
	 * Return User where user.mail = mail
	 * @param login
	 * @return User if exist; null otherwise
	 */
	public User searchUserByMail(String mail);

	public User searchUserByLogin(String mail, String password);
	

}
