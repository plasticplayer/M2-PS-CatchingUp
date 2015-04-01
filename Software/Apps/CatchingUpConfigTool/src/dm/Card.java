package dm;

public class Card {
	private int id;
    private String numberCard;
    private User user;
    
    public Card(){
    	
    }
    
    public Card(String numberCard, User user){
    	this.numberCard = numberCard;
    	this.user= user;	
    }
    
    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }
    
    public String getNumberCard() {
        return numberCard;
    }

    public void setNumberCard(String numberCard) {
        this.numberCard = numberCard;
    }
    
    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }
    

}
