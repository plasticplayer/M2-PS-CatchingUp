package ui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;

import java.awt.GridLayout;

import javax.swing.SwingConstants;

import java.awt.Component;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;

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
	private JPasswordField passwordFieldPassword;
	private JTextField txtBoxEmail;
	private JPasswordField passwordFieldConfirmPassword;
	private JButton okButton;
	private JButton cancelButton;

	/**
	 * Create the dialog.
	 */
	public SpeakerUpdate(String firstName, String lastName, String email, String dateBegin, String dateEnd) {
		setTitle("Modification intervenant");
		this.firstName = firstName;
		this.lastName = lastName;
		this.password = password;
		this.email = email;
		this.dateBegin = dateBegin;
		this.dateEnd = dateEnd;
		
		setBounds(100, 100, 735, 491);
		getContentPane().setLayout(new BorderLayout());
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		contentPanel.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
		
		JPanel TopPanel = new JPanel();
		contentPanel.add(TopPanel);
		TopPanel.setLayout(new GridLayout(0, 1, 0, 0));
		
		JPanel identityPanel = new JPanel();
		TopPanel.add(identityPanel);
		FlowLayout flowLayout = (FlowLayout) identityPanel.getLayout();
		flowLayout.setVgap(10);
		
		JLabel lblNom = new JLabel("Nom :");
		identityPanel.add(lblNom);
		txtBoxLastname = new JTextField(lastName);
		identityPanel.add(txtBoxLastname);
		txtBoxLastname.setColumns(10);
		
		JLabel lblFirstName = new JLabel("Pr\u00E9nom :");
		identityPanel.add(lblFirstName);
		
		txtBoxFirstname = new JTextField(firstName);
		identityPanel.add(txtBoxFirstname);
		txtBoxFirstname.setColumns(10);
		
		JPanel passwordPanel = new JPanel();
		TopPanel.add(passwordPanel);
		
		JLabel lblPassword = new JLabel("Mot de passe :");
		lblPassword.setHorizontalAlignment(SwingConstants.CENTER);
		passwordPanel.add(lblPassword);
		passwordFieldPassword = new JPasswordField(password);
		passwordFieldPassword.setHorizontalAlignment(SwingConstants.CENTER);
		passwordPanel.add(passwordFieldPassword);
		passwordFieldPassword.setColumns(10);
		
		JLabel lblConfirmPassword = new JLabel("Confirmation mot de passe :");
		lblConfirmPassword.setHorizontalAlignment(SwingConstants.CENTER);
		passwordPanel.add(lblConfirmPassword);
		
		passwordFieldConfirmPassword = new JPasswordField();
		passwordFieldConfirmPassword.setHorizontalAlignment(SwingConstants.CENTER);
		passwordFieldConfirmPassword.setColumns(10);
		passwordPanel.add(passwordFieldConfirmPassword);
		
		JPanel mailPanel = new JPanel();
		TopPanel.add(mailPanel);
		
		JLabel lblEmail = new JLabel("Email :");
		mailPanel.add(lblEmail);
		txtBoxEmail = new JTextField(email);
		mailPanel.add(txtBoxEmail);
		txtBoxEmail.setColumns(25);
		
		JPanel rightPanel = new JPanel();
		contentPanel.add(rightPanel);
		rightPanel.setLayout(new BorderLayout(0, 0));
		
		JPanel rightPresentationPanel = new JPanel();
		rightPanel.add(rightPresentationPanel, BorderLayout.NORTH);
		rightPresentationPanel.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
		
		JLabel lblNewLabel = new JLabel("Droits d'enregistrements de la personne");
		rightPresentationPanel.add(lblNewLabel);
		lblNewLabel.setHorizontalAlignment(SwingConstants.CENTER);
		lblNewLabel.setVerticalAlignment(SwingConstants.TOP);
		
		JPanel rightContentPanel = new JPanel();
		rightPanel.add(rightContentPanel, BorderLayout.CENTER);
		rightContentPanel.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
		
		JPanel calendarGridLayout = new JPanel();
		rightPanel.add(calendarGridLayout, BorderLayout.WEST);
		calendarGridLayout.setLayout(new GridLayout(1, 2, 0, 0));
		
		JPanel dateBeginPanel = new JPanel();
		calendarGridLayout.add(dateBeginPanel);
		dateBeginPanel.setLayout(new BorderLayout(0, 0));
		
		JLabel lblDateBegin = new JLabel("Date de d\u00E9but :");
		dateBeginPanel.add(lblDateBegin, BorderLayout.NORTH);
		calendar calBegin = new calendar();
		dateBeginPanel.add(calBegin, BorderLayout.CENTER);
		
		JPanel dateEndPanel = new JPanel();
		calendarGridLayout.add(dateEndPanel);
		dateEndPanel.setLayout(new BorderLayout(0, 0));
		
		JLabel lblDateEnd = new JLabel("Date de d\u00E9but :");
		dateEndPanel.add(lblDateEnd, BorderLayout.NORTH);
		calendar calEnd = new calendar();
		dateEndPanel.add(calEnd);
		{
			JPanel endPanel = new JPanel();
			rightPanel.add(endPanel, BorderLayout.SOUTH);
			endPanel.setLayout(new GridLayout(2, 1, 0, 0));
			
			JPanel detailsPanel = new JPanel();
			endPanel.add(detailsPanel);
			
			JLabel lblNewLabel_1 = new JLabel("Carte associ\u00E9e:");
			detailsPanel.add(lblNewLabel_1);
			
			JComboBox comboBox = new JComboBox();
			detailsPanel.add(comboBox);
			
			JCheckBox chckbxNewCheckBox = new JCheckBox("D\u00E9sactivation de l'utilisateur");
			detailsPanel.add(chckbxNewCheckBox);
			
			JPanel buttonPanel = new JPanel();
			endPanel.add(buttonPanel);
			{
				cancelButton = new JButton("Cancel");
				buttonPanel.add(cancelButton);
				cancelButton.setAlignmentX(Component.CENTER_ALIGNMENT);
				cancelButton.setActionCommand("Cancel");
			}
			{
				okButton = new JButton("OK");
				buttonPanel.add(okButton);
				okButton.setAlignmentX(Component.CENTER_ALIGNMENT);
				okButton.setActionCommand("OK");
				getRootPane().setDefaultButton(okButton);
			}
		}
	}
}
