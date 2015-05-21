package communication;

import java.io.*;
import java.net.*;

public class Server {
	public static Server _Server = null;
	
	private Socket _clientSocket = null; 
	private DataOutputStream _outToServer;
	private BufferedReader _inFromServer;
	private String _host = "";
	private int _port = 1918;
	
	private static PrintWriter toServer;
	public Server ( String host, int port ){
		if ( _Server != null )
			return;
		
		_port = port;
		_host = host;
		_Server = this;
	}
	
	public static boolean connect(){
		if ( _Server == null )
			return false;
		
		try {
			_Server._clientSocket = new Socket( _Server._host, _Server._port);
			toServer = new PrintWriter(_Server._clientSocket.getOutputStream(),true);
			//_Server._outToServer  = new DataOutputStream(_Server._clientSocket.getOutputStream());
			_Server._inFromServer = new BufferedReader(new InputStreamReader(_Server._clientSocket.getInputStream()));
			
		} catch (UnknownHostException e) {
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}
	
	private static boolean isWorking = false;
	
	public static String sendData ( String data) {
		String ans = "", line = "" ;

		while ( isWorking );
		isWorking = true;
		
		try {
	//		System.out.println("TCP Send: " + data );

			toServer.println( "<request>" + data + "</request>" );
			
			while ( !_Server._inFromServer.ready() );
			
			while ( _Server._inFromServer.ready() ){
				line = _Server._inFromServer.readLine();
				if ( line == null )
					break;
				
				//System.out.println( "line: " + line );
				if ( ans == "" )
					ans = line;
				else
					ans += "\n" + line ;
				//System.out.println( "Ans: " + ans + "\n");
				line = "";
			}
			System.out.println( "End data" );

		} catch (IOException e) {
			e.printStackTrace();
			isWorking = false;
			return ans;
		}
		isWorking = false;
		return ans;
	}
}
