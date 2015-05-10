#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "Mysql.h"
#include "DefVariableBase.h"
#include "Config.h"


using namespace std;

/** Statics objects **/
static User _UserTable;
static WebsiteUser _WebsiteUserTable;
static RecorderUser _RecorderUserTable;
static Card _CardTable;
static Historic _HistoricTable;
static RecordingModule _RecordingTable;
static Recorder _RecorderTable;
static ConnectingModule _ConnectingModuleTable;
static Room _RoomTable;
static Chapter _ChapterTable;
static Category _CategoryTable;
static Lesson _LessonTable;
static Attachment _AttachmentTable;
static Reference _ReferenceTable;
static FileLesson _FileLessonTable;

Mysql *Mysql::_DataBase = NULL;

/// construtors
Mysql::Mysql(  ){
	_DataBase = this;
	_IsInsertingRow = false;
}

bool Mysql::connect(string host, string database, string user, string password ){
	try{
                _Connection = new Connection( ( char*) host.c_str() , (char *) database.c_str() , (char*) user.c_str() , (char*) password.c_str() );
        }
        catch ( exception e  ){
                LOGGER_ERROR("MySql parameters Error");
		return false;
        }
	return true;
}

uint64_t Mysql::getIdRecorderFromMac( string mac ){
	uint64_t idRecorder = 0;
	string req = SSTR("SELECT " << _RecordingTable.id.value << " FROM " << _RecordingTable.name << " WHERE " << _RecordingTable.adressMAC.value << "='" << mac  << "'");
	LOGGER_VERB("Get id Recorder req: " << req );
	Result* res = _DataBase->_Connection->Query( (char*) req.c_str() );
	if (res->Next())
	{
		idRecorder = strtoull(res->GetCurrentRow()->GetField(1), NULL, 0);
	}
	return idRecorder;	
}

uint64_t Mysql::getIdUserRecorderFromTag( uint64_t idCard ){
	if ( idCard == 0 ) return 0x00;

	uint64_t idUserRecorder = 0;

	// __RecorderUserTable.dateBegin _RecorderUserTable.dateEnd
	string req = SSTR("SELECT r." <<  _RecorderUserTable.id.value  << " FROM " << _RecorderUserTable.name << " r ," << _CardTable.name 
			<< " s WHERE " << _CardTable.number.value << "=" << idCard  << " AND r." << _RecorderUserTable.id.value << "=s." << _CardTable.iduser.value << " AND NOW() BETWEEN " 
			<< _RecorderUserTable.dateBegin.value << " AND " << _RecorderUserTable.dateEnd.value << ";");


	LOGGER_VERB("Get id UserRecorder req: " << req );

	Result* res = _DataBase->_Connection->Query( (char*) req.c_str() );
	if (res->Next())
	{
		idUserRecorder = strtoull(res->GetCurrentRow()->GetField(1), NULL, 0);
	}
	return idUserRecorder;
}

uint64_t Mysql::createRecording ( uint64_t idRecorder , uint64_t idUserRecorder){
	if ( idRecorder == 0 || idUserRecorder == 0 )
		return 0x00;

	// Look for only 1 create Recording on same time 
	while ( _DataBase->_IsInsertingRow ) { usleep(10 ); }
	_DataBase->_IsInsertingRow = true;

	uint64_t idRecording = 0x00;		

	string req = SSTR( "INSERT INTO " << _ChapterTable.name << " ( " << _ChapterTable.status.value << ", " << _ChapterTable.date.value << " , " << _ChapterTable.idUser.value << " , " 
		<< _ChapterTable.idRecorder.value << " ) values ( 'TRANSFER' , NOW() , " << idUserRecorder << " , " << idRecorder << " );");	
	LOGGER_DEBUG("Mysql create recording : " << req );
	_DataBase->_Connection->Query( (char*) req.c_str() );

	Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();" );
	if ( res->Next() ){
		idRecording = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
	}	
	_DataBase->_IsInsertingRow = false;
	
	LOGGER_INFO("Create Chapter: " << idRecording << " For Recorder: " << idRecorder );
	return idRecording;
}

