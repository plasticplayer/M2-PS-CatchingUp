package communication;

import java.awt.Image;
import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import javax.imageio.ImageIO;

import sun.org.mozilla.javascript.internal.UintMap;
import ui.MessageBox;

public class Server {
	public static Server _Server = new Server("127.0.0.1", 1918);
	private static boolean isWorking = false;
	private Socket _clientSocket = null;
	private BufferedReader _inFromServer = null;
	private String _host = "";
	private int _port = 1918;

	private static PrintWriter toServer;

	public Server(String host, int port) {
		if (_Server != null)
			return;

		_port = port;
		_host = host;
		_Server = this;
	}

	public static void setIp(String ip) {
		if (_Server == null)
			new Server(ip, 1918);
		else
			_Server._host = ip;
	}

	public static void setPort(int port) {
		if (_Server != null)
			_Server._port = port;
	}

	public static boolean connect() {
		if (_Server == null)
			return false;

		try {
			_Server._clientSocket = new Socket(_Server._host, _Server._port);
			toServer = new PrintWriter(_Server._clientSocket.getOutputStream(),
					true);
			_Server._inFromServer = new BufferedReader(new InputStreamReader(
					_Server._clientSocket.getInputStream()));
			Tools.LOGGER_INFO("Connected to server");
			return true;

		} catch (UnknownHostException e) {
			// e.printStackTrace();
			// return false;
		} catch (IOException e) {
			// e.printStackTrace();
			// return false;
		}
		new MessageBox("Connexion impossible au server").setVisible(true);
		Tools.LOGGER_ERROR("Cannot connect to server");
		return false;
	}

	public static void disconnect() {
		if (_Server == null)
			return;
		Tools.LOGGER_INFO("Try disconnection");

		if (_Server._clientSocket.isConnected())
			try {
				_Server._inFromServer.close();
				toServer.close();
				_Server._clientSocket.close();
				Tools.LOGGER_DEBUG("Server disconnection");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	}

	public static String sendData(String data) {
		if (_Server == null) {
			Tools.LOGGER_ERROR("Server is null");
			return "";
		} else if (_Server._inFromServer == null) {
			Tools.LOGGER_ERROR("_inFromServer is null");
			return "";
		}
		String ans = "", line = "";

		while (isWorking)
			;
		isWorking = true;

		try {
			_Server._inFromServer.reset();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			// e1.printStackTrace();
		}

		try {

			Tools.LOGGER_DEBUG("TCP Send: " + data);

			toServer.println("<request>" + data + "</request>");

			while (!_Server._inFromServer.ready())
				;

			while (_Server._inFromServer.ready()) {
				line = _Server._inFromServer.readLine();
				if (line == null)
					break;

				// System.out.println( "line: " + line );
				if (ans == "")
					ans = line;
				else
					ans += "\n" + line;
				// System.out.println( "Ans: " + ans + "\n");
				line = "";
			}
			Tools.LOGGER_DEBUG("TCP get: " + ans);

		} catch (IOException e) {
			e.printStackTrace();
			isWorking = false;
			return ans;
		}
		isWorking = false;
		return ans;
	}

	public static Image getImage(String data) {
		if (_Server == null) {
			Tools.LOGGER_ERROR("Server is null");
			return null;
		} else if (_Server._inFromServer == null) {
			Tools.LOGGER_ERROR("_inFromServer is null");
			return null;
		}
		Image res = null;

		while (isWorking)
			;
		isWorking = true;

		try {
			_Server._inFromServer.reset();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			// e1.printStackTrace();
		}

		try {

			Tools.LOGGER_DEBUG("TCP Send: " + data);

			toServer.println("<request>" + data + "</request>");
			List<Byte> datas = new ArrayList<Byte>();
			BufferedInputStream st = new BufferedInputStream(
					_Server._clientSocket.getInputStream());
			// while ( !_Server._inFromServer.ready() );
			int r = st.read();
			int imageSizeBytes = r;
			Tools.LOGGER_DEBUG("1 : "+ r);
			r = st.read();
			imageSizeBytes += ((r&0xFF) << 8) ;
			Tools.LOGGER_DEBUG("2 : "+ r);
			r = st.read();
			imageSizeBytes += ((r&0xFF) << 16) ;
			Tools.LOGGER_DEBUG("3 : "+ r);
			r = st.read();
			imageSizeBytes += ((r&0xFF) << 24) ;
			Tools.LOGGER_DEBUG("4 : "+ r);
			
			int byteCountRead = 0;
			Tools.LOGGER_DEBUG("Size image " + imageSizeBytes);
			while (imageSizeBytes > byteCountRead) {
				datas.add((byte) st.read());
				byteCountRead++;
			}
			byte[] dataByte = new byte[datas.size()];
			for (int i = 0; i < dataByte.length; i++) {
				dataByte[i] = (byte) datas.get(i);
			}
			res = ImageIO.read(new ByteArrayInputStream(dataByte));
			// et la on ajoute les bytes plus petits que 1024 ) la fin

			/*
			 * while ( !_Server._inFromServer.ready() );
			 * 
			 * while ( _Server._inFromServer.ready() ){ int line =
			 * _Server._inFromServer.read(); if ( line == -1 ) break;
			 * 
			 * //System.out.println( "line: " + line ); ans += ((char)line);
			 * //System.out.println( "Ans: " + ans + "\n");
			 * 
			 * }
			 */
			// Tools.LOGGER_DEBUG( "TCP get: " + ans );

		} catch (IOException e) {
			e.printStackTrace();
			isWorking = false;
			return null;
		}
		isWorking = false;
		return res;
	}

	public static boolean verifyPassword(String data) {
		if (_Server == null)
			return false;

		String r = sendData("<type>password</type><pass>" + data + "</pass>");

		String state = Tools.getValue(r, "state");
		if (state.trim().compareTo("1") == 0) {
			Tools.LOGGER_INFO("Password OK");
			return true;
		}
		Tools.LOGGER_INFO("Password error");
		return false;
	}

}
