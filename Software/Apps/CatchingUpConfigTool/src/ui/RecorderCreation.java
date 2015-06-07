
package ui;

import java.awt.BorderLayout;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;

import persistence.RecorderDAOImpl;
import persistence.RoomDAOImpl;
import dm.ConnectingModule;
import dm.Recorder;
import dm.RecordingModule;
import dm.Room;

import java.awt.Choice;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

@SuppressWarnings("serial")
public class RecorderCreation extends JDialog {

	private final JPanel contentPanel = new JPanel();

	/**
	 * Create the dialog.
	 */
	public RecorderCreation() {
		setTitle("Ajout d'un enregistreur");
		
		setBounds(100, 100, 435, 191);
		getContentPane().setLayout(new BorderLayout());
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		contentPanel.setLayout(new BorderLayout(0, 0));
		
		JPanel centerPanel = new JPanel();
		contentPanel.add(centerPanel, BorderLayout.NORTH);
		
		JLabel lblIdCard = new JLabel("Enregistreurs disponibles : ");
		centerPanel.add(lblIdCard);
		
		final Choice choice = new Choice();
		centerPanel.add(choice);
		for ( Recorder rec : RecorderDAOImpl._unconnectedRecorders) {
			choice.add("" + rec.getRecordingModule().getAdressMac() );
		}
		
		JPanel panelTop = new JPanel();
		contentPanel.add(panelTop, BorderLayout.CENTER);
		
		JLabel lblAction = new JLabel("Associ\u00E9 \u00E0 la salle :");
		panelTop.add(lblAction);
		
		final Choice choice_1 = new Choice();
		panelTop.add(choice_1);
		for ( Room room : RoomDAOImpl._freeRooms )
			choice_1.add(room.getName());
		
		
		JPanel buttonPanel = new JPanel();
		contentPanel.add(buttonPanel, BorderLayout.SOUTH);
		
		JButton btnAnnulation = new JButton("Annuler");
		btnAnnulation.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				dispose();
			}
		});
		buttonPanel.add(btnAnnulation);
		
		JButton btnOk = new JButton("OK");
		btnOk.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				Recorder rec = RecorderDAOImpl._unconnectedRecorders.get( choice.getSelectedIndex() );
				Room r = RoomDAOImpl._freeRooms.get( choice_1.getSelectedIndex() );
				rec.setRoom(r);
				if ( RecorderDAOImpl._instance.createRecorder(rec) )
					dispose();
			}
		});
		buttonPanel.add(btnOk);
		
		if ( choice_1.getItemCount() == 0 || choice.getItemCount() == 0 )
			btnOk.setEnabled(false);
			
	}
}