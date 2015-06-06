package persistence;

import java.util.ArrayList;
import java.util.List;

import ui.MessageBox;
import communication.Tools;
import dao.CardDAO;
import dm.Card;
import dm.User;
import dm.UserRecorder;

public class CardDAOImpl implements CardDAO {

	public static List<Card> cards = new ArrayList<Card>();
	public static CardDAOImpl _instance = new CardDAOImpl();
	
	public List<Card> getCardFree( User user ){
		List<Card> cardsFree = new ArrayList<Card>();
		
		if ( cards.isEmpty() )
			getCardList();
		
		for ( Card card : cards ){
			if ( card.getUser() == null || card.getUser() == user )
				cardsFree.add(card);
		}
		
		return cardsFree;
	}
	
	public Card getCardFromIdUser ( UserRecorder user ){
		for ( Card card : cards ){
			if ( card.getUser() == user )
				return card;
		}
		return null;
	}
	
	public boolean createCard ( Card card ){
		String req = "<type>create_cards</type><cards>"
				+ "<card><number>"+ card.getNumberCard() +"</number>";
		if ( card.getUser() != null)
			req += "<iduser>" + card.getUser().getId() + "</iduser>";
		req +="</card></cards>";
		
		
		String res = communication.Server.sendData( req );
		String[] lines = res.split(System.getProperty("line.separator"));
		
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("CREATE_CARDS") != 0 )
		  	return false;
		
		String idCard, number, line;
		
		for ( int i = 1; i < lines.length; i++ ){
			line = Tools.getValue( lines[i] ,"card");
			if ( line == "" )
				continue;
				
			idCard 	= Tools.getValue( line ,"idcard");
			number 	= Tools.getValue( line ,"number");
			
			if ( card.getNumberCard().compareTo(number) == 0 ){
				int id = Integer.parseInt(idCard.trim());
				card.setId(id);
				if ( id == 0 ){
					new MessageBox("Création de la carte impossible").setVisible(true);
					Tools.LOGGER_ERROR("Cannot create card");
					return false;
				}
				Tools.LOGGER_INFO("Create card OK" );
				return true;
			}
		}		
		new MessageBox("Erreur lors de la création de la carte").setVisible(true);
		return false;
		
	}
	
	@Override
	public List<Card> getCardList() { 
		cards = new ArrayList<Card>();
		
		String res = communication.Server.sendData("<type>need_cards</type>");
		String[] lines = res.split(System.getProperty("line.separator"));
		  
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo("GET_CARDS") != 0 )
		  	return null;
	  
		  Card card;
		  String id, idUser, number;
		  
		  for ( int i = 1; i < lines.length; i++ ){
			 id 	= Tools.getValue( lines[i] ,"id");
			 idUser = Tools.getValue( lines[i] ,"iduser");
			 number = Tools.getValue( lines[i] ,"number");
			 
			 if ( id == "" || number == "" )
				 continue;
			 
			 int idCard = Integer.parseInt(id.trim());
			 int idUsr  = Integer.parseInt(idUser.trim());
			 
			 User user = UserRecorderDAOImpl._instance.getUserRecorder( idUsr );
			 card = new Card(number, user );
			 card.setId(idCard);
			 cards.add( card );
		  }
		return cards;
	}

	@Override
	public boolean updateCard(Card card) {
		String req;
		if ( card.getUser() != null )
			req = "<type>update_cards</type><cards><card><idcard>" + card.getId() + "</idcard><iduser>" + card.getUser().getId() + "</iduser></card></cards>";
		else
			req = "<type>update_cards</type><cards><card><idcard>" + card.getId() + "</idcard><iduser>0</iduser></card></cards>";
		
		String res = communication.Server.sendData( req );
		String[] lines = res.split(System.getProperty("line.separator"));
		
		String type = Tools.getValue( lines[0] ,"type");
		if ( type.compareTo( "UPDATE_CARDS" ) != 0 )
		  	return false;
		
		String id, success;
		 for ( int i = 1; i < lines.length; i++ ){
			 id 	= Tools.getValue( lines[i] ,"idcard");
			 success = Tools.getValue( lines[i] ,"success");
			 if ( id.compareTo(""+ card.getId()) == 0 ){
				 if ( success.trim().compareTo("1") == 0 ){
					 Tools.LOGGER_INFO("Update card ok");
					 return true;
				 }
			 }
		 }
		new MessageBox("Erreur lors de la mise à jour").setVisible(true);
		Tools.LOGGER_ERROR("Cannot update card" );
		return false;
	}

}
