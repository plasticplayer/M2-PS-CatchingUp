package ui;

import javax.swing.table.AbstractTableModel;

public class Model extends AbstractTableModel {

	Object donnees[][];
	String titres[];

	public Model(Object donnees[][], String titres[]) {
		this.donnees = donnees;
		this.titres = titres;
	}

	public int getColumnCount() {
		if (donnees != null && donnees.length > 0)
			return donnees[0].length;
		else
			return 0;
	}

	public Object getValueAt(int parm1, int parm2) {
		if (donnees != null && donnees.length > parm1
				&& donnees[parm1].length > parm2)
			return donnees[parm1][parm2];
		else
			return new String("");
	}

	public int getRowCount() {
		if (donnees != null)
			return donnees.length;
		else
			return 0;
	}

	public String getColumnName(int col) {
		if (titres != null && titres.length > col)
			return titres[col];
		else
			return "";
	}
}
