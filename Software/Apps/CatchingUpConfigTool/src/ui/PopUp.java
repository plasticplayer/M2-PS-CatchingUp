package ui;

import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

public class PopUp extends JPopupMenu{
	JMenuItem anItem;
    public PopUp(){
        anItem = new JMenuItem("Click Me!");
        add(anItem);
    }
}
