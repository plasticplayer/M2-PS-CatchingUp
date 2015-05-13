#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "Config.h"
#include "define.h"
#include "ConfigAppli.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "Mysql.h"
#include "Recorder.h"

using namespace std;

/****************  Constructor  ****************/ 
ConfigAppli::ConfigAppli ( ){
}

bool ConfigAppli::run(){
	getRecorders();
	//CurrentApplicationConfig.TCP_APPLI_PORT
	return true;
}


void ConfigAppli::getRecorders(){
	Result* res = Mysql::getRecorders();
	
	if ( res == NULL ) 
		return;
		
	uint64_t idRec;
	string roomName, mac;
	Recorder *rec;
	string ans = "";
	while ( res->Next() ){
		idRec = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		roomName = res->GetCurrentRow()->GetField(2);
		mac = res->GetCurrentRow()->GetField(3);

		rec = Recorder::getRecorderByMac( (BYTE*) mac.c_str() );
		if ( rec != NULL && rec->isTcpConnected() ){
			ans = SSTR ( ans << "Connected.  IdRec: " << idRec << " RoomName: " << roomName << " Mac:" << mac << " Ip: " << rec->_IpAddr 
					<< " File in queue: " << rec->filesInWait << " IsRecording: " << rec->isRecording() << endl);
    			//bool isRecording();
			//uint16_t filesInWait;
		}
		else		
			ans = SSTR ( ans << "Unconnected.  IdRec: " << idRec << " RoomName: " << roomName << " Mac:" << mac << endl);
	}
	
	for ( list<UnconnectedClient*>::iterator it = Recorder::_UnconnectedClients.begin() ; it != Recorder::_UnconnectedClients.end(); ++it){
		UnconnectedClient * uc = *it;	
		ans = SSTR ( ans << "Unassociate. Mac: " << uc->_MacAddress << endl);
	}
	cout << ans << endl;
}

/****************  Getters / Setters  ****************/ 
