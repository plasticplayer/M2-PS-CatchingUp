package communication;

public class Tools {
	public static boolean enableDebug = true;
	public static String getValue ( String req, String id ){
		String s = "<" + id + ">";
		
		int start = req.indexOf(s);
		int stop  = req.indexOf("</" + id + ">");
		if ( start == -1 || stop == -1 )
			return "";
		
		return req.substring(start + s.length() , stop );
	}
	
	public static void LOGGER_DEBUG ( String s ){
		if ( enableDebug == true )
			System.out.println("Debug : " + s);
	}
	
	public static void LOGGER_INFO ( String s ){
		System.out.println("Info  : " + s);
	}
	
	public static void LOGGER_ERROR ( String s ){
		System.out.println("Error : " + s);
	}
}
