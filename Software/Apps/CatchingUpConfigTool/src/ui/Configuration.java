package ui;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;
import java.awt.Point;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

import persistence.CardDAOImpl;
import persistence.LogsImpl;
import persistence.RecorderDAOImpl;
import persistence.RoomDAOImpl;
import persistence.UserRecorderDAOImpl;
import dao.CardDAO;
import dao.RecorderDAO;
import dao.RoomDAO;
import dao.UserRecorderDAO;
import dm.Card;
import dm.Recorder;
import dm.Room;
import dm.UserRecorder;

import javax.swing.JButton;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.BorderLayout;

public class Configuration extends JFrame {
	private JPanel contentPane;
	private JTable tableSpeakers;
	private JTable tableCards;
	private JTable tableModuls;
	private JTable tableClassrooms;
	private static JTextArea logsArea = null;
	private CardDAO cardDao= new CardDAOImpl();
	private UserRecorderDAO userRecorderDao= new UserRecorderDAOImpl();
	private RoomDAO roomDao = new RoomDAOImpl();
	private JTabbedPane tabbedPane ;

	private Model mRoom;
	private Model mCard;
	private Model mModul;
	private Model mSpeacker;



	private Thread refreshRecorderStates = new Thread( new Runnable() {
		@Override
		public void run() {
			while ( true ){
				try {
					Thread.sleep(5000);
					if ( tabbedPane.getSelectedIndex() == 4 && !RecorderCreation.isOpen )
					reloadRecorder();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				catch(Exception e)
				{
					e.printStackTrace();
				}
				

			}
		}
	});
	String[] connectingModuleColumnTitle = { "Id Enregistreur","Id Activateur", "Salle", "Statut", "Fichiers en attente" };

	/**
	 * Create the frame.
	 * @throws ParseException 
	 */
	public Configuration() throws ParseException {
		setTitle("Configuration");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 875, 499);
		contentPane = new JPanel();

		setContentPane(contentPane);

		tabbedPane = new JTabbedPane(JTabbedPane.TOP);
		tabbedPane.addChangeListener( new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent e) {
				switch (tabbedPane.getSelectedIndex()) {
				case 0 : // Logs
					//loadLogs();
					break;
				case 1:
					reloadSpeacker();
					break;
				case 2:
					reloadCard();
					break;
				case 3:
					reloadRoom();
					break;
				case 4:
					//reloadRecorder();
					break;
				default:
					break;
				}
			}
		});
		contentPane.setLayout(new BorderLayout(0, 0));



		// Logs
		tabbedPane.addTab("Logs", null, createLogPannel(), null);


		// Speackers Managment
		tabbedPane.addTab("Gestion des intervenants", null, createSpeackerPanel(), null);


		// Cards Managment
		tabbedPane.addTab("Gestion des cartes", null, createCardPanel(), null);

		// Rooms Managment
		tabbedPane.addTab("Gestion des salles", null, createRoomPanel(), null);

		// Recorder Manamgment
		tabbedPane.addTab("Gestion des enregistreurs", null, createRecorderPanel(), null);
		contentPane.add(tabbedPane);

		refreshRecorderStates.start();
	}


	/// Create Panel
	public JPanel createLogPannel(){
		JPanel logsPannel = new JPanel();
		logsArea = new JTextArea();
		logsArea.setEditable(false);

		JButton btnRafraichir = new JButton("Rafraichir");
		btnRafraichir.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				loadLogs();
			}
		});
		logsPannel.setLayout(new BorderLayout(0, 0));


		JScrollPane scrollPaneLog = new JScrollPane(logsArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		logsPannel.add(scrollPaneLog);
		logsPannel.add(btnRafraichir, BorderLayout.SOUTH);
		loadLogs();
		return logsPannel;
	}


	public JPanel createSpeackerPanel() throws ParseException{
		JPanel speakersManagementPanel = new JPanel();


		final List<UserRecorder> userRecorder = userRecorderDao.getUserRecorderList();


		String[] titreColonnes = {"Identifiant",  "Pr�nom","Nom", "Email","Date de d�but","Date de fin"}; 

		final Object[][] arrayUserRecorder = toArrayUserRecorder(userRecorder);
		mSpeacker = new Model(arrayUserRecorder,titreColonnes);

		JScrollPane scrollPaneUser = new JScrollPane();

		JButton createUserButton = new JButton("Nouvel utilisateur");
		createUserButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				new SpeakerCreation().setVisible(true);;
			}
		});
		speakersManagementPanel.setLayout(new BorderLayout(0, 0));



		tableSpeakers = new JTable(mSpeacker);
		scrollPaneUser.setViewportView(tableSpeakers);
		speakersManagementPanel.add(scrollPaneUser);
		speakersManagementPanel.add(createUserButton, BorderLayout.SOUTH);
		tableSpeakers.addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				JTable table =(JTable) e.getSource();
				Point p = e.getPoint();

				int row = table.rowAtPoint(p);
				if (e.getClickCount() == 2) {
					UserRecorder r = userRecorder.get(row);

					SpeakerUpdate speakerUpdate = new SpeakerUpdate( r );
					speakerUpdate.setVisible(true);
				}
			}

			public void mousePressed(MouseEvent e){
				JTable table =(JTable) e.getSource();
				Point p = e.getPoint();
				int row = table.rowAtPoint(p);
				if(table.isRowSelected(row)){
					if (e.isPopupTrigger())
						doPop(e);
				}
			}

			public void mouseReleased(MouseEvent e){
				if (e.isPopupTrigger())
					doPop(e);
			}

			private void doPop(MouseEvent e){
				PopUp menu = new PopUp();
				menu.show(e.getComponent(), e.getX(), e.getY());
			}
		});

		return speakersManagementPanel;
	}


	public JPanel createCardPanel(){
		JPanel cardsManagementPanel = new JPanel();
		String[] cardColumnTitle = { "Id","Utilisateur associé"};


		JButton btnNewCard = new JButton("Nouvelle carte");
		btnNewCard.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				new CardCreation().setVisible(true);
			}
		});



		JScrollPane scrollPaneCard = new JScrollPane(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);

		List<Card> card = cardDao.getCardList();

		mCard =  new Model(toArrayCard(card),cardColumnTitle);
		cardsManagementPanel.setLayout(new BorderLayout(0, 0));
		tableCards = new JTable( mCard );

		scrollPaneCard.setViewportView(tableCards);
		cardsManagementPanel.add(scrollPaneCard);
		cardsManagementPanel.add(btnNewCard, BorderLayout.SOUTH);

		return cardsManagementPanel;
	}


	public JPanel createRoomPanel(){
		JPanel classroomsManagementPanel = new JPanel();
		JScrollPane scrollPaneRoom = new JScrollPane();

		JButton btnRoom = new JButton("Nouvelle salle");
		btnRoom.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				new RoomCreation().setVisible(true);
			}
		});

		final List<Room> room = roomDao.getRoomList();
		String[] roomColumnTitle = {"nom","description"}; 
		final Object[][] arrayRoom = toArrayRoom(room);
		mRoom = new Model(arrayRoom,roomColumnTitle);
		classroomsManagementPanel.setLayout(new BorderLayout(0, 0));
		tableClassrooms= new JTable(mRoom);

		scrollPaneRoom.setViewportView(tableClassrooms);
		classroomsManagementPanel.add(scrollPaneRoom);
		classroomsManagementPanel.add(btnRoom, BorderLayout.SOUTH);
		tableClassrooms.addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				JTable table =(JTable) e.getSource();
				Point p = e.getPoint();
				int row = table.rowAtPoint(p);
				if (e.getClickCount() == 2) {
					Room updated = room.get(row);
					new RoomUpdate(updated).setVisible(true);
				}
			}
		});
		return classroomsManagementPanel;
	}


	public JPanel createRecorderPanel(){
		JPanel recordersManagementPanel = new JPanel();
		JScrollPane scrollPane_enregistreurs = new JScrollPane();

		JButton btnRecorder = new JButton("Nouvel enregistreur");
		btnRecorder.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				new RecorderCreation().setVisible(true);
			}
		});

		RecorderDAO connectingModuleDao = RecorderDAOImpl._instance;
		List<Recorder> recorders = connectingModuleDao.getRecorderList();

		mModul = new Model(toArrayModuls(recorders),connectingModuleColumnTitle);
		tableModuls = new JTable(mModul);


		tableModuls.addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				JTable table =(JTable) e.getSource();
				Point p = e.getPoint();
				int row = table.rowAtPoint(p);
				if (e.getClickCount() == 2) {
					Recorder rec =  RecorderDAOImpl._recorders.get(row);
					if ( rec != null )
						new Parring(rec);
				}
			}
		});
		recordersManagementPanel.setLayout(new BorderLayout(0, 0));

		scrollPane_enregistreurs.setViewportView(tableModuls);
		recordersManagementPanel.add(scrollPane_enregistreurs);
		recordersManagementPanel.add(btnRecorder, BorderLayout.SOUTH);

		return recordersManagementPanel;
	}




	/// Reload functions
	public void loadLogs(){
		logsArea.setText( LogsImpl._instance.getLogs() );
	}

	public void reloadRoom(){
		mRoom.donnees = toArrayRoom( roomDao.getRoomList() );
		mRoom.fireTableDataChanged();
	}

	public void reloadSpeacker(){
		try {
			mSpeacker.donnees = toArrayUserRecorder(userRecorderDao.getUserRecorderList());
		} catch (ParseException e) {
		}
		mSpeacker.fireTableDataChanged();
	}

	public void reloadCard(){
		mCard.donnees = toArrayCard( cardDao.getCardList() );
		mCard.fireTableDataChanged();	
	}

	public void reloadRecorder(){
		List<Recorder> recorders = RecorderDAOImpl._instance.getRecorderList();
		mModul.donnees = toArrayModuls(recorders);
		mModul.fireTableDataChanged();
	}



	///  List to Object[][]
	private Object[][] toArrayUserRecorder(List<UserRecorder> datas){
		Object[][] array = new Object[datas.size()][];
		for (int i = 0; i < datas.size(); i++) {
			ArrayList<String> row = new ArrayList<String>();
			row.add(""+datas.get(i).getId());
			row.add(datas.get(i).getFirstName());
			row.add(datas.get(i).getLastName());
			row.add(datas.get(i).getEmail());
			row.add(datas.get(i).getDateBegin().toString());
			row.add(datas.get(i).getDateEnd().toString());
			array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}

	private Object[][] toArrayCard(List<Card> datas){
		Object[][] array = new Object[datas.size()][];
		for (int i = 0; i < datas.size(); i++) {
			ArrayList<String> row = new ArrayList<String>();
			row.add(datas.get(i).getNumberCard());
			if(datas.get(i).getUser()!=null){
				row.add(datas.get(i).getUser().getFirstName() + " " + datas.get(i).getUser().getLastName() );
			}
			else{
				row.add("");
			}
			array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}

	private Object[][] toArrayRoom(List<Room> datas){
		Object[][] array = new Object[datas.size()][];
		for (int i = 0; i < datas.size(); i++) {
			ArrayList<String> row = new ArrayList<String>();
			row.add(datas.get(i).getName());
			row.add(datas.get(i).getDescription());
			array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}

	private Object[][] toArrayModuls(List<Recorder> recorders){
		Object[][] array = new Object[recorders.size()][];
		for (int i = 0; i < recorders.size(); i++) {
			ArrayList<String> row = new ArrayList<String>();
			row.add(recorders.get(i).getRecordingModule().getIdNetwork()+"");
			row.add(recorders.get(i).getConnectingModule().getIdNetworkRecording()+"");
			row.add(recorders.get(i).getRoom().getName() );
			row.add( recorders.get(i).toString() );
			row.add( recorders.get(i).getFilesInQueue() + "" );
			array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}
}
