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
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if ( tabbedPane.getSelectedIndex() == 4 && !RecorderCreation.isOpen )
					reloadRecorder();

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
		setBounds(100, 100, 825, 499);
		contentPane = new JPanel();

		setContentPane(contentPane);

		tabbedPane = new JTabbedPane(JTabbedPane.TOP);
		GroupLayout gl_contentPane = new GroupLayout(contentPane);
		gl_contentPane.setHorizontalGroup(
				gl_contentPane.createParallelGroup(Alignment.LEADING)
				.addGroup(Alignment.TRAILING, gl_contentPane.createSequentialGroup()
						.addContainerGap(132, Short.MAX_VALUE)
						.addComponent(tabbedPane, GroupLayout.PREFERRED_SIZE, 654, GroupLayout.PREFERRED_SIZE))
				);
		gl_contentPane.setVerticalGroup(
				gl_contentPane.createParallelGroup(Alignment.LEADING)
				.addGroup(Alignment.TRAILING, gl_contentPane.createSequentialGroup()
						.addContainerGap(25, Short.MAX_VALUE)
						.addComponent(tabbedPane, GroupLayout.PREFERRED_SIZE, 436, GroupLayout.PREFERRED_SIZE))
				);
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


		JScrollPane scrollPaneLog = new JScrollPane(logsArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);


		GroupLayout gl_logsPanel = new GroupLayout(logsPannel);
		gl_logsPanel.setHorizontalGroup(
				gl_logsPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_logsPanel.createSequentialGroup()
						.addGap(25)
						.addComponent(scrollPaneLog, GroupLayout.PREFERRED_SIZE, 800, GroupLayout.PREFERRED_SIZE)
						)
						.addComponent(btnRafraichir)
				);
		gl_logsPanel.setVerticalGroup(
				gl_logsPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_logsPanel.createSequentialGroup()
						.addComponent(scrollPaneLog, GroupLayout.PREFERRED_SIZE, 400, GroupLayout.PREFERRED_SIZE)
						.addComponent(btnRafraichir)
						)
				);
		logsPannel.setLayout(gl_logsPanel);
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

		GroupLayout gl_speakersManagementPanel = new GroupLayout(speakersManagementPanel);
		gl_speakersManagementPanel.setHorizontalGroup(
				gl_speakersManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_speakersManagementPanel.createSequentialGroup()
						.addGap(25)
						.addComponent(scrollPaneUser, GroupLayout.PREFERRED_SIZE, 800, GroupLayout.PREFERRED_SIZE)
						)
						.addComponent(createUserButton)
				);
		gl_speakersManagementPanel.setVerticalGroup(
				gl_speakersManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_speakersManagementPanel.createSequentialGroup()
						.addComponent(scrollPaneUser, GroupLayout.PREFERRED_SIZE, 400, GroupLayout.PREFERRED_SIZE)
						.addComponent(createUserButton)
						)
				);



		tableSpeakers = new JTable(mSpeacker);
		scrollPaneUser.setViewportView(tableSpeakers);
		speakersManagementPanel.setLayout(gl_speakersManagementPanel);
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


		GroupLayout gl_cardsManagementPanel = new GroupLayout(cardsManagementPanel);
		gl_cardsManagementPanel.setHorizontalGroup(
				gl_cardsManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_cardsManagementPanel.createSequentialGroup()
						.addGap(25)
						.addComponent(scrollPaneCard, GroupLayout.PREFERRED_SIZE, 800, GroupLayout.PREFERRED_SIZE)
						)
						.addComponent(btnNewCard)
				);
		gl_cardsManagementPanel.setVerticalGroup(
				gl_cardsManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_cardsManagementPanel.createSequentialGroup()
						.addComponent(scrollPaneCard, GroupLayout.PREFERRED_SIZE, 400, GroupLayout.PREFERRED_SIZE)
						.addComponent(btnNewCard)
						)
				);

		List<Card> card = cardDao.getCardList();

		mCard =  new Model(toArrayCard(card),cardColumnTitle);
		tableCards = new JTable( mCard );

		scrollPaneCard.setViewportView(tableCards);
		cardsManagementPanel.setLayout(gl_cardsManagementPanel);

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

		GroupLayout gl_roomManagementPanel = new GroupLayout(classroomsManagementPanel);
		gl_roomManagementPanel.setHorizontalGroup(
				gl_roomManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_roomManagementPanel.createSequentialGroup()
						.addGap(25)
						.addComponent(scrollPaneRoom, GroupLayout.PREFERRED_SIZE, 800, GroupLayout.PREFERRED_SIZE)
						)
						.addComponent(btnRoom)
				);
		gl_roomManagementPanel.setVerticalGroup(
				gl_roomManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_roomManagementPanel.createSequentialGroup()
						.addComponent(scrollPaneRoom, GroupLayout.PREFERRED_SIZE, 400, GroupLayout.PREFERRED_SIZE)
						.addComponent(btnRoom)
						)
				);

		final List<Room> room = roomDao.getRoomList();
		String[] roomColumnTitle = {"nom","description"}; 
		final Object[][] arrayRoom = toArrayRoom(room);
		mRoom = new Model(arrayRoom,roomColumnTitle);
		tableClassrooms= new JTable(mRoom);

		scrollPaneRoom.setViewportView(tableClassrooms);
		classroomsManagementPanel.setLayout(gl_roomManagementPanel);
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


		GroupLayout gl_roomManagementPanel = new GroupLayout(recordersManagementPanel);
		gl_roomManagementPanel.setHorizontalGroup(
				gl_roomManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_roomManagementPanel.createSequentialGroup()
						.addGap(25)
						.addComponent(scrollPane_enregistreurs, GroupLayout.PREFERRED_SIZE, 800, GroupLayout.PREFERRED_SIZE)
						)
						.addComponent(btnRecorder)
				);
		gl_roomManagementPanel.setVerticalGroup(
				gl_roomManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_roomManagementPanel.createSequentialGroup()
						.addComponent(scrollPane_enregistreurs, GroupLayout.PREFERRED_SIZE, 400, GroupLayout.PREFERRED_SIZE)
						.addComponent(btnRecorder)
						)
				);

		RecorderDAO connectingModuleDao = new RecorderDAOImpl();
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

		scrollPane_enregistreurs.setViewportView(tableModuls);
		recordersManagementPanel.setLayout(gl_roomManagementPanel);

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
