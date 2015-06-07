package ui;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.JTextField;
import javax.swing.JButton;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.JLabel;
import javax.swing.LayoutStyle.ComponentPlacement;

import communication.Server;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.text.ParseException;

public class UserLogin extends JFrame {

	private static String defaultIp = "catchingup.fr:1918";
	/**
	 * Launch the application.
	 */
	static UserLogin frame;
/*	public static void main(String[] args) {
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
	}*/

	/**
	 * Create the frame.
	 */
	@SuppressWarnings("deprecation")
	public UserLogin() {
		frame = this;
		setResizable(false);
		setTitle("Authentification");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 449, 275);
		JPanel contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);

		final JTextField tbxUser = new JTextField();
		final JTextField tbxAddress = new JTextField();
		final JTextField tbxPassword = new JTextField();

		final JLabel lblUser = new JLabel("Utilisateur :");
		tbxUser.setText("Administrateur");
		tbxUser.disable();

		JLabel lblPassword = new JLabel("Mot de passe :");
		JLabel lblAddress = new JLabel("Addresse :");

		tbxAddress.setText(defaultIp);

		final JButton btnValidate = new JButton("Valider");
		btnValidate.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				String[] parameters = tbxAddress.getText().split(":");

				Server.setIp( parameters[0] );
				if ( parameters.length > 1 ){
					int port = Integer.parseInt(parameters[1]);
					Server.setPort(port);
				}

				if( Server.connect() ){
					if ( Server.verifyPassword(tbxPassword.getText()) ){
						try {
							new Configuration().setVisible(true);
							frame.dispose();
							return;
						}
						catch (ParseException e) {}
					}
					else
						new MessageBox( "Utilisateur / mot de passe incorrect." ).setVisible(true);
					
					Server.disconnect();
				}
			}
		});

		GroupLayout gl_contentPane = new GroupLayout(contentPane);
		gl_contentPane.setHorizontalGroup(
				gl_contentPane.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_contentPane.createSequentialGroup()
						.addContainerGap(96, Short.MAX_VALUE)
						.addGroup(gl_contentPane.createParallelGroup(Alignment.TRAILING)
								.addComponent(lblPassword, GroupLayout.PREFERRED_SIZE, 73, GroupLayout.PREFERRED_SIZE)
								.addComponent(lblUser, GroupLayout.PREFERRED_SIZE, 61, GroupLayout.PREFERRED_SIZE)
								.addComponent(lblAddress, GroupLayout.PREFERRED_SIZE, 73, GroupLayout.PREFERRED_SIZE))
								.addGap(25)
								.addGroup(gl_contentPane.createParallelGroup(Alignment.LEADING)
										.addComponent(tbxPassword, GroupLayout.PREFERRED_SIZE, 159, GroupLayout.PREFERRED_SIZE)
										.addGroup(gl_contentPane.createParallelGroup(Alignment.TRAILING)
												.addGroup(gl_contentPane.createSequentialGroup()
														.addGroup(gl_contentPane.createParallelGroup(Alignment.LEADING)
																.addComponent(tbxUser, GroupLayout.PREFERRED_SIZE, 159, GroupLayout.PREFERRED_SIZE)
																.addComponent(tbxAddress, GroupLayout.PREFERRED_SIZE, 159, GroupLayout.PREFERRED_SIZE))
																.addGap(80))
																.addGroup(gl_contentPane.createSequentialGroup()
																		.addComponent(btnValidate)
																		.addPreferredGap(ComponentPlacement.RELATED))))
																		.addGap(6))
				);
		gl_contentPane.setVerticalGroup(
				gl_contentPane.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_contentPane.createSequentialGroup()
						.addGap(67)
						.addGroup(gl_contentPane.createParallelGroup(Alignment.TRAILING)
								.addComponent(tbxUser, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
								.addComponent(lblUser))
								.addGap(18)
								.addGroup(gl_contentPane.createParallelGroup(Alignment.LEADING)
										.addComponent(tbxPassword, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
										.addComponent(lblPassword))
										.addPreferredGap(ComponentPlacement.UNRELATED)
										.addGroup(gl_contentPane.createParallelGroup(Alignment.BASELINE)
												.addComponent(tbxAddress, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
												.addComponent(lblAddress))
												.addPreferredGap(ComponentPlacement.RELATED, 27, Short.MAX_VALUE)
												.addComponent(btnValidate)
												.addContainerGap())
				);
		contentPane.setLayout(gl_contentPane);
	}
}
