package ui;

import java.awt.BorderLayout;
import java.awt.Image;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.JLabel;
import javax.swing.LayoutStyle.ComponentPlacement;

import persistence.RecorderDAOImpl;
import sun.java2d.pipe.DrawImage;
import dm.Recorder;
import dm.Recorder.RecorderStatus;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.nio.charset.Charset;

import javax.swing.SwingConstants;

import communication.Tools;

public class Parring extends JFrame {

	/**
	 * Launch the application.
	 */
	static Parring frame;
	private Recorder _recorder = null;

	/**
	 * Create the frame.
	 */
	public Parring(Recorder rec) {
		if (rec.getStatus() != RecorderStatus.CONNECTED) {
			new MessageBox("Module d'enregistrement non connect\u00E9")
					.setVisible(true);
			dispose();
		} else if (rec.isRecording()) {
			new MessageBox("Module actuelement en enregistrement")
					.setVisible(true);
			dispose();
		} else {
			frame = this;
			_recorder = rec;
			setResizable(false);
			setTitle("");
			setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			setBounds(100, 100, 446, 168);
			JPanel contentPane = new JPanel();
			contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
			setContentPane(contentPane);

			final JLabel text = new JLabel(
					"Pour appairer, cliquer sur le bouton 'Appairer'");
			final JLabel text2 = new JLabel(
					"Et Presser pendant 5 secondes le bouton sur le module de controle");
			text.setHorizontalAlignment(SwingConstants.CENTER);
			text2.setHorizontalAlignment(SwingConstants.CENTER);

			final JButton btnAnnuler = new JButton("Annuler");
			btnAnnuler.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					frame.dispose();
				}
			});

			final JButton btnCalibrer = new JButton("Visualiser");
			btnCalibrer.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					Image image = RecorderDAOImpl._instance.getImage(_recorder);
					if (image == null)
						new MessageBox("Erreur d'acquisition de l'image")
								.setVisible(true);
					else {
						JFrame FImage = new JFrame();

						ImageIcon icon = null;
						try {
							icon = new ImageIcon(image);
							FImage.setSize(icon.getIconWidth(),
									icon.getIconHeight());
						} catch (Exception e) {
							e.printStackTrace();
						}
						FImage.add(new JLabel(icon));
						FImage.repaint();
						FImage.setVisible(true);
					}
				}
			});
			
			final JButton btnAppairer = new JButton("Appairer");
			btnAppairer.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					text.setText("Appairage en cours");
					text2.setText("Veuillez presser pendant 5 secondes le bouton sur le module de controle");
					btnAppairer.setEnabled(false);
					btnAnnuler.setEnabled(false);
					btnCalibrer.setEnabled(false);
					btnAppairer.setVisible(false);
					btnAnnuler.setVisible(false);
					btnCalibrer.setVisible(false);
					//frame.getContentPane().repaint();
					frame.revalidate();
					revalidate();
					Thread t = new Thread()
					{
						public void run() {
							if (RecorderDAOImpl._instance.parringRecorder(_recorder));
								frame.dispose();
						}
					};
					
					//RecorderDAOImpl._instance.parringRecorder(_recorder);
					t.start();
					
				}
			});

			

			JPanel buttonPanel = new JPanel();
			contentPane.add(buttonPanel, BorderLayout.SOUTH);
			buttonPanel.add(btnAnnuler);
			buttonPanel.add(btnCalibrer);
			buttonPanel.add(btnAppairer);

			GroupLayout gl_contentPane = new GroupLayout(contentPane);
			gl_contentPane
					.setHorizontalGroup(gl_contentPane
							.createParallelGroup(Alignment.TRAILING)
							.addGroup(
									gl_contentPane
											.createSequentialGroup()
											.addContainerGap(34,
													Short.MAX_VALUE)
											.addGroup(
													gl_contentPane
															.createParallelGroup(
																	Alignment.TRAILING)
															.addGroup(
																	gl_contentPane
																			.createSequentialGroup()
																			.addComponent(
																					buttonPanel,
																					GroupLayout.PREFERRED_SIZE,
																					GroupLayout.DEFAULT_SIZE,
																					GroupLayout.PREFERRED_SIZE)
																			.addGap(137))
															.addGroup(
																	gl_contentPane
																			.createSequentialGroup()
																			.addComponent(
																					text,
																					GroupLayout.PREFERRED_SIZE,
																					300,
																					GroupLayout.PREFERRED_SIZE)
																			.addGap(63))))
							.addGroup(
									Alignment.LEADING,
									gl_contentPane
											.createSequentialGroup()
											.addGap(18)
											.addComponent(text2,
													GroupLayout.PREFERRED_SIZE,
													370,
													GroupLayout.PREFERRED_SIZE)
											.addContainerGap(48,
													Short.MAX_VALUE)));
			gl_contentPane.setVerticalGroup(gl_contentPane.createParallelGroup(
					Alignment.CENTER).addGroup(
					gl_contentPane
							.createSequentialGroup()
							.addContainerGap()
							.addComponent(text, GroupLayout.PREFERRED_SIZE, 40,
									GroupLayout.PREFERRED_SIZE)
							.addPreferredGap(ComponentPlacement.RELATED)
							.addComponent(text2, GroupLayout.PREFERRED_SIZE,
									40, GroupLayout.PREFERRED_SIZE)
							.addPreferredGap(ComponentPlacement.RELATED,
									GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
							.addComponent(buttonPanel,
									GroupLayout.PREFERRED_SIZE,
									GroupLayout.DEFAULT_SIZE,
									GroupLayout.PREFERRED_SIZE).addGap(3)));
			contentPane.setLayout(gl_contentPane);
			frame.setVisible(true);
		}
	}
}
