package persistence;

import javax.swing.JTextArea;

import communication.Server;
import communication.Tools;

public class LogsImpl {
	public static LogsImpl _instance = new LogsImpl();
	
	public void getLogs(JTextArea logsArea){
		 Server.getLogs(logsArea);
	}
}
