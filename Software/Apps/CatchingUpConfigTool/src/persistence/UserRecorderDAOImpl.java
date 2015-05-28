package persistence;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import communication.Tools;

import dao.UserRecorderDAO;
import dm.UserRecorder;

public class UserRecorderDAOImpl implements UserRecorderDAO {
	public static List<UserRecorder> userRecorders = new ArrayList<UserRecorder>();
	public static SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
	public static UserRecorderDAOImpl _instance = new UserRecorderDAOImpl();
	
	
	@Override
	public UserRecorder getUserRecorder(int id) {
		for ( UserRecorder user : userRecorders ){
			if ( user.getId() == id )
				return user;
		}
		return null;
	}

	public boolean createUserRecorder( UserRecorder userRecorder ){
		String req = "<type>create_userrecorder</type>"
				+ "<user>"
				+ "<firstname>" + userRecorder.getFirstName() + "</firstname>"
				+ "<lastname>"  + userRecorder.getLastName()  + "</lastname>"
				+ "<password>"  + userRecorder.getPassword()  + "</password>"
				+ "<email>" + userRecorder.getEmail() + "</email>"
				+ "<datebegin>" + sdf.format(userRecorder.getDateBegin()) + "</datebegin>"
				+ "<dateend>"+ sdf.format(userRecorder.getDateEnd()) + "</dateend>"
				+ "</user>"; 

		String res = communication.Server.sendData(req);
		String[] lines = res.split(System.getProperty("line.separator"));
		
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("CREATE_USERSRECORDER") != 0 )
			return false;

		String idUser, line;
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue(lines[i], "user");
			if ( line == "" )
				continue;
			
			idUser 	= Tools.getValue( line ,"iduser");
			if ( idUser.trim().compareTo("0") == 0 ){
				Tools.LOGGER_ERROR("Cannot create userRecorder");
				return false;
			}
			
			int id = Integer.parseInt(idUser.trim());
			userRecorder.setId( id );
			Tools.LOGGER_INFO("Create userRecorder OK");
			userRecorders.add(userRecorder);
			return true;
		}
		return false;
	}

	@Override
	public List<UserRecorder> getUserRecorderList() throws ParseException {
		if ( !userRecorders.isEmpty() )
			return userRecorders;
		
		String res = communication.Server.sendData("<type>need_users_recorders</type>");

		String[] lines = res.split(System.getProperty("line.separator"));
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");

		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("GET_USERS_RECORDERS") != 0 )
			return null;

		UserRecorder userRecorder;
		String idUser, firstName, lastName, email, dateBegin, dateEnd;
		for ( int i = 1; i < lines.length; i++ ){
			idUser 	= Tools.getValue( lines[i] ,"id");
			firstName 	= Tools.getValue( lines[i] ,"firstname");
			lastName 	= Tools.getValue( lines[i] ,"lastname");
			email 		= Tools.getValue( lines[i] ,"email");
			dateBegin 	= Tools.getValue( lines[i] ,"datebegin");
			dateEnd 	= Tools.getValue( lines[i] ,"dateend");

			if ( idUser == "" || firstName == "" || lastName == "" || email == "" || dateBegin == "" || dateEnd == "" )
				continue;

			int id = Integer.parseInt(idUser.trim());
			userRecorder = new UserRecorder(firstName, lastName, "", email , (Date)sdf.parse(dateBegin),(Date)sdf.parse(dateEnd));
			userRecorder.setId(id);
			userRecorders.add(userRecorder);
		}
		return userRecorders;
	}

	@Override
	public UserRecorderDAO getInstance() {
		if(_instance == null)
			_instance = new UserRecorderDAOImpl();
		return _instance;
	}
}
