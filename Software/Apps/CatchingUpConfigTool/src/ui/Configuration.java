package ui;

import java.awt.BorderLayout;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.Point;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.table.JTableHeader;
import javax.swing.UIManager;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.JToggleButton;
import javax.swing.JTabbedPane;
import javax.swing.JList;
import javax.swing.JTable;
import javax.swing.JScrollPane;
import javax.swing.table.DefaultTableModel;

import persistence.CardDAOImpl;
import persistence.RoomDAOImpl;
import persistence.UserRecorderDAOImpl;
import dao.CardDAO;
import dao.RoomDAO;
import dao.UserRecorderDAO;
import dm.Card;
import dm.Room;
import dm.User;
import dm.UserRecorder;

import javax.swing.LayoutStyle.ComponentPlacement;

public class Configuration extends JFrame {

	private JPanel contentPane;
	private JTable tableSpeakers;
	private JTable tableCards;
	private JTable tableClassrooms;
	//public 

	/*/**
	 * Launch the application.
	 */
	/*public static void main(String[] args) {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Throwable e) {
			e.printStackTrace();
		}
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					Configuration frame = new Configuration();
					frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}*/

	/**
	 * Create the frame.
	 * @throws ParseException 
	 */
	public Configuration() throws ParseException {
		setTitle("Configuration");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 766, 504);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		
		JTabbedPane tabbedPane = new JTabbedPane(JTabbedPane.TOP);
		GroupLayout gl_contentPane = new GroupLayout(contentPane);
		gl_contentPane.setHorizontalGroup(
			gl_contentPane.createParallelGroup(Alignment.LEADING)
				.addComponent(tabbedPane, GroupLayout.DEFAULT_SIZE, 740, Short.MAX_VALUE)
		);
		gl_contentPane.setVerticalGroup(
			gl_contentPane.createParallelGroup(Alignment.LEADING)
				.addComponent(tabbedPane, Alignment.TRAILING, GroupLayout.DEFAULT_SIZE, 252, Short.MAX_VALUE)
		);
		
		JPanel cameraManagementPanel = new JPanel();
		tabbedPane.addTab("Calibrage cam�ra", null, cameraManagementPanel, null);
		
		JPanel speakersManagementPanel = new JPanel();
		tabbedPane.addTab("Gestion des intervenants", null, speakersManagementPanel, null);
		
		UserRecorderDAO userRecorderDao= new UserRecorderDAOImpl();
		List<UserRecorder> userRecorder = userRecorderDao.getUserRecorderList();
		//UserRecorder test [] = (UserRecorder[])(userRecorder.toArray());
		
		
		
		/*Object[][] donnees = {  
				   {"Swing", "Astral", "standard", 
				      Color.red, Boolean.TRUE}, 
				   {"Swing", "Mistral", "standard", 
				      Color.yellow, Boolean.FALSE}, 
				   {"Gin", "Oasis", "standard", 
				      Color.blue, Boolean.FALSE},
				   {"Gin", "boomerang", "comp�tition", 
				      Color.green, Boolean.TRUE},
				   {"Advance", "Omega", "performance", 
				      Color.cyan, Boolean.TRUE}, 
				} ;
				String[] titreColonnes = { 
				   "marque","modele","homologation",
				   "couleur","ok",}; 
				Model m = new Model(donnees,titreColonnes);
				tableSpeakers = new JTable(m);
			
			/*boolean[] columnEditables = new boolean[] {
				false, false, false, false, false
			};
			public boolean isCellEditable(int row, int column) {
				return columnEditables[column];
			}
		});*/
		//String firstName, String lastName, String password, String email, Date dateBegin, Date dateEnd
		String[] titreColonnes = { 
				   "pr�nom","nom","mot de passe",
				   "email","Date de d�but","date de fin"}; 
		final Object[][] arrayUserRecorder = toArrayUserRecorder(userRecorder);
		Model m = new Model(arrayUserRecorder,titreColonnes);
		
		JScrollPane scrollPane_2 = new JScrollPane();
				
