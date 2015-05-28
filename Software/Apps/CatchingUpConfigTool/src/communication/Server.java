package communication;

import java.io.*;
import java.net.*;

import com.sun.javafx.collections.ElementObservableListDecorator;

public class Server {
	public static Server _Server = null;
	private static boolean isWorking = false;
	private Socket _clientSocket = null; 
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
			_Server._inFromServer = new BufferedReader(new InputStreamReader(_Server._clientSocket.getInputStream()));
			Tools.LOGGER_INFO("Connected to server");
			return true;
			
		} catch (UnknownHostException e) {
			e.printStackTrace();
			//return false;
		} catch (IOException e) {
			e.printStackTrace();
			//return false;
		}
		Tools.LOGGER_ERROR( "Cannot connect to server" );
		return true;
	}
	
	public static void disconnect (){
		if ( _Server == null )
			return;

		if ( _Server._clientSocket.isConnected() )
			try {
				_Server._inFromServer.close();
				toServer.close();
				_Server._clientSocket.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	}
	
	public static String sendData ( String data) {
		String ans = "", line = "" ;

		while ( isWorking );
		isWorking = true;
		
		try {
			Tools.LOGGER_DEBUG("TCP Send: " + data );

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
					ans += System.lineSeparator()+ line ;
				//System.out.println( "Ans: " + ans + "\n");
				line = "";
			}
			Tools.LOGGER_DEBUG( "TCP get: " + ans );

		} catch (IOException e) {
			e.printStackTrace();
			isWorking = false;
			return ans;
		}
		isWorking = false;
		return ans;
	}

	public static boolean verifyPassword( String data ){
		if ( _Server == null )
			return false;
		
		String r = sendData("<type>password</type><pass>" + data + "</pass>");
		
		String state = Tools.getValue(r, "state");
		if ( state.trim().compareTo("1") == 0){
			Tools.LOGGER_INFO("Password OK");
			return true;
		}
		Tools.LOGGER_INFO("Password error");
		return false;
	}

}
