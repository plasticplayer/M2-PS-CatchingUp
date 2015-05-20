package ui;

import java.awt.BorderLayout;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;

@SuppressWarnings("serial")
public class CardCreation extends JDialog {

	private final JPanel contentPanel = new JPanel();
	private JTextField txtIdCard;

	/**
	 * Create the dialog.
	 */
	public CardCreation() {
		setTitle("Ajout d'une carte");
		
		setBounds(100, 100, 335, 191);
		getContentPane().setLayout(new BorderLayout());
		
		JPanel panel = new JPanel();
		getContentPane().add(panel, BorderLayout.SOUTH);
		
		JButton btnOk = new JButton("OK");
		panel.add(btnOk);
		
		JButton btnAnnulation = new JButton("Annuler");
		panel.add(btnAnnulation);
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		contentPanel.setLayout(new BorderLayout(0, 0));
		
		JPanel panelTop = new JPanel();
		contentPanel.add(panelTop, BorderLayout.NORTH);
		
		JLabel lblAction = new JLabel("Passez votre carte devant le lecteur");
		panelTop.add(lblAction);
		
		JPanel panelCenter = new JPanel();
		contentPanel.add(panelCenter);
		
		JLabel lblIdCard = new JLabel("Id de la carte: ");
		panelCenter.add(lblIdCard);
		
		txtIdCard = new JTextField();
		panelCenter.add(txtIdCard);
		txtIdCard.setToolTipText("");
		txtIdCard.setEditable(false);
		txtIdCard.setColumns(10);
		
	}
}