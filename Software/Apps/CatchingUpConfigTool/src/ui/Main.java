package ui;

import java.awt.EventQueue;
import java.awt.event.WindowEvent;

import javax.swing.UIManager;

import communication.CardReader;
import communication.Server;
import communication.Tools;

public class Main {

	/**
	 * Launch the application.
	 */
	static UserLogin frame;
	public static void main(String[] args) {
		Runtime.getRuntime().addShutdownHook(new Thread() {
			@Override
			public void run() {
				CardReader.close();
			}

		});
		new CardReader();
		
	//	Tools.enableDebug = true;

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
}
