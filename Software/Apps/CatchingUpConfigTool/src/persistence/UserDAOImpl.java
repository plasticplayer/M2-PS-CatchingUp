package persistence;

import java.util.List;

import dao.UserDAO;
import dm.User;

public class UserDAOImpl implements UserDAO {

	@Override
	public int createUser(User user) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int deleteUser(User user) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public User getUser(int id) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public List<User> searchUserByName(String lastName) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public User searchUserByMail(String mail) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public User searchUserByLogin(String mail, String password) {
		User user = new User ("Amir","abdelaoui", "1234", "amir.abdelaoui@gmail.com");
		return user;
	}

}
