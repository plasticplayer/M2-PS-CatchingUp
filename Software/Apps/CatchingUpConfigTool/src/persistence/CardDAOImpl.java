package persistence;

import java.util.ArrayList;
import java.util.List;

import dao.CardDAO;
import dm.Card;
import dm.User;

public class CardDAOImpl implements CardDAO {

	@Override
	public List<Card> getCardList() { 
		List<Card> cards = new ArrayList<Card>();
		//String numberCard, User user
		User user = new User("Ghilles", "Mostafaoui", "1234", "ghilles.mostafaoui@u-cergy.fr");
		Card card = new Card("5E:FF:56:A2:AF:15",user);
		cards.add(card);
		user = null;
		user = new User("Phillipe", "Laroque", "1234", "phillipe.laroque@u-cergy.fr");
		card = new Card("7E:BD:84:A2:C4:21", user);
		cards.add(card);
		card = new Card("B3:C9:93:D3:C8:78", null);
		cards.add(card);
		return cards;
	}

}
