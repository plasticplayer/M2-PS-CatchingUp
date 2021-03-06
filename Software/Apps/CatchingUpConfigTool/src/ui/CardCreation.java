package ui;

import java.awt.BorderLayout;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;
import javax.swing.text.html.HTMLDocument.HTMLReader.IsindexAction;

import org.omg.CORBA.ORB;
import org.omg.CORBA.Object;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.Servant;
import org.omg.PortableServer.portable.Delegate;

import com.sun.corba.se.impl.legacy.connection.USLPort;

import communication.CardReader;
import communication.Tools;
import persistence.CardDAOImpl;
import dm.Card;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

@SuppressWarnings("serial")
public class CardCreation extends JDialog {

	private final JPanel contentPanel = new JPanel();
	private static boolean isOpen = true;
	private JTextField txtIdCard;
	private Thread t1 ;
	/**
	 * Create the dialog.
	 */
	public CardCreation() {
		addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
				isOpen = false;
				if(t1.isAlive())
				{
					Tools.LOGGER_DEBUG("Stop T1");
					t1.stop();
				}
				
				super.windowClosing(e);
			}
		});
		setTitle("Ajout d'une carte");
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		setBounds(100, 100, 335, 191);
		getContentPane().setLayout(new BorderLayout());

		JPanel panel = new JPanel();
		getContentPane().add(panel, BorderLayout.SOUTH);

		JButton btnOk = new JButton("OK");
		btnOk.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if (txtIdCard.getText().isEmpty())
					return;
				try {
					
					String idC = (Long.toString(
							Long.valueOf(txtIdCard.getText().replace(" ", "").replace("0x", ""),16)
									)
									);
					Card c = new Card(idC, null);
					if (CardDAOImpl._instance.createCard(c))
						
						dispose();
				} catch (Exception e) {

				}

			}
		});

		JButton btnAnnulation = new JButton("Annuler");
		btnAnnulation.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				dispose();
			}
		});
		panel.add(btnAnnulation);
		panel.add(btnOk);
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
		if(t1 == null || !t1.isAlive())
		{
			t1= new Thread(new Runnable() {
				public void run() {
					waitCard();
				}
			});
			t1.start();
		}
		CardReader.newValue = false;
	}

	public void waitCard() {
		if (CardReader.openPort() == false) {
			txtIdCard.setEditable(true);
			return;
		}
		while (isOpen && !CardReader.newValue) {
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		// CardReader.close();
		CardReader.newValue = false;
		if (isOpen)
			txtIdCard.setText("0x"
					+ Long.toHexString(Long.parseLong(CardReader.s))
							.toUpperCase());
	}
}