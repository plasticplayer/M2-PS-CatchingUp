package ui;

import java.awt.EventQueue;
import java.awt.event.WindowEvent;

import javax.swing.UIManager;

import communication.Server;

public class Main {
	
	/**
	 * Launch the application.
	 */
	static UserLogin frame;
	public static void main(String[] args) {
	//new Server( "squadfree.net",1918);
	new Server( "192.168.43.90",1918);
	Server.connect();
	
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Throwable e) {
			e.printStackTrace();
		}
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					frame = new UserLogin();
					frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}
	
	/* Configuration config = new Configuration();
	 config.setVisible(true);
	 frame.dispatchEvent(new WindowEvent(frame, WindowEvent.WINDOW_CLOSING));*/


}
