package communication;

import java.math.BigInteger;
import java.util.*;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import jssc.SerialPortList;

public class CardReader {
	public static CardReader reader = null ;
	public static String s = "";
	public static boolean newValue = false;
	private SerialPort serial = null;
	
	public CardReader (){
		String[] a = SerialPortList.getPortNames();
		String s = "Serial ports detected: ";
		for ( String b : a ){
			s += b + ",";
		}
		Tools.LOGGER_INFO(s.substring(0, s.length()-1));
		reader = this;
		openPort();
	}

	public static void openPort(){
		if ( reader.serial != null )
			return;
		
		String[] a = SerialPortList.getPortNames();
		if ( a.length != 0 ){
			reader.serial = new SerialPort(a[0]);
			try{
				Tools.LOGGER_INFO("Open Serial Port");
				reader.serial.openPort();
				reader.serial.setParams(SerialPort.BAUDRATE_9600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
				reader.serial.writeByte((byte) 0x12);
				reader.serial.addEventListener(new SerialPortEventListener() {
					@Override
					public void serialEvent(SerialPortEvent arg0) {
						try {
							s = reader.serial.readHexString().replace(" ","");
							
							if ( s.length() != 14 )
								return;
							s = "" + new BigInteger( s, 16);
							
							//s = "" + Integer.parseInt("0x" + s );
							Tools.LOGGER_INFO( "Serial read: " + s );
							newValue = true;
						} catch (SerialPortException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						
					}
				});
			}
			catch ( SerialPortException e){
				Tools.LOGGER_ERROR(e.toString());
			}
		}
	}
	
	@Override
	protected void finalize() throws Throwable {
		close();
		reader = null;
	}
	
	public static void close(){
		if ( reader == null || reader.serial == null )
			return;
			
		try {
			Tools.LOGGER_INFO("Close Serial Port");
			reader.serial.closePort();
			reader.serial = null;
		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
