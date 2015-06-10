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

	/**
	 * Create the frame.
	 * @throws ParseException 
	 */
	public Configuration() throws ParseException {
		setTitle("Configuration");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 825, 499);
		contentPane = new JPanel();
		//contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		
		JTabbedPane tabbedPane = new JTabbedPane(JTabbedPane.TOP);
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
		
		
		
		
		// Logs
		tabbedPane.addTab("Logs", null, createLogPannel(), null);
		
		
		// Speackers Managment
		tabbedPane.addTab("Gestion des intervenants", null, createIntervenantPanel(), null);
		
		
		// Cards Managment
		tabbedPane.addTab("Gestion des cartes", null, createCardPanel(), null);
		
		// Rooms Managment
		tabbedPane.addTab("Gestion des salles", null, createRoomPanel(), null);
		
		// Recorder Manamgment
		tabbedPane.addTab("Gestion des enregistreurs", null, createRecorderPanel(), null);
		
	}
	
	
	
	public JPanel createLogPannel(){
		JPanel logsPannel = new JPanel();
		final JTextArea textArea = new JTextArea();
		textArea.setText( LogsImpl._instance.getLogs() );
		textArea.setEditable(false);
		
		JButton btnRafraichir = new JButton("Rafraichir");
		btnRafraichir.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				textArea.setText( LogsImpl._instance.getLogs() );
			}
		});
		
		
		JScrollPane scrollPaneLog = new JScrollPane(textArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		
		
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
		return logsPannel;
	}

	
	public JPanel createIntervenantPanel() throws ParseException{
		JPanel speakersManagementPanel = new JPanel();
		
		UserRecorderDAO userRecorderDao= new UserRecorderDAOImpl();
		final List<UserRecorder> userRecorder = userRecorderDao.getUserRecorderList();
		
		
		String[] titreColonnes = {"Identifiant",  "Pr�nom","Nom", "Email","Date de d�but","Date de fin"}; 
		
		final Object[][] arrayUserRecorder = toArrayUserRecorder(userRecorder);
		Model m = new Model(arrayUserRecorder,titreColonnes);
		
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
		
		
		
		tableSpeakers = new JTable(m);
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
		
		
		JButton btnNewCard = new JButton("Nouvelle carte");
		btnNewCard.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
					new CardCreation().setVisible(true);
			}
		});
		
		CardDAO cardDao= new CardDAOImpl();
		
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
		String[] cardColumnTitle = { "Id","modele","Utilisateur"};
		
		tableCards = new JTable( new Model(toArrayCard(card),cardColumnTitle) );
		
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
		
		RoomDAO roomDao= new RoomDAOImpl();
		final List<Room> room = roomDao.getRoomList();
		String[] roomColumnTitle = {"nom","description"}; 
		final Object[][] arrayRoom = toArrayRoom(room);
		Model mRoom = new Model(arrayRoom,roomColumnTitle);
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
		String[] connectingModuleColumnTitle = { "Id Enregistreur","Id Activateur", "Salle", "Statut" }; 
		

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
		
		Model mModul = new Model(toArrayModuls(recorders),connectingModuleColumnTitle);
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
	
	
	
	
	private Object[][] toArrayUserRecorder(List<UserRecorder> datas){
		Object[][] array = new Object[datas.size()][];
		for (int i = 0; i < datas.size(); i++) {
		    ArrayList<String> row = new ArrayList<String>();
		    row.add(""+datas.get(i).getId());
		    row.add(datas.get(i).getFirstName());
		    row.add(datas.get(i).getLastName());
		   // row.add(datas.get(i).getPassword());
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
		    	row.add(datas.get(i).getUser().getFirstName());
		    	row.add(datas.get(i).getUser().getLastName());
		    }
		    else{
		    	row.add("");
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
		    array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}
}
