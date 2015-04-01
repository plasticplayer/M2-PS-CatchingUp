package ui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;

import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.LayoutStyle.ComponentPlacement;
import javax.swing.border.EmptyBorder;

public class SpeakerUpdate extends JDialog {
	private String firstName;
	private String lastName;
	private String password;
	private String email;
	private String dateBegin;
	private String dateEnd;
	

	private final JPanel contentPanel = new JPanel();
	private JTextField txtBoxFirstname;
	private JTextField txtBoxLastname;
	private JTextField txtBoxpassword;
	private JTextField txtBoxEmail;
	private JTextField txtBoxDateBegin;
	private JTextField txtBoxDateEnd;

	/**
	 * Create the dialog.
	 */
	public SpeakerUpdate(String firstName, String lastName, String password, String email, String dateBegin, String dateEnd) {
		setTitle("Modification intervenant");
		this.firstName = firstName;
		this.lastName = lastName;
		this.password = password;
		this.email = email;
		this.dateBegin = dateBegin;
		this.dateEnd = dateEnd;
		
		setBounds(100, 100, 519, 413);
		getContentPane().setLayout(new BorderLayout());
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		
		txtBoxFirstname = new JTextField(firstName);
		txtBoxFirstname.setColumns(10);
		txtBoxLastname = new JTextField(lastName);
		txtBoxLastname.setColumns(10);
		txtBoxpassword = new JTextField(password);
		txtBoxpassword.setColumns(10);
		txtBoxEmail = new JTextField(email);
		txtBoxEmail.setColumns(10);
		txtBoxDateBegin = new JTextField(dateBegin);
		txtBoxDateBegin.setColumns(10);
		txtBoxDateEnd = new JTextField(dateEnd);
		txtBoxDateEnd.setColumns(10);
		
		JLabel lblFirstName = new JLabel("Pr\u00E9nom");
		
		JLabel lblNom = new JLabel("Nom");
		
		JLabel lblMotDePasse = new JLabel("Mot de passe");
		
		JLabel lblEmail = new JLabel("email");
		
		JLabel lblDateBegin = new JLabel("Date de d\u00E9but");
		
		JLabel lblDateEnd = new JLabel("Date de fin");
		GroupLayout gl_contentPanel = new GroupLayout(contentPanel);
		gl_contentPanel.setHorizontalGroup(
			gl_contentPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_contentPanel.createSequentialGroup()
					.addContainerGap(140, Short.MAX_VALUE)
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.LEADING)
						.addComponent(lblFirstName)
						.addComponent(lblNom)
						.addComponent(lblMotDePasse)
						.addComponent(lblEmail)
						.addComponent(lblDateBegin)
						.addComponent(lblDateEnd))
					.addGap(31)
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.LEADING)
						.addComponent(txtBoxDateEnd, GroupLayout.PREFERRED_SIZE, 119, GroupLayout.PREFERRED_SIZE)
						.addComponent(txtBoxDateBegin, GroupLayout.PREFERRED_SIZE, 119, GroupLayout.PREFERRED_SIZE)
						.addComponent(txtBoxEmail, GroupLayout.PREFERRED_SIZE, 119, GroupLayout.PREFERRED_SIZE)
						.addComponent(txtBoxpassword, GroupLayout.PREFERRED_SIZE, 119, GroupLayout.PREFERRED_SIZE)
						.addComponent(txtBoxLastname, GroupLayout.PREFERRED_SIZE, 119, GroupLayout.PREFERRED_SIZE)
						.addComponent(txtBoxFirstname, GroupLayout.PREFERRED_SIZE, 119, GroupLayout.PREFERRED_SIZE))
					.addGap(157))
		);
		gl_contentPanel.setVerticalGroup(
			gl_contentPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_contentPanel.createSequentialGroup()
					.addGap(78)
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.BASELINE)
						.addComponent(txtBoxFirstname, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblFirstName))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.BASELINE)
						.addComponent(txtBoxLastname, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblNom))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.BASELINE)
						.addComponent(txtBoxpassword, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblMotDePasse))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.BASELINE)
						.addComponent(txtBoxEmail, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblEmail))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.BASELINE)
						.addComponent(txtBoxDateBegin, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblDateBegin))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.BASELINE)
						.addComponent(txtBoxDateEnd, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblDateEnd))
					.addContainerGap(104, Short.MAX_VALUE))
		);
		contentPanel.setLayout(gl_contentPanel);
		{
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new FlowLayout(FlowLayout.RIGHT));
			getContentPane().add(buttonPane, BorderLayout.SOUTH);
			{
				JButton okButton = new JButton("OK");
				okButton.setActionCommand("OK");
				buttonPane.add(okButton);
				getRootPane().setDefaultButton(okButton);
			}
			{
				JButton cancelButton = new JButton("Cancel");
				cancelButton.setActionCommand("Cancel");
				buttonPane.add(cancelButton);
			}
		}
	}
}
