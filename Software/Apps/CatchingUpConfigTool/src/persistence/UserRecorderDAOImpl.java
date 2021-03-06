package persistence;

import java.io.UnsupportedEncodingException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import ui.MessageBox;
import communication.Tools;
import dao.UserRecorderDAO;
import dm.UserRecorder;

public class UserRecorderDAOImpl implements UserRecorderDAO {
	public static List<UserRecorder> userRecorders = new ArrayList<UserRecorder>();
	public static SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
	public static UserRecorderDAOImpl _instance = new UserRecorderDAOImpl();
	
	public boolean updateUserRecorder ( UserRecorder user, boolean updateFn, boolean updateLn, boolean updateMail, boolean updatePassword, boolean updateBeginDate, boolean updateEndDate ){
		if ( !updateFn && !updateLn && !updateMail && !updatePassword && !updateBeginDate && !updateEndDate ){
			Tools.LOGGER_DEBUG("UserRecorder: Nothing to update");
			return true;
		}
		
		String req = "<type>update_usersrecorders</type><users><user><id>" + user.getId() + "</id>";
		if ( updateFn )
			req += "<firstname>" + user.getFirstName() + "</firstname>";
		if ( updateLn )
			req += "<lastname>" + user.getLastName() + "</lastname>";
		if ( updateMail )
			req += "<email>" + user.getEmail() + "</email>";
		if ( updatePassword )
			req += "<password>" + MD5(user.getPassword()) + "</password>";
		if ( updateBeginDate )
			req += "<begin>" + sdf.format(user.getDateBegin()) + "</begin>";
		if ( updateEndDate )
			req += "<end>" + sdf.format(user.getDateEnd()) + "</end>";	
		
		String res = communication.Server.sendData(req + "</user></users>");
		String[] lines =res.split("["+System.getProperty("line.separator")+"]");
		  
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("UPDATES_USERSRECORDERS") != 0 )
		  	return false;

		String idUserRecorder, success, line;
		
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"user");
			if ( line == "" )
				continue;
			
			idUserRecorder = Tools.getValue(line, "iduser");
			success = Tools.getValue(line, "success" );
			
			if ( idUserRecorder.compareTo("" + user.getId()) == 0 ){
				if ( success.compareTo( "1") == 0 ){
					Tools.LOGGER_INFO("Update userRecorder OK");
					return true;
				}
			}
		}
		new MessageBox("Erreur de la mise à jour de l'intervenant").setVisible(true);
		Tools.LOGGER_ERROR("Cannot update userRecorder");
		return false;
	}
	
	@Override
	public UserRecorder getUserRecorder(int id) {
		for ( UserRecorder user : userRecorders ){
			if ( user.getId() == id )
				return user;
		}
		return null;
	}
	public String MD5(String md5)  {
		   try {
			    md5 += Tools.Salt;
		        java.security.MessageDigest md = java.security.MessageDigest.getInstance("MD5");
		        byte[] array = md.digest(md5.getBytes("UTF-8"));
		        StringBuffer sb = new StringBuffer();
		        for (int i = 0; i < array.length; ++i) {
		          sb.append(Integer.toHexString((array[i] & 0xFF) | 0x100).substring(1,3));
		       }
		        return sb.toString();
		    } catch (java.security.NoSuchAlgorithmException e) {
		    } catch (UnsupportedEncodingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		    return null;
		}
	public boolean createUserRecorder( UserRecorder userRecorder ){
		String req = "<type>create_userrecorder</type>"
				+ "<user>"
				+ "<firstname>" + userRecorder.getFirstName() + "</firstname>"
				+ "<lastname>"  + userRecorder.getLastName()  + "</lastname>"
				+ "<password>"  + MD5(userRecorder.getPassword()) + "</password>"
				+ "<email>" + userRecorder.getEmail() + "</email>"
				+ "<datebegin>" + sdf.format(userRecorder.getDateBegin()) + "</datebegin>"
				+ "<dateend>"+ sdf.format(userRecorder.getDateEnd()) + "</dateend>"
				+ "</user>"; 

		String res = communication.Server.sendData(req);
		String[] lines = res.split("["+System.getProperty("line.separator")+"]");
		
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
				new MessageBox("Erreur de le cr�ation de l'intervenant").setVisible(true);
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

		String[] lines = res.split("["+System.getProperty("line.separator")+"]");
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
