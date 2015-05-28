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
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Date;
import java.util.List;

import javax.swing.JCheckBox;
import javax.swing.JComboBox;

import persistence.CardDAOImpl;
import persistence.UserRecorderDAOImpl;
import dao.UserRecorderDAO;
import dm.Card;
import dm.UserRecorder;

public class SpeakerUpdate extends JDialog {
	private String firstName;
	private String lastName;
	private String password;
	private String email;
	private Date dateBegin;
	private Date dateEnd;
	
	private UserRecorder userUpdated;

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
	public SpeakerUpdate( UserRecorder user ) {
		setTitle("Modification intervenant");
		userUpdated = user;
		this.firstName = user.getFirstName();
		this.lastName = user.getLastName();
		this.password = user.getPassword();
		this.email = user.getEmail();
		this.dateBegin = user.getDateBegin();
		this.dateEnd = user.getDateBegin();

		
		
		
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
		final calendar calBegin = new calendar();
		dateBeginPanel.add(calBegin, BorderLayout.CENTER);
		//calBegin.setDate(1900+dateBegin.getYear(),dateBegin.getMonth()-1,dateBegin.getDate());
		calBegin.setDate(dateBegin.getYear(), dateBegin.getMonth() -1, dateBegin.getDate() );
		
		JPanel dateEndPanel = new JPanel();
		calendarGridLayout.add(dateEndPanel);
		dateEndPanel.setLayout(new BorderLayout(0, 0));
		
		JLabel lblDateEnd = new JLabel("Date de d\u00E9but :");
		dateEndPanel.add(lblDateEnd, BorderLayout.NORTH);
		final calendar calEnd = new calendar();
		//calEnd.setDate(2010, 01, 12); //TODO Set Date
		//calEnd.setDate(1900+dateEnd.getYear(),dateEnd.getMonth()-1,dateEnd.getDate());
		calEnd.setDate(dateEnd.getYear(), dateEnd.getMonth() -1, dateEnd.getDate() );
		
		dateEndPanel.add(calEnd);
		{
			JPanel endPanel = new JPanel();
			rightPanel.add(endPanel, BorderLayout.SOUTH);
			endPanel.setLayout(new GridLayout(2, 1, 0, 0));
			
			JPanel detailsPanel = new JPanel();
			endPanel.add(detailsPanel);
			
			JLabel lblNewLabel_1 = new JLabel("Carte associ\u00E9e:");
			detailsPanel.add(lblNewLabel_1);
			
			final JComboBox cardList = new JComboBox();
			cardList.addItem("Aucune");
			for ( Card card : CardDAOImpl._instance.getCardFree(userUpdated) ){
				cardList.addItem(card);
				if ( card.getUser() == userUpdated )
					cardList.setSelectedItem(card);
			}
			detailsPanel.add(cardList);

			
			
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
				okButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						if ( passwordFieldPassword.getText().compareTo(passwordFieldConfirmPassword.getText()) != 0 ||
								txtBoxEmail.getText().isEmpty() || txtBoxFirstname.getText().isEmpty() || txtBoxLastname.getText().isEmpty() )
								return;
						Date deb = calBegin.getDate();
						Date end = calEnd.getDate();
						
						boolean updateFn = ( userUpdated.getFirstName().compareTo(txtBoxFirstname.getText()) != 0 );
						boolean updateLn = ( userUpdated.getLastName().compareTo(txtBoxLastname.getText()) != 0 );
						boolean updateMail = ( userUpdated.getEmail().compareTo(txtBoxEmail.getText()) != 0 );
						boolean updatePassword = !passwordFieldPassword.getText().isEmpty();
						boolean updateBeginDate = (deb != userUpdated.getDateBegin() );
						boolean updateEndDate = (end != userUpdated.getDateEnd() );
						
						
						if ( updateFn )
							userUpdated.setFirstName(txtBoxFirstname.getText());
						if ( updateLn )
							userUpdated.setLastName(txtBoxLastname.getText());
						if ( updateMail )
							userUpdated.setEmail(txtBoxEmail.getText());
						if ( updatePassword )
							userUpdated.setPassword(passwordFieldPassword.getText());
						if ( updateBeginDate )
							userUpdated.setDateBegin(deb);
						if ( updateEndDate )
							userUpdated.setDateEnd(end);
						
						
						/*UserRecorder user = new UserRecorder(txtBoxFirstname.getText(), txtBoxLastname.getText(),
								passwordFieldPassword.getText(), txtBoxEmail.getText(), calBegin.getDate(),calEnd.getDate() );*/

						
						UserRecorderDAO dao = UserRecorderDAOImpl._instance;
						if ( dao.updateUserRecorder(userUpdated, updateFn, updateLn, updateMail, updatePassword, updateBeginDate, updateEndDate) ){
							Card userCard = CardDAOImpl._instance.getCardFromIdUser(userUpdated);
							if ( cardList.getSelectedIndex() == 0 && userCard != null ){
								userCard.setUser(null);
								CardDAOImpl._instance.updateCard(userCard);
							}
							else{
								Card newCard = (Card) cardList.getSelectedItem();
								if ( newCard != userCard ){
									if ( userCard != null  ){
										userCard.setUser(null);
										CardDAOImpl._instance.updateCard(userCard);
									}
									newCard.setUser(userUpdated);
									CardDAOImpl._instance.updateCard(newCard);
								}
								
							}
						}
					}
				});
				getRootPane().setDefaultButton(okButton);
			}
		}
	}
}
