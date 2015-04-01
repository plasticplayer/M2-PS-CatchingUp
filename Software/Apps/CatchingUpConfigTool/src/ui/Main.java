package ui;

import java.awt.EventQueue;
import java.awt.event.WindowEvent;

import javax.swing.UIManager;

public class Main {
	
	/**
	 * Launch the application.
	 */
	static UserLogin frame;
	public static void main(String[] args) {
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
