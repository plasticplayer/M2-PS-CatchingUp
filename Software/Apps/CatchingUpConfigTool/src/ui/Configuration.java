package ui;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;
import java.awt.Color;
import java.awt.Point;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.event.WindowStateListener;

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
import sun.org.mozilla.javascript.internal.ObjArray;
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
	private CardDAO cardDao = new CardDAOImpl();
	private UserRecorderDAO userRecorderDao = new UserRecorderDAOImpl();
	private RoomDAO roomDao = new RoomDAOImpl();
	private JTabbedPane tabbedPane;

	private Model mRoom;
	private Model mCard;
	private Model mModul;
	private Model mSpeacker;

	private Thread refreshRecorderStates = new Thread(new Runnable() {
		@Override
		public void run() {
			while (true) {
				try {
					Thread.sleep(5000);
					if (tabbedPane.getSelectedIndex() == 4
							&& !RecorderCreation.isOpen)
						reloadRecorder();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (Exception e) {
					e.printStackTrace();
				}

			}
		}
	});
	String[] connectingModuleColumnTitle = { "Id Enregistreur",
			"Id Activateur", "Salle", "Statut", "Fichiers en attente" };

	/**
	 * Create the frame.
	 * 
	 * @throws ParseException
	 */
	public Configuration() throws ParseException {
		setTitle("Configuration");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 875, 499);
		contentPane = new JPanel();

		setContentPane(contentPane);

		tabbedPane = new JTabbedPane(JTabbedPane.TOP);
		tabbedPane.addChangeListener(new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent e) {
				switch (tabbedPane.getSelectedIndex()) {
				case 0: // Logs
					// loadLogs();
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
					// reloadRecorder();
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
		tabbedPane.addTab("Gestion des intervenants", null,
				createSpeackerPanel(), null);

		// Cards Managment
		tabbedPane.addTab("Gestion des cartes", null, createCardPanel(), null);

		// Rooms Managment
		tabbedPane.addTab("Gestion des salles", null, createRoomPanel(), null);

		// Recorder Manamgment
		tabbedPane.addTab("Gestion des enregistreurs", null,
				createRecorderPanel(), null);
		contentPane.add(tabbedPane);

		refreshRecorderStates.start();
	}

	// / Create Panel
	public JPanel createLogPannel() {
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

		JScrollPane scrollPaneLog = new JScrollPane(logsArea,
				JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
				JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		logsPannel.add(scrollPaneLog);
		logsPannel.add(btnRafraichir, BorderLayout.SOUTH);
		loadLogs();
		return logsPannel;
	}

	public JPanel createSpeackerPanel() throws ParseException {
		JPanel speakersManagementPanel = new JPanel();

		final List<UserRecorder> userRecorder = userRecorderDao
				.getUserRecorderList();

		String[] titreColonnes = { "Identifiant", "Pr\u00E9nom", "Nom", "Email",
				"Date de d\u00E9but", "Date de fin" };

		Object[][] arrayUserRecorder = toArrayUserRecorder(userRecorder);
		if ( arrayUserRecorder.length == 0){
			arrayUserRecorder = new Object[1][titreColonnes.length];
		}
		mSpeacker = new Model(arrayUserRecorder, titreColonnes);

		JScrollPane scrollPaneUser = new JScrollPane();
	
		JButton createUserButton = new JButton("Nouvel utilisateur");
		createUserButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				SpeakerCreation r = new SpeakerCreation();
				r.addWindowListener(new closeWinListener() {
					@Override
					public void windowClosed(WindowEvent e) {
						//super.windowClosed(e);
						reloadSpeacker();
					};
				});
				r.setVisible(true);
				
			}
		});
		speakersManagementPanel.setLayout(new BorderLayout(0, 0));

		tableSpeakers = new JTable(mSpeacker);

		scrollPaneUser.setViewportView(tableSpeakers);
		scrollPaneUser.setBackground(Color.WHITE);		
		speakersManagementPanel.add(scrollPaneUser);
		speakersManagementPanel.add(createUserButton, BorderLayout.SOUTH);
		tableSpeakers.addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				JTable table = (JTable) e.getSource();
				Point p = e.getPoint();

				int row = table.rowAtPoint(p);
				if (e.getClickCount() == 2) {
					UserRecorder r = userRecorder.get(row);

					SpeakerUpdate speakerUpdate = new SpeakerUpdate(r);
					speakerUpdate.addWindowListener(new closeWinListener() {
						@Override
						public void windowClosed(WindowEvent e) {
							//super.windowClosed(e);
							reloadSpeacker();
						};
					});
					speakerUpdate.setVisible(true);
				}
			}

		});

		return speakersManagementPanel;
	}

	public JPanel createCardPanel() {
		JPanel cardsManagementPanel = new JPanel();
		String[] cardColumnTitle = { "Id", "Utilisateur associ\u00E9" };

		JButton btnNewCard = new JButton("Nouvelle carte");
		btnNewCard.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				CardCreation r = new CardCreation();
				r.addWindowListener(new closeWinListener() {
					@Override
					public void windowClosed(WindowEvent e) {
						//super.windowClosed(e);
						reloadCard();
					};
				});
				r.setVisible(true);
			}
		});

		JScrollPane scrollPaneCard = new JScrollPane(
				JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
				JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);

		List<Card> card = cardDao.getCardList();
		Object[][] cards = toArrayCard(card);
		if ( cards.length == 0 )
			cards = new Object[1][cardColumnTitle.length];
		
		mCard = new Model(cards, cardColumnTitle);
		cardsManagementPanel.setLayout(new BorderLayout(0, 0));
		tableCards = new JTable(mCard);

		scrollPaneCard.setViewportView(tableCards);
		cardsManagementPanel.add(scrollPaneCard);
		cardsManagementPanel.add(btnNewCard, BorderLayout.SOUTH);

		return cardsManagementPanel;
	}

	public JPanel createRoomPanel() {
		JPanel classroomsManagementPanel = new JPanel();
		JScrollPane scrollPaneRoom = new JScrollPane();

		JButton btnRoom = new JButton("Nouvelle salle");
		btnRoom.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				RoomCreation r= new RoomCreation();
				r.addWindowListener(new closeWinListener() {
					@Override
					public void windowClosed(WindowEvent e) {
						//super.windowClosed(e);
						reloadRoom();
					};
				});
				r.setVisible(true);
			}
		});

		final List<Room> room = roomDao.getRoomList();
		String[] roomColumnTitle = { "nom", "description" };
		Object[][] arrayRoom = toArrayRoom(room);
		if ( arrayRoom.length == 0)
			arrayRoom = new Object[1][roomColumnTitle.length];
		
		mRoom = new Model(arrayRoom, roomColumnTitle);
		classroomsManagementPanel.setLayout(new BorderLayout(0, 0));
		tableClassrooms = new JTable(mRoom);

		scrollPaneRoom.setViewportView(tableClassrooms);
		classroomsManagementPanel.add(scrollPaneRoom);
		classroomsManagementPanel.add(btnRoom, BorderLayout.SOUTH);
		tableClassrooms.addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				JTable table = (JTable) e.getSource();
				Point p = e.getPoint();
				int row = table.rowAtPoint(p);
				if (e.getClickCount() == 2) {
					Room updated = room.get(row);
					RoomUpdate r = new RoomUpdate(updated);
					
					r.addWindowListener(new closeWinListener() {
						@Override
						public void windowClosed(WindowEvent e) {
							//super.windowClosed(e);
							reloadRoom();
						};
					});
					r.setVisible(true);
				}
			}
		});
		return classroomsManagementPanel;
	}

	public JPanel createRecorderPanel() {
		JPanel recordersManagementPanel = new JPanel();
		JScrollPane scrollPane_enregistreurs = new JScrollPane();

		JButton btnRecorder = new JButton("Nouvel enregistreur");
		btnRecorder.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				RecorderCreation r = new RecorderCreation();
				r.addWindowListener(new closeWinListener() {
					@Override
					public void windowClosed(WindowEvent e) {
						//super.windowClosed(e);
						reloadRecorder();
					};
				});
				r.setVisible(true);
			}
		});

		RecorderDAO connectingModuleDao = RecorderDAOImpl._instance;
		List<Recorder> recorders = connectingModuleDao.getRecorderList();
		Object[][] recordersO = toArrayModuls(recorders);
		if ( recordersO.length == 0 )
			recordersO = new Object[1][connectingModuleColumnTitle.length];
		
		mModul = new Model(recordersO,
				connectingModuleColumnTitle);
		tableModuls = new JTable(mModul);

		tableModuls.addMouseListener(new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				JTable table = (JTable) e.getSource();
				Point p = e.getPoint();
				int row = table.rowAtPoint(p);
				if (e.getClickCount() == 2) {
					Recorder rec = RecorderDAOImpl._recorders.get(row);
					if (rec != null)
					{
						Parring r = new Parring(rec);
						r.addWindowListener(new closeWinListener() {
							@Override
							public void windowClosed(WindowEvent e) {
								//super.windowClosed(e);
								reloadRecorder();
							};
						});
						//r.setVisible(true);
					}
				}
			}
		});
		recordersManagementPanel.setLayout(new BorderLayout(0, 0));

		scrollPane_enregistreurs.setViewportView(tableModuls);
		recordersManagementPanel.add(scrollPane_enregistreurs);
		recordersManagementPanel.add(btnRecorder, BorderLayout.SOUTH);

		return recordersManagementPanel;
	}

	// / Reload functions
	public void loadLogs() {
		new Thread()
		{
			public void run() {	
				logsArea.setText("");
				LogsImpl._instance.getLogs(logsArea);
			};
	
		}.run();
		 //logsArea.setText( LogsImpl._instance.getLogs() );
	}

	public void reloadRoom() {
		Object[][] rooms = toArrayRoom(roomDao.getRoomList());
		if ( rooms.length == 0 )
			mRoom.donnees = new Object[1][mRoom.titres.length];
		else mRoom.donnees = rooms;
		mRoom.fireTableDataChanged();
	}

	public void reloadSpeacker() {
		try {
			Object[][] users = toArrayUserRecorder(userRecorderDao.getUserRecorderList());
			if ( users.length == 0 )
				mSpeacker.donnees = new Object[1][mSpeacker.titres.length]; 
			else
				mSpeacker.donnees = users;
		} catch (ParseException e) {
		}
		mSpeacker.fireTableDataChanged();
	}

	public void reloadCard() {
		Object[][] cards = toArrayCard(cardDao.getCardList());
		if ( cards.length == 0)
			mCard.donnees = new Object[1][mCard.titres.length];
		else
			mCard.donnees = cards;
		mCard.fireTableDataChanged();
	}

	public void reloadRecorder() {
		List<Recorder> recorders = RecorderDAOImpl._instance.getRecorderList();
		Object[][] reco = toArrayModuls(recorders);
		if ( reco.length == 0)
			mModul.donnees = new Object[1][mModul.titres.length];
		else
			mModul.donnees = reco;
		mModul.fireTableDataChanged();
	}

	// / List to Object[][]
	private Object[][] toArrayUserRecorder(List<UserRecorder> datas) {
		if(datas == null)
			return new Object[0][6];
		Object[][] array = new Object[datas.size()][];
		for (int i = 0; i < datas.size(); i++) {
			ArrayList<String> row = new ArrayList<String>();
			row.add("" + datas.get(i).getId());
			row.add(datas.get(i).getFirstName());
			row.add(datas.get(i).getLastName());
			row.add(datas.get(i).getEmail());
			row.add(datas.get(i).getDateBegin().toString());
			row.add(datas.get(i).getDateEnd().toString());
			array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}

	private Object[][] toArrayCard(List<Card> datas) {
		Object[][] array = new Object[datas.size()][];
		for (int i = 0; i < datas.size(); i++) {
			ArrayList<String> row = new ArrayList<String>();
			row.add(datas.get(i).getHexNumberCard());
			if (datas.get(i).getUser() != null) {
				row.add(datas.get(i).getUser().getFirstName() + " "
						+ datas.get(i).getUser().getLastName());
			} else {
				row.add("");
			}
			array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}

	private Object[][] toArrayRoom(List<Room> datas) {
		Object[][] array = new Object[datas.size()][];
		for (int i = 0; i < datas.size(); i++) {
			ArrayList<String> row = new ArrayList<String>();
			row.add(datas.get(i).getName());
			row.add(datas.get(i).getDescription());
			array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}

	private Object[][] toArrayModuls(List<Recorder> recorders) {
		Object[][] array = new Object[recorders.size()][];
		for (int i = 0; i < recorders.size(); i++) {
			ArrayList<String> row = new ArrayList<String>();
			row.add(recorders.get(i).getRecordingModule().getIdNetwork() + "");
			row.add(recorders.get(i).getConnectingModule()
					.getIdNetworkRecording()
					+ "");
			row.add(recorders.get(i).getRoom().getName());
			row.add(recorders.get(i).toString());
			row.add(recorders.get(i).getFilesInQueue() + "");
			array[i] = row.toArray(new String[row.size()]);
		}
		return array;
	}

	public class closeWinListener implements WindowListener {
		@Override
		public void windowActivated(WindowEvent e) {
			// TODO Auto-generated method stub

		}

		@Override
		public void windowClosed(WindowEvent e) {
			// TODO Auto-generated method stub

		}

		@Override
		public void windowClosing(WindowEvent e) {
			// TODO Auto-generated method stub

		}

		@Override
		public void windowDeactivated(WindowEvent e) {
			// TODO Auto-generated method stub

		}

		@Override
		public void windowDeiconified(WindowEvent e) {
			// TODO Auto-generated method stub

		}

		@Override
		public void windowIconified(WindowEvent e) {
			// TODO Auto-generated method stub

		}

		@Override
		public void windowOpened(WindowEvent e) {
			// TODO Auto-generated method stub

		}
	}

}
