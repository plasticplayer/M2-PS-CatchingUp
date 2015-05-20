package ui;

import java.awt.BorderLayout;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JTextArea;

public class RoomCreation extends JDialog {
	private String name;
	private String description;

	private final JPanel contentPanel = new JPanel();
	private JTextField tbxName;
	private JTextArea tbxDescription;
	private JButton cancelButton;
	private JButton okButton;

	/**
	 * Create the dialog.
	 */
	public RoomCreation() {
		this.name = name;
		this.description = description;
		setTitle("Creation salle");
		setBounds(100, 100, 450, 300);
		getContentPane().setLayout(new BorderLayout());
		
		JPanel namePanel = new JPanel();
		getContentPane().add(namePanel, BorderLayout.NORTH);
		
		JLabel lblName = new JLabel("Nom :");
		namePanel.add(lblName);
		
		tbxName = new JTextField("");
		namePanel.add(tbxName);
		tbxName.setColumns(10);
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		tbxDescription = new JTextArea("");
		tbxDescription.setColumns(250);
		JLabel lblDescription = new JLabel("Description :");
		GroupLayout gl_contentPanel = new GroupLayout(contentPanel);
		gl_contentPanel.setHorizontalGroup(
			gl_contentPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(Alignment.LEADING, gl_contentPanel.createSequentialGroup()
					.addContainerGap()
					.addComponent(lblDescription, GroupLayout.PREFERRED_SIZE, 70, GroupLayout.PREFERRED_SIZE)
					.addGap(18)
					.addComponent(tbxDescription, GroupLayout.PREFERRED_SIZE, 295, GroupLayout.PREFERRED_SIZE)
					.addContainerGap(31, Short.MAX_VALUE))
		);
		gl_contentPanel.setVerticalGroup(
			gl_contentPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_contentPanel.createSequentialGroup()
					.addContainerGap()
					.addGroup(gl_contentPanel.createParallelGroup(Alignment.BASELINE)
						.addComponent(tbxDescription, GroupLayout.DEFAULT_SIZE, 110, Short.MAX_VALUE)
						.addComponent(lblDescription))
					.addGap(29))
		);
		contentPanel.setLayout(gl_contentPanel);
		{
			JPanel buttonPanel = new JPanel();
			getContentPane().add(buttonPanel, BorderLayout.SOUTH);
			{
				cancelButton = new JButton("Cancel");
				cancelButton.setActionCommand("Cancel");
			}
			{
				okButton = new JButton("OK");
				okButton.setActionCommand("OK");
				getRootPane().setDefaultButton(okButton);
			}
			GroupLayout gl_buttonPanel = new GroupLayout(buttonPanel);
			gl_buttonPanel.setHorizontalGroup(
				gl_buttonPanel.createParallelGroup(Alignment.LEADING)
					.addGroup(gl_buttonPanel.createSequentialGroup()
						.addGap(145)
						.addComponent(cancelButton)
						.addGap(18)
						.addComponent(okButton)
						.addGap(159))
			);
			gl_buttonPanel.setVerticalGroup(
				gl_buttonPanel.createParallelGroup(Alignment.LEADING)
					.addGroup(gl_buttonPanel.createParallelGroup(Alignment.BASELINE)
						.addComponent(cancelButton)
						.addComponent(okButton))
			);
			buttonPanel.setLayout(gl_buttonPanel);
		}
	}
}
