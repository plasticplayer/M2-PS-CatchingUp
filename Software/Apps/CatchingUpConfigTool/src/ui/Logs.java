package ui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.JTextField;
import javax.swing.UIManager;
import javax.swing.JButton;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.JLabel;
import javax.swing.LayoutStyle.ComponentPlacement;

import communication.Server;
import persistence.LogsImpl;
import persistence.UserDAOImpl;
import dao.UserDAO;
import dm.User;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.text.ParseException;

import javax.swing.SwingConstants;
import javax.swing.JTextArea;

public class Logs extends JFrame {

	/**
	 * Launch the application.
	 */
	static Logs frame;
	
	/**
	 * Create the frame.
	 */
	
	@SuppressWarnings("deprecation")
	public Logs() {
		frame = this;
		setResizable(false);
		setTitle("");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 688, 534);
		JPanel contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		
		final JButton btnOk = new JButton("OK");
		btnOk.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				frame.dispose();
			}
		});
		
		JTextArea textArea = new JTextArea();
		textArea.setText( "" );
		
		GroupLayout gl_contentPane = new GroupLayout(contentPane);
		gl_contentPane.setHorizontalGroup(
			gl_contentPane.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_contentPane.createSequentialGroup()
					.addContainerGap(385, Short.MAX_VALUE)
					.addComponent(btnOk)
					.addGap(218))
				.addGroup(gl_contentPane.createSequentialGroup()
					.addComponent(textArea, GroupLayout.PREFERRED_SIZE, 649, GroupLayout.PREFERRED_SIZE)
					.addContainerGap(29, Short.MAX_VALUE))
		);
		gl_contentPane.setVerticalGroup(
			gl_contentPane.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_contentPane.createSequentialGroup()
					.addContainerGap(17, Short.MAX_VALUE)
					.addComponent(textArea, GroupLayout.PREFERRED_SIZE, 438, GroupLayout.PREFERRED_SIZE)
					.addPreferredGap(ComponentPlacement.UNRELATED)
					.addComponent(btnOk)
					.addContainerGap())
		);
		contentPane.setLayout(gl_contentPane);
	}
}
