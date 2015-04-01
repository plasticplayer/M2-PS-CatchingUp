package dm;

import java.util.Date;

public class UserRecorder extends User {
	
	private Date dateBegin;
	private Date dateEnd;
	
	public UserRecorder(){
		super();
		
	}
	
	public UserRecorder(String firstName, String lastName, String password, String email, Date dateBegin, Date dateEnd){
		super(firstName,lastName,password,email);
		this.dateBegin = dateBegin;
		this.dateEnd = dateEnd;	
	}
	
	public Date getDateBegin() {
        return dateBegin;
	}

    public void setDateBegin(Date dateBegin) {
        this.dateBegin = dateBegin;
    }
    
    public Date getDateEnd() {
        return dateEnd;
	}

    public void setDateEnd(Date dateEnd) {
        this.dateEnd = dateEnd;
    }

}
