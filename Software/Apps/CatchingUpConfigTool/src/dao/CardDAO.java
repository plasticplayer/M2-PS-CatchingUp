package dao;

import java.util.List;

import dm.Card;

public interface CardDAO {
	/**
	 * Return cards list
	 * @return
	 */
	public List<Card> getCardList();
	
}
