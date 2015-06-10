package ui;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;
import java.awt.Point;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.table.JTableHeader;
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

import javax.swing.LayoutStyle.ComponentPlacement;
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
		JPanel logsPannel = new JPanel();
		tabbedPane.addTab("Logs", null, logsPannel, null);
		
		JTextArea textArea = new JTextArea();
		textArea.setText( LogsImpl._instance.getLogs() );
		textArea.setEditable(false);
		
		JScrollPane scrollPaneLog = new JScrollPane(textArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		
		
		GroupLayout gl_logsPanel = new GroupLayout(logsPannel);
		gl_logsPanel.setHorizontalGroup(
				gl_logsPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_logsPanel.createSequentialGroup()
					.addGap(25)
					.addComponent(scrollPaneLog, GroupLayout.PREFERRED_SIZE, 800, GroupLayout.PREFERRED_SIZE)
					)
		);
		gl_logsPanel.setVerticalGroup(
				gl_logsPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_logsPanel.createSequentialGroup()
					.addComponent(scrollPaneLog, GroupLayout.PREFERRED_SIZE, 400, GroupLayout.PREFERRED_SIZE)
					.addPreferredGap(ComponentPlacement.RELATED, 41, Short.MAX_VALUE)
					)
		);
		logsPannel.setLayout(gl_logsPanel);
		
		
		
		
		
		
		/*JPanel cameraManagementPanel = new JPanel();
		tabbedPane.addTab("Calibrage cam�ra", null, cameraManagementPanel, null);*/
		
		JPanel speakersManagementPanel = new JPanel();
		tabbedPane.addTab("Gestion des intervenants", null, speakersManagementPanel, null);
		
		UserRecorderDAO userRecorderDao= new UserRecorderDAOImpl();
		final List<UserRecorder> userRecorder = userRecorderDao.getUserRecorderList();
		
		
		String[] titreColonnes = {"Identifiant", 
				   "Pr�nom","Nom",
				   "Email","Date de d�but","Date de fin"}; 
		
		final Object[][] arrayUserRecorder = toArrayUserRecorder(userRecorder);
		Model m = new Model(arrayUserRecorder,titreColonnes);
		
		JScrollPane scrollPaneUser = new JScrollPane();
		
		JButton UserButton = new JButton("Nouvel utilisateur");
		UserButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				SpeakerCreation spCreationSpeaker = new SpeakerCreation();
				spCreationSpeaker.show();
			}
		});
				
		GroupLayout gl_speakersManagementPanel = new GroupLayout(speakersManagementPanel);
		gl_speakersManagementPanel.setHorizontalGroup(
			gl_speakersManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_speakersManagementPanel.createSequentialGroup()
					.addGap(39)
					.addComponent(scrollPaneUser, GroupLayout.DEFAULT_SIZE, 650, Short.MAX_VALUE)
					.addGap(46))
				.addGroup(gl_speakersManagementPanel.createSequentialGroup()
					.addGap(89)
					.addComponent(UserButton)
					.addContainerGap(557, Short.MAX_VALUE))
		);
		gl_speakersManagementPanel.setVerticalGroup(
			gl_speakersManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_speakersManagementPanel.createSequentialGroup()
					.addGap(68)
					.addComponent(scrollPaneUser, GroupLayout.PREFERRED_SIZE, 241, GroupLayout.PREFERRED_SIZE)
					.addPreferredGap(ComponentPlacement.RELATED, 64, Short.MAX_VALUE)
					.addComponent(UserButton)
					.addGap(31))
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
		//tableSpeakers.addMouseListener(new PopClickListener());
		
		JPanel cardsManagementPanel = new JPanel();
		tabbedPane.addTab("Gestion des cartes", null, cardsManagementPanel, null);
		
		JScrollPane scrollPane = new JScrollPane();	
		JButton btnNewCard = new JButton("Nouvelle carte");
		btnNewCard.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
						CardCreation spCreationCard = new CardCreation();
						spCreationCard.show();
			}
		});
		GroupLayout gl_cardsManagementPanel = new GroupLayout(cardsManagementPanel);
		gl_cardsManagementPanel.setHorizontalGroup(
			gl_cardsManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_cardsManagementPanel.createSequentialGroup()
					.addContainerGap(193, Short.MAX_VALUE)
					.addComponent(scrollPane, GroupLayout.PREFERRED_SIZE, 384, GroupLayout.PREFERRED_SIZE)
					.addGap(184))
				.addGroup(Alignment.LEADING, gl_cardsManagementPanel.createSequentialGroup()
					.addGap(98)
					.addComponent(btnNewCard)
					.addContainerGap(574, Short.MAX_VALUE))
		);
		gl_cardsManagementPanel.setVerticalGroup(
			gl_cardsManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_cardsManagementPanel.createSequentialGroup()
					.addGap(76)
					.addComponent(scrollPane, GroupLayout.PREFERRED_SIZE, 229, GroupLayout.PREFERRED_SIZE)
					.addPreferredGap(ComponentPlacement.RELATED, 52, Short.MAX_VALUE)
					.addComponent(btnNewCard)
					.addGap(48))
		);
		
		CardDAO cardDao= new CardDAOImpl();
		List<Card> card = cardDao.getCardList();
		String[] cardColumnTitle = { 
				   "Id","modele","Utilisateur"}; 
		Model mCard = new Model(toArrayCard(card),cardColumnTitle);
		tableCards = new JTable(mCard);
	//	JTableHeader headerCard = tableCards.getTableHeader();
	
		scrollPane.setViewportView(tableCards);
		cardsManagementPanel.setLayout(gl_cardsManagementPanel);
		
		JPanel classroomsManagementPanel = new JPanel();
		tabbedPane.addTab("Gestion des salles", null, classroomsManagementPanel, null);
		
		JScrollPane scrollPaneRoom = new JScrollPane();
		
		JButton btnRoom = new JButton("Nouvelle salle");
		btnRoom.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				RoomCreation spCreationRoom = new RoomCreation();
				spCreationRoom.show();
			}
		});
		GroupLayout gl_classroomsManagementPanel = new GroupLayout(classroomsManagementPanel);
		gl_classroomsManagementPanel.setHorizontalGroup(
			gl_classroomsManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_classroomsManagementPanel.createSequentialGroup()
					.addContainerGap(219, Short.MAX_VALUE)
					.addComponent(scrollPaneRoom, GroupLayout.PREFERRED_SIZE, 409, GroupLayout.PREFERRED_SIZE)
					.addGap(145))
				.addGroup(Alignment.LEADING, gl_classroomsManagementPanel.createSequentialGroup()
					.addGap(92)
					.addComponent(btnRoom)
					.addContainerGap(592, Short.MAX_VALUE))
		);
		gl_classroomsManagementPanel.setVerticalGroup(
			gl_classroomsManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_classroomsManagementPanel.createSequentialGroup()
					.addGap(75)
					.addComponent(scrollPaneRoom, GroupLayout.PREFERRED_SIZE, 234, GroupLayout.PREFERRED_SIZE)
					.addPreferredGap(ComponentPlacement.RELATED, 49, Short.MAX_VALUE)
					.addComponent(btnRoom)
					.addGap(47))
		);
		
		RoomDAO roomDao= new RoomDAOImpl();
		final List<Room> room = roomDao.getRoomList();
		String[] roomColumnTitle = {"nom","description"}; 
		final Object[][] arrayRoom = toArrayRoom(room);
		Model mRoom = new Model(arrayRoom,roomColumnTitle);
		tableClassrooms= new JTable(mRoom);
		//JTableHeader headerRoom = tableClassrooms.getTableHeader();
		//scrollPane_1.setColumnHeaderView(tableClassrooms);
		scrollPaneRoom.setViewportView(tableClassrooms);   // affiche titres
		classroomsManagementPanel.setLayout(gl_classroomsManagementPanel);
		tableClassrooms.addMouseListener(new MouseAdapter() {
		    public void mouseClicked(MouseEvent e) {
		        JTable table =(JTable) e.getSource();
		        Point p = e.getPoint();
		        int row = table.rowAtPoint(p);
		        if (e.getClickCount() == 2) {
		            // your valueChanged overridden method 
		        	Room updated = room.get(row);
		        	//RoomUpdate roomUpdate = new RoomUpdate((String)arrayRoom[row][0],(String)arrayRoom[row][1]);
		        	RoomUpdate roomUpdate = new RoomUpdate(updated);
		        	roomUpdate.setVisible(true);
		        }
		    }
		});
			
		JPanel recordersManagementPanel = new JPanel();
		tabbedPane.addTab("Gestion des enregistreurs", null, recordersManagementPanel, null);
		
		JScrollPane scrollPane_enregistreurs = new JScrollPane();
		
		JButton btnCard = new JButton("Nouvel enregistreur");
		btnCard.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				new RecorderCreation().setVisible(true);
			}
		});
		GroupLayout gl_recordersManagementPanel = new GroupLayout(recordersManagementPanel);
		gl_recordersManagementPanel.setHorizontalGroup(
			gl_recordersManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_recordersManagementPanel.createSequentialGroup()
					.addContainerGap(257, Short.MAX_VALUE)
					.addComponent(scrollPane_enregistreurs, GroupLayout.PREFERRED_SIZE, 347, GroupLayout.PREFERRED_SIZE)
					.addGap(177))
				.addGroup(Alignment.LEADING, gl_recordersManagementPanel.createSequentialGroup()
					.addGap(90)
					.addComponent(btnCard)
					.addContainerGap(602, Short.MAX_VALUE))
		);
		gl_recordersManagementPanel.setVerticalGroup(
			gl_recordersManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_recordersManagementPanel.createSequentialGroup()
					.addGap(96)
					.addComponent(scrollPane_enregistreurs, GroupLayout.PREFERRED_SIZE, 229, GroupLayout.PREFERRED_SIZE)
					.addPreferredGap(ComponentPlacement.RELATED, 41, Short.MAX_VALUE)
					.addComponent(btnCard)
					.addGap(39))
		);
		
		RecorderDAO connectingModuleDao = new RecorderDAOImpl();
		List<Recorder> recorders = connectingModuleDao.getRecorderList();
		String[] connectingModuleColumnTitle = { "Id Enregistreur","Id Activateur", "Salle", "Statut" }; 
		Model mModul = new Model(toArrayModuls(recorders),connectingModuleColumnTitle);
		tableModuls = new JTable(mModul);
		
		tableModuls.addMouseListener(new MouseAdapter() {
		    public void mouseClicked(MouseEvent e) {
		        JTable table =(JTable) e.getSource();
		        Point p = e.getPoint();
		        int row = table.rowAtPoint(p);
		        if (e.getClickCount() == 2) {
		           Recorder rec =  RecorderDAOImpl._instance._recorders.get(row);
		           if ( rec != null )
		        	   new Parring(rec);
		        }
		    }
		});
	
		scrollPane_enregistreurs.setViewportView(tableModuls);
		recordersManagementPanel.setLayout(gl_recordersManagementPanel);
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
