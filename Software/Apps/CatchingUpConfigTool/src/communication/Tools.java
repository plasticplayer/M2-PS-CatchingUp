package communication;

public class Tools {
	public static String getValue ( String req, String id ){
		String s = "<" + id + ">";
		
		int start = req.indexOf(s);
		int stop  = req.indexOf("</" + id + ">");
		if ( start == -1 || stop == -1 )
			return "";
		
		return req.substring(start + s.length() , stop );
	}
}
