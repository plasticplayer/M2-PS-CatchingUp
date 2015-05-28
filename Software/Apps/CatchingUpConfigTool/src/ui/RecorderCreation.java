
package ui;

import java.awt.BorderLayout;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import java.awt.Choice;

@SuppressWarnings("serial")
public class RecorderCreation extends JDialog {

	private final JPanel contentPanel = new JPanel();

	/**
	 * Create the dialog.
	 */
	public RecorderCreation() {
		setTitle("Ajout d'un enregistreur");
		
		setBounds(100, 100, 335, 191);
		getContentPane().setLayout(new BorderLayout());
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		contentPanel.setLayout(new BorderLayout(0, 0));
		
		JPanel centerPanel = new JPanel();
		contentPanel.add(centerPanel, BorderLayout.NORTH);
		
		JLabel lblIdCard = new JLabel("Enregistreurs disponibles : ");
		centerPanel.add(lblIdCard);
		
		Choice choice = new Choice();
		centerPanel.add(choice);
		
		JPanel panelTop = new JPanel();
		contentPanel.add(panelTop, BorderLayout.CENTER);
		
		JLabel lblAction = new JLabel("Associ\u00E9 \u00E0 la salle :");
		panelTop.add(lblAction);
		
		Choice choice_1 = new Choice();
		panelTop.add(choice_1);
		
		JPanel buttonPanel = new JPanel();
		contentPanel.add(buttonPanel, BorderLayout.SOUTH);
		
		JButton btnOk = new JButton("OK");
		buttonPanel.add(btnOk);
		
		JButton btnAnnulation = new JButton("Annuler");
		buttonPanel.add(btnAnnulation);
		
	}
}