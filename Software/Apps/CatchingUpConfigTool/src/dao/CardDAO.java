package dao;

import java.util.List;

import dm.Card;
import dm.User;
import dm.UserRecorder;

public interface CardDAO {
	/**
	 * Return cards list
	 * @return
	 */
	public List<Card> getCardList();
	
	public Card getCardFromIdUser ( UserRecorder user );
	
	public List<Card> getCardFree( User user );
	
	public boolean createCard ( Card card );
	
	public boolean updateCard ( Card card );
}