void Mysql::stopRecording ( uint64_t idRecording ){
	LOGGER_INFO("Stop Recording:" << idRecording);
	string req = SSTR("UPDATE " << _ChapterTable.name << " SET " << _ChapterTable.status.value << "='WAITINGTREATMENT' WHERE " << _ChapterTable.id.value << "=" << idRecording << "; " );
	_DataBase->_Connection->Query( (char*) req.c_str() );
}


uint64_t Mysql::createRecorder ( uint64_t idRoom, string addressMac ){
	if ( getIdRecorderFromMac( addressMac) != 0x00 ) {
		LOGGER_WARN( "Record already exist with Mac. Request ignore");
		return 0;
	}

	uint64_t idRecorder = 0x00, idRecordingModule = 0x00, idConnectingModule = 0x00;
	string req;

	LOGGER_INFO("Create Recorder");
	while ( _DataBase->_IsInsertingRow ) usleep(10);
	_DataBase->_IsInsertingRow = true;
	
	// Create ConnectingModule	
	req = SSTR("INSERT INTO " << _ConnectingModuleTable.name << " ( " << _ConnectingModuleTable.idNetwork.value << ") values ( 0 );");
	LOGGER_VERB ( "Mysql Create connecting Module: " << req );
	_DataBase->_Connection->Query( ( char*) req.c_str());
	
	Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
	if ( res->Next() ){
		idConnectingModule = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
	}


	// Create Recording Module
	req = SSTR("INSERT INTO " << _RecordingTable.name << " ( " <<  _RecordingTable.adressMAC.value << " ) values ( '" << addressMac << "');" );
	LOGGER_VERB( "Mysql Create Recording Module: " << req );
	_DataBase->_Connection->Query(( char*) req.c_str() );

	res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
	if ( res->Next() ){
		idRecordingModule = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
	}


	// Create recorder
	req = SSTR ( "INSERT INTO " << _RecorderTable.name << "( " << _RecorderTable.idRecordingModule.value << " , " << _RecorderTable.idConnectingModule.value << " , " << _RecorderTable.idRoom.value << " ) values ( "
		<< idRecordingModule << " , " << idConnectingModule << " , " << idRoom << " );");
	LOGGER_VERB("Myslq create recorder: " << req ) ;
	_DataBase->_Connection->Query( (char* ) req.c_str() );

	res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
	if ( res->Next() ){
		idRecorder = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
	}

	
	_DataBase->_IsInsertingRow = false;
	LOGGER_INFO("Create Recoder: " << idRecorder << " | idConnectingModule: " << idConnectingModule << " | idRecordingModule " << idRecordingModule );
	return idRecorder;
}


bool Mysql::generateNrfAddress ( uint64_t recorderId, uint64_t *rec, uint64_t *act ){
	// TODO : USE DB TO GENERATE ADDRESSE

	*(rec) = (uint64_t) ( ((uint64_t)rand())<<32 | ((uint64_t)rand())<<16 | rand() ) & 0x00FFFFFFFF;
	*(act) = (uint64_t) ( ((uint64_t)rand())<<32 | ((uint64_t)rand())<<16 | rand() ) & 0x00FFFFFFFF;

	return true; // RETURN TRUE IF RECORDER EXIST
}

uint64_t Mysql::createRoom ( string roomName, string description ){
	uint64_t idRoom = 0;
	
	while ( _DataBase->_IsInsertingRow ) usleep(10);
        _DataBase->_IsInsertingRow = true;

	string req = SSTR ( "INSERT INTO " << _RoomTable.name << "( " << _RoomTable.roomName.value << " , " << _RoomTable.description.value << " ) values ( '"
                << roomName  << "' , '" << description  << "' );");
        LOGGER_VERB("Myslq create room: " << req ) ;
        _DataBase->_Connection->Query( (char* ) req.c_str() );

        Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
        if ( res->Next() ){
                idRoom = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
        }

        _DataBase->_IsInsertingRow = false;
	LOGGER_INFO("Create Room:" << roomName << " :" << idRoom);
	return idRoom;	
}

