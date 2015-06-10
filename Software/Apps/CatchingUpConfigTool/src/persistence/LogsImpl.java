package persistence;

import communication.Server;
import communication.Tools;

public class LogsImpl {
	public static LogsImpl _instance = new LogsImpl();
	
	public String getLogs(){
		String res = Server.sendData("<type>need_logs</type>");
		
		String[] lines = res.split("["+System.getProperty("line.separator")+"]");
		String s = "", line;
		
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"log");
			if ( line != "" ){
				if ( s == "" )
					s += line;
				else
					s +=  System.getProperty("line.separator") + line;
			}
		}
		return s;
	}
}
