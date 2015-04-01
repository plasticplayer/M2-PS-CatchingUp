package persistence;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import dao.UserRecorderDAO;
import dm.UserRecorder;

public class UserRecorderDAOImpl implements UserRecorderDAO {

	@Override
	public UserRecorder getUserRecorder(int id) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public List<UserRecorder> getUserRecorderList() throws ParseException {
		  List<UserRecorder> userRecorders = new ArrayList<UserRecorder>();
		//  String firstName, String lastName, String password, String email, Date dateBegin, Date dateEnd
		  SimpleDateFormat sdf = new SimpleDateFormat("dd/MM/yyyy");
		  UserRecorder userRecorder = new UserRecorder("Ghilles", "Mostafaoui", "1234", "ghilles.mostafaoui@u-cergy.fr", (Date)sdf.parse("21/12/2014"),(Date)sdf.parse("21/12/2015"));
		  userRecorders.add(userRecorder);
		  userRecorder = null;
		  userRecorder = new UserRecorder("Phillipe", "Laroque", "1234", "phillipe.laroque@u-cergy.fr", (Date)sdf.parse("01/01/2015"),(Date)sdf.parse("01/12/2016"));
		  userRecorders.add(userRecorder);
		  
		return userRecorders;
	}

}