		GroupLayout gl_speakersManagementPanel = new GroupLayout(speakersManagementPanel);
		gl_speakersManagementPanel.setHorizontalGroup(
			gl_speakersManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(Alignment.LEADING, gl_speakersManagementPanel.createSequentialGroup()
					.addGap(39)
					.addComponent(scrollPane_2, GroupLayout.DEFAULT_SIZE, 650, Short.MAX_VALUE)
					.addGap(46))
		);
		gl_speakersManagementPanel.setVerticalGroup(
			gl_speakersManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_speakersManagementPanel.createSequentialGroup()
					.addGap(68)
					.addComponent(scrollPane_2, GroupLayout.PREFERRED_SIZE, 241, GroupLayout.PREFERRED_SIZE)
					.addContainerGap(119, Short.MAX_VALUE))
		);
		tableSpeakers = new JTable(m);
		scrollPane_2.setViewportView(tableSpeakers);
		JTableHeader header = tableSpeakers.getTableHeader();
		speakersManagementPanel.setLayout(gl_speakersManagementPanel);
		tableSpeakers.addMouseListener(new MouseAdapter() {
		    boolean itemClicked=false;
		    public void mouseClicked(MouseEvent e) {
		        JTable table =(JTable) e.getSource();
		        Point p = e.getPoint();
		       
		        int row = table.rowAtPoint(p);
		        if (e.getClickCount() == 2) {
		            // your valueChanged overridden method 
		        	SpeakerUpdate speakerUpdate = new SpeakerUpdate((String)arrayUserRecorder[row][0],(String)arrayUserRecorder[row][1],(String)arrayUserRecorder[row][2],(String)arrayUserRecorder[row][3],
		        			(String)arrayUserRecorder[row][4],(String)arrayUserRecorder[row][5]);
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
		GroupLayout gl_cardsManagementPanel = new GroupLayout(cardsManagementPanel);
		gl_cardsManagementPanel.setHorizontalGroup(
			gl_cardsManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(Alignment.TRAILING, gl_cardsManagementPanel.createSequentialGroup()
					.addContainerGap(211, Short.MAX_VALUE)
					.addComponent(scrollPane, GroupLayout.PREFERRED_SIZE, 347, GroupLayout.PREFERRED_SIZE)
					.addGap(177))
		);
		gl_cardsManagementPanel.setVerticalGroup(
			gl_cardsManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(Alignment.LEADING, gl_cardsManagementPanel.createSequentialGroup()
					.addGap(96)
					.addComponent(scrollPane, GroupLayout.PREFERRED_SIZE, 229, GroupLayout.PREFERRED_SIZE)
					.addContainerGap(103, Short.MAX_VALUE))
		);
		
		CardDAO cardDao= new CardDAOImpl();
		List<Card> card = cardDao.getCardList();
		String[] cardColumnTitle = { 
				   "marque","modele","homologation"}; 
		Model mCard = new Model(toArrayCard(card),cardColumnTitle);
		tableCards = new JTable(mCard);
	//	JTableHeader headerCard = tableCards.getTableHeader();
	
		scrollPane.setViewportView(tableCards);
		cardsManagementPanel.setLayout(gl_cardsManagementPanel);
		
		JPanel classroomsManagementPanel = new JPanel();
		tabbedPane.addTab("Gestion des salles", null, classroomsManagementPanel, null);
		
		JScrollPane scrollPane_1 = new JScrollPane();
		GroupLayout gl_classroomsManagementPanel = new GroupLayout(classroomsManagementPanel);
		gl_classroomsManagementPanel.setHorizontalGroup(
			gl_classroomsManagementPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(Alignment.TRAILING, gl_classroomsManagementPanel.createSequentialGroup()
					.addContainerGap(187, Short.MAX_VALUE)
					.addComponent(scrollPane_1, GroupLayout.PREFERRED_SIZE, 409, GroupLayout.PREFERRED_SIZE)
					.addGap(139))
		);
		gl_classroomsManagementPanel.setVerticalGroup(
			gl_classroomsManagementPanel.createParallelGroup(Alignment.TRAILING)
				.addGroup(Alignment.TRAILING, gl_classroomsManagementPanel.createSequentialGroup()
					.addGap(88)
					.addComponent(scrollPane_1, GroupLayout.PREFERRED_SIZE, 234, GroupLayout.PREFERRED_SIZE)
					.addContainerGap(106, Short.MAX_VALUE))
		);
		
		
		
		RoomDAO roomDao= new RoomDAOImpl();
		List<Room> room = roomDao.getRoomList();
		String[] roomColumnTitle = {"nom","description"}; 
		final Object[][] arrayRoom = toArrayRoom(room);
		Model mRoom = new Model(arrayRoom,roomColumnTitle);
		tableClassrooms= new JTable(mRoom);
		//JTableHeader headerRoom = tableClassrooms.getTableHeader();
		//scrollPane_1.setColumnHeaderView(tableClassrooms);
		scrollPane_1.setViewportView(tableClassrooms);   // affiche titres
		classroomsManagementPanel.setLayout(gl_classroomsManagementPanel);
		tableClassrooms.addMouseListener(new MouseAdapter() {
		    public void mouseClicked(MouseEvent e) {
		        JTable table =(JTable) e.getSource();
		        Point p = e.getPoint();
		        int row = table.rowAtPoint(p);
		        if (e.getClickCount() == 2) {
		            // your valueChanged overridden method 
		        	RoomUpdate roomUpdate = new RoomUpdate((String)arrayRoom[row][0],(String)arrayRoom[row][1]);
		        	roomUpdate.setVisible(true);
		        }
		    }
		});
		
		
		JPanel recordersManagementPanel = new JPanel();
		tabbedPane.addTab("Gestion des enregisteurs", null, recordersManagementPanel, null);
		contentPane.setLayout(gl_contentPane);
	}
	
	private Object[][] toArrayUserRecorder(List<UserRecorder> datas){
		Object[][] array = new Object[datas.size()][];
		for (int i = 0; i < datas.size(); i++) {
		    ArrayList<String> row = new ArrayList<String>();
		    row.add(datas.get(i).getFirstName());
		    row.add(datas.get(i).getLastName());
		    row.add(datas.get(i).getPassword());
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
}