uint64_t Mysql::createUserRecorder ( string firstName, string lastName, string password, string email, string dateBegin, string dateEnd ){
	uint64_t idUserRecorder = 0;
	
	while ( _DataBase->_IsInsertingRow ) usleep(10);
        _DataBase->_IsInsertingRow = true;

	string req = SSTR ( "INSERT INTO " << _UserTable.name << "( " << _UserTable.firstName.value << " , " << _UserTable.lastName.value << " , " << _UserTable.password.value << "," << _UserTable.email.value << " ) values ( '"
			  << firstName << "' , '" << lastName << "' , '" << password << "' , '" << email << "' );");  
        LOGGER_VERB("Mysql create userRecorder : " << req );
 
	_DataBase->_Connection->Query ( (char*) req.c_str());
	Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
        if ( res->Next() ){
                idUserRecorder = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
        }


	req = SSTR ( "INSERT INTO " << _RecorderUserTable.name << "( " << _RecorderUserTable.id.value << " , " << _RecorderUserTable.dateBegin.value << " , " << _RecorderUserTable.dateEnd.value << " ) values ( " << idUserRecorder  << " , '" << dateBegin << "' , '" << dateEnd << "' );");  
        LOGGER_VERB("Myslq create userRecorder: " << req ) ;
	
	_DataBase->_Connection->Query( (char* ) req.c_str() );

	_DataBase->_IsInsertingRow = false;
        LOGGER_INFO("Create UserRecorder:" << firstName << "," << lastName << ":" << idUserRecorder );
        return idUserRecorder;
}

uint64_t Mysql::getIdFromTagNumber ( uint64_t cardNumber ){
	uint64_t idCard = 0x00;
	string req = SSTR ( "SELECT " << _CardTable.idcard.value << " FROM " << _CardTable.name << " WHERE " << _CardTable.number.value << "='" << cardNumber <<"';") ;	
	Result *res = _DataBase->_Connection->Query(( char*) req.c_str()  );
        if ( res->Next() ){
                idCard = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
        }
	return idCard;
}

uint64_t Mysql::createCard ( uint64_t cardNumber, uint64_t idUser ){
	uint64_t idCard = 0x00;
	
	if ( getIdFromTagNumber ( cardNumber ) != 0x00 ){
		LOGGER_WARN ( "Card already in base: " << std::hex << cardNumber << ". Request ignore");
		return 0x00;
	}
	string req;
	if (  idUser == 0x00 ) 
		req = SSTR ( "INSERT INTO " << _CardTable.name << " ( " << _CardTable.number.value << " , " << _CardTable.iduser.value << " ) VALUES ( '" << cardNumber << "', NULL  );" );
	else
		req = SSTR ( "INSERT INTO " << _CardTable.name << " ( " << _CardTable.number.value << " , " << _CardTable.iduser.value << " ) VALUES ( '" 
			<< cardNumber << "' , " << idUser  << " );" );

	while ( _DataBase->_IsInsertingRow ) usleep( 10 );
	_DataBase->_IsInsertingRow = true;

	LOGGER_VERB("Mysql create Card " << req );
	_DataBase->_Connection->Query( (char*) req.c_str() );

	Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
        if ( res->Next() ){
                idCard = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
        }

	_DataBase->_IsInsertingRow = false;
	LOGGER_INFO ( "Card create: " << std::hex << cardNumber << ":" << idUser <<":" << idCard );
	return idCard;
}


bool Mysql::addFileToRecording ( uint64_t idRecording , string path ){
	// TODO:
	LOGGER_VERB("Add file " << path << " To Record: " << idRecording);
	return true;
}

