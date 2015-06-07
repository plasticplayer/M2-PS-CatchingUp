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
	return verifyTables();
}

bool Mysql::verifyTables(){
	while ( _DataBase->_IsInsertingRow ) { usleep(10 ); }
	_DataBase->_IsInsertingRow = true;
	string req;
	try{
		/// USERS
		req = SSTR ( "SELECT " << _UserTable.id.value << "," <<  _UserTable.firstName.value << "," << _UserTable.lastName.value 
					<< "," << _UserTable.password.value << "," << _UserTable.email.value << " FROM " << _UserTable.name << " LIMIT 0;");
		
		LOGGER_VERB("Verify Table: " << _UserTable.name );
		_DataBase->_Connection->Query( (char*) req.c_str() );
		
		
		/// USERS RECORDERS
		req = SSTR ( "SELECT " << _RecorderUserTable.id.value << "," <<  _RecorderUserTable.dateBegin.value << "," << _RecorderUserTable.dateEnd.value 
					<< " FROM " << _RecorderUserTable.name << " LIMIT 0;" );
		
		LOGGER_VERB("Verify Table: " << _RecorderUserTable.name );
		_DataBase->_Connection->Query( (char*) req.c_str() );
		
		/// USERS WebSite
		req = SSTR ( "SELECT " << _WebsiteUserTable.id.value << "," <<  _WebsiteUserTable.registration.value << " FROM " << _WebsiteUserTable.name << " LIMIT 0;");
		
		LOGGER_VERB("Verify Table: " << _WebsiteUserTable.name );
		_DataBase->_Connection->Query( (char*) req.c_str() );
		
		// TODO : ADD OTHERS TABLE
	}
	catch ( exception e  ){
			cout << req << endl;
			LOGGER_ERROR("MySql DB format Error");
			_DataBase->_IsInsertingRow = false;
			return false;
	}
	_DataBase->_IsInsertingRow = false;
	
	LOGGER_INFO("MYSQL DATABASE OK");
	return true;
}


uint64_t Mysql::countUpdatedRows(){
	Result* res = _DataBase->_Connection->Query( (char*) "SELECT ROW_COUNT();" );
	if ( res->Next() ){
		return strtoull( res->GetCurrentRow()->GetField(1),NULL,0);
	}
	return ( uint64_t) 0;
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
		<< _ChapterTable.idRecorder.value << " ) values ( 'RECORDING' , NOW() , " << idUserRecorder << " , " << idRecorder << " );");	
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
	string req = SSTR("UPDATE " << _ChapterTable.name << " SET " << _ChapterTable.status.value << "='TRANSFER' WHERE " << _ChapterTable.id.value << "=" << idRecording << "; " );
	_DataBase->_Connection->Query( (char*) req.c_str() );
}




bool Mysql::generateNrfAddress ( uint64_t recorderId, uint64_t *rec, uint64_t *act ){
	// TODO : USE DB TO GENERATE ADDRESSES
	uint64_t idRecordingModule = 0x00;
	uint64_t idConnectingModule = 0x00;
 
	*(rec) = (uint64_t) ( ((uint64_t)rand())<<32 | ((uint64_t)rand())<<16 | rand() ) & 0x00FFFFFFFF;
	*(act) = (uint64_t) ( ((uint64_t)rand())<<32 | ((uint64_t)rand())<<16 | rand() ) & 0x00FFFFFFFF;	

	if ( idConnectingModule == 0x00 || idRecordingModule == 0x00 ){
		LOGGER_ERROR("Cannot generate NRF Address");
		return true;
	}
	string req = SSTR( "UPDATE " << _RecordingTable.name << " SET " << _RecordingTable.idNetwork.value << "=" << *rec << " WHERE " << _RecordingTable.id.value << "=" << idRecordingModule << ";") ;

	while ( _DataBase->_IsInsertingRow ) usleep( 10 );
	_DataBase->_IsInsertingRow = true;

	_DataBase->_Connection->Query( (char*) req.c_str() );

	req = SSTR ( "UPDATE " << _ConnectingModuleTable.name << " SET " << _ConnectingModuleTable.idNetwork.value << "=" << *act << " WHERE " << _ConnectingModuleTable.id.value << "=" << idConnectingModule << ";" );	
	_DataBase->_Connection->Query( (char*) req.c_str() );
	
	_DataBase->_IsInsertingRow = false;
	
	return true; // RETURN TRUE IF RECORDER EXIST
}

uint64_t Mysql::getIdFromTagNumber ( string cardNumber ){
	uint64_t idCard = 0x00;
	string req = SSTR ( "SELECT " << _CardTable.idcard.value << " FROM " << _CardTable.name << " WHERE " << _CardTable.number.value << "='" << cardNumber <<"';") ;	
	Result *res = _DataBase->_Connection->Query(( char*) req.c_str()  );
        if ( res->Next() ){
                idCard = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
        }
	return idCard;
}




bool Mysql::addFileToRecording ( uint64_t idRecording , string path, string type ){
	// TODO:
	string req = SSTR("INSERT INTO " << _FileLessonTable.name << " ( " << _FileLessonTable.fileLessonName.value << "," << _FileLessonTable.type.value << ","
		<< _FileLessonTable.status.value << ", " << _FileLessonTable.idChapter.value << " ) VALUES ( '" 
		<< path << "', '" << type << "' ,'WAITINGTREAtMENT', "  << idRecording << ");");
	//LOGGER_VERB("Add file " << path << " To Record: " << idRecording << endl << req  );
	
	while ( _DataBase->_IsInsertingRow ) usleep( 10 );
	_DataBase->_IsInsertingRow = true;

	LOGGER_VERB("Mysql AddFile (" << path << ") " << req );
	_DataBase->_Connection->Query( (char*) req.c_str() );
	
	_DataBase->_IsInsertingRow = false;
	return true;
}

void Mysql::RecordingTransferFinished ( uint64_t idRecording ){
	string req = SSTR ( "UPDATE " << _ChapterTable.name << " SET " << _ChapterTable.status.value << "='TRANSFER' WHERE " << _ChapterTable.id.value << "=" << idRecording << ";") ;
	while ( _DataBase->_IsInsertingRow ) usleep( 10 );
	_DataBase->_IsInsertingRow = true;

	LOGGER_VERB("Mysql RecordingTransfer Finished (" << idRecording << ") " << req );
	_DataBase->_Connection->Query( (char*) req.c_str() );
	
	_DataBase->_IsInsertingRow = false;
}



/****************  Administration - Getters ****************/
Result* Mysql::getUsers(){
	string req = SSTR (
			"SELECT u." << _UserTable.id.value << ",u." << _UserTable.firstName.value << ",u." << _UserTable.lastName.value << ",u." << _UserTable.email.value 
			<< ",w." << _WebsiteUserTable.registration.value
			<< " FROM " << _UserTable.name << " u, " << _WebsiteUserTable.name << " w WHERE u." << _UserTable.id.value << "=w." << _RecorderUserTable.id.value << ";"
	);
	return _DataBase->_Connection->Query(( char* ) req.c_str() );
}

Result* Mysql::getRooms(){
	string req = SSTR (
			"SELECT DISTINCT ro." << _RoomTable.id.value << "," << _RoomTable.roomName.value << "," << _RoomTable.description.value << ", IFNULL(" << _RecorderTable.idRecorder.value
			<< ",0) FROM " << _RoomTable.name << " ro LEFT OUTER JOIN " << _RecorderTable.name << " re  on re." << _RecorderTable.idRoom.value << "=ro."  << _RoomTable.id.value  << ";"
	);
	return _DataBase->_Connection->Query(( char* ) req.c_str() );
}

Result* Mysql::getCards(){
	string req = SSTR (
			"SELECT " << _CardTable.idcard.value << ",IFNULL(" << _CardTable.iduser.value << ",0)," << _CardTable.number.value
			<< " FROM " << _CardTable.name << ";"
	);

	return _DataBase->_Connection->Query(( char* ) req.c_str() );
}

Result* Mysql::getRecorders(){
	string req = SSTR ( 
			"SELECT " << _RecorderTable.idRecorder.value << ",ro." << _RoomTable.id.value << "," << _RoomTable.roomName.value << "," << _RecordingTable.adressMAC.value
			<< ",r." << _RecorderTable.idConnectingModule.value << ",r." << _RecorderTable.idRecordingModule.value
			<< " FROM " << _RecorderTable.name << " r," << _RoomTable.name << " ro," << _RecordingTable.name << " re WHERE r."
			<< _RecorderTable.idRecordingModule.value  << "=re." << _RecordingTable.id.value << " AND r." << _RecorderTable.idRoom.value << "=ro." 
			<< _RoomTable.id.value << ";"
	);
	
	return _DataBase->_Connection->Query(( char* ) req.c_str() );
}

Result* Mysql::getUsersRecorders(){
	string req = SSTR (
			"SELECT u." << _UserTable.id.value << ",u." << _UserTable.firstName.value << ",u." << _UserTable.lastName.value << ",u." << _UserTable.email.value 
			<< ",r." << _RecorderUserTable.dateBegin.value << ",r." << _RecorderUserTable.dateEnd.value
			<< " FROM " << _UserTable.name << " u, " << _RecorderUserTable.name << " r WHERE u." << _UserTable.id.value << "=r." << _RecorderUserTable.id.value << ";"
	);

	return _DataBase->_Connection->Query(( char* ) req.c_str() );
}



/****************  Administration - Create ****************/
uint64_t Mysql::createRoom ( string roomName, string description ){
	uint64_t idRoom = 0;
	
	while ( _DataBase->_IsInsertingRow ) usleep(10);
        _DataBase->_IsInsertingRow = true;

	string req = SSTR ( "INSERT INTO " << _RoomTable.name << "( " << _RoomTable.roomName.value << " , " << _RoomTable.description.value << " ) values ( '"
                << roomName  << "' , '" << description  << "' );");
				
	LOGGER_VERB("Myslq create room: " << req ) ;
	try{
		_DataBase->_Connection->Query( (char* ) req.c_str() );
	
		Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
		if ( res->Next() ){
				idRoom = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		}
	}
	catch ( ... ){
		LOGGER_ERROR("MYSQL failed -> Insert Room");
		_DataBase->_IsInsertingRow = false;
		return 0x00;
	}
    
	_DataBase->_IsInsertingRow = false;
	LOGGER_INFO("Create Room:" << roomName << " :" << idRoom);
	return idRoom;	
}

uint64_t Mysql::createCard ( string cardNumber, uint64_t idUser ){
	uint64_t idCard = 0x00;
	
	if ( getIdFromTagNumber ( cardNumber ) != 0x00 ){
		LOGGER_WARN ( "Card already in base: " << std::hex << cardNumber << ". Request ignore");
		return idCard;
	}
	string req;
	if (  idUser == 0x00 ) 
		req = SSTR ( "INSERT INTO " << _CardTable.name << " ( " << _CardTable.number.value << " , " << _CardTable.iduser.value << " ) VALUES ( '" << cardNumber << "', NULL  );" );
	else
		req = SSTR ( "INSERT INTO " << _CardTable.name << " ( " << _CardTable.number.value << " , " << _CardTable.iduser.value << " ) VALUES ( '" 
			<< cardNumber << "' , " << idUser  << " );" );

	while ( _DataBase->_IsInsertingRow ) usleep( 10 );
	_DataBase->_IsInsertingRow = true;

	//LOGGER_VERB("Mysql create Card " << req );
	
	try{
		_DataBase->_Connection->Query( (char*) req.c_str() );

		Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
		if ( res->Next() ){
				idCard = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		}
	}
	catch ( ... ) {
		LOGGER_ERROR("MYSQL failed -> Insert card");
		_DataBase->_IsInsertingRow = false;
		return idCard;
	}

	_DataBase->_IsInsertingRow = false;
	LOGGER_INFO ( "Card create: " << std::hex << cardNumber << ":" << idUser <<":" << idCard );
	return idCard;
}

uint64_t Mysql::createRecorder( uint64_t idRoom, string addressMac, uint64_t *idRmod, uint64_t *idCmod ){
	if ( getIdRecorderFromMac( addressMac) != 0x00 ) {
		LOGGER_WARN( "Record already exist with Mac. Request ignore");
		return 0;
	}

	uint64_t idRecorder = 0x00;
	*idRmod = 0;
	*idCmod = 0; 
	string req;

	LOGGER_INFO("Create Recorder");
	while ( _DataBase->_IsInsertingRow ) usleep(10);
	_DataBase->_IsInsertingRow = true;
	
	// Create ConnectingModule	
	req = SSTR("INSERT INTO " << _ConnectingModuleTable.name << " ( " << _ConnectingModuleTable.idNetwork.value << ") values ( 0 );");
	//LOGGER_VERB ( "Mysql Create connecting Module: " << req );
	Result* res;	
	try{
		_DataBase->_Connection->Query( ( char*) req.c_str());
		
		res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
		if ( res->Next() ){
			*idCmod = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		}
	}
	catch ( ... ){
		LOGGER_ERROR("MYSQL failed -> Insert ConnectingModule");
		_DataBase->_IsInsertingRow = false;
		return 0x00;
	}

	// Create Recording Module
	req = SSTR("INSERT INTO " << _RecordingTable.name << " ( " <<  _RecordingTable.adressMAC.value << " ) values ( '" << addressMac << "');" );
	//LOGGER_VERB( "Mysql Create Recording Module: " << req );
	
	try{
		_DataBase->_Connection->Query(( char*) req.c_str() );

		res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
		if ( res->Next() ){
			*idRmod = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		}
	}
	catch ( ... ){
		// TODO: Delete ConnectingModule
		LOGGER_ERROR("MYSQL failed -> Insert RecordingModule");
		_DataBase->_IsInsertingRow = false;
		return 0x00;
	}

	// Create recorder
	req = SSTR ( "INSERT INTO " << _RecorderTable.name << "( " << _RecorderTable.idRecordingModule.value << " , " << _RecorderTable.idConnectingModule.value << " , " << _RecorderTable.idRoom.value << " ) values ( "
		<< *idRmod << " , " << *idCmod << " , " << idRoom << " );");
	//LOGGER_VERB("Myslq create recorder: " << req ) ;
	
	try{
		_DataBase->_Connection->Query( (char* ) req.c_str() );

		res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
		if ( res->Next() ){
			idRecorder = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		}
	}
	catch ( ... ) {
		// TODO: Delete ConnectingModule && RecordingModule
		LOGGER_ERROR("MYSQL failed -> Insert _RecorderTable");
		_DataBase->_IsInsertingRow = false;
		return 0x00;
	}
	
	_DataBase->_IsInsertingRow = false;
	LOGGER_INFO("Create Recoder: " << idRecorder << " | idConnectingModule: " << *idCmod << " | idRecordingModule " << *idRmod );
	return idRecorder;
}

uint64_t Mysql::createUserWebSite ( string firstName, string lastName, string password, string email , string DateRegistration){
	uint64_t idUserWebSite = 0;
	
	while ( _DataBase->_IsInsertingRow ) usleep(10);
        _DataBase->_IsInsertingRow = true;
		
	string req = SSTR ( "INSERT INTO " << _UserTable.name << "( " << _UserTable.firstName.value << " , " << _UserTable.lastName.value << " , " << _UserTable.password.value << "," << _UserTable.email.value << " ) values ( '"
			  << firstName << "' , '" << lastName << "' , '" << password << "' , '" << email << "' );");  
    LOGGER_VERB("Mysql create userRecorder : " << req );
 
	_DataBase->_Connection->Query ( (char*) req.c_str());
	Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
    if ( res->Next() ){
			idUserWebSite = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
	}
	req = SSTR ( "INSERT INTO " << _WebsiteUserTable.name << "( " << _WebsiteUserTable.id.value << " , " << _WebsiteUserTable.registration.value << " ) values ( " << idUserWebSite  << " , '" << DateRegistration << "');");  
    LOGGER_VERB("Myslq create UserWebSite: " << req ) ;
	
	_DataBase->_Connection->Query( (char* ) req.c_str() );

	_DataBase->_IsInsertingRow = false;
	
    LOGGER_INFO("Create UserWebSite:" << firstName << "," << lastName << ":" << idUserWebSite );
    return idUserWebSite;
}

uint64_t Mysql::createUserRecorder ( string firstName, string lastName, string password, string email, string dateBegin, string dateEnd){
	uint64_t idUserRecorder = 0;
	
	while ( _DataBase->_IsInsertingRow ) usleep(10);
        _DataBase->_IsInsertingRow = true;

	string req = SSTR ( "INSERT INTO " << _UserTable.name << "( " << _UserTable.firstName.value << " , " << _UserTable.lastName.value << " , " << _UserTable.password.value << "," << _UserTable.email.value << " ) values ( '"
			  << firstName << "' , '" << lastName << "' , '" << password << "' , '" << email << "' );");  
    LOGGER_VERB("Mysql create userRecorder");
 
	try{
		_DataBase->_Connection->Query ( (char*) req.c_str());
		Result *res = _DataBase->_Connection->Query(( char*) "SELECT LAST_INSERT_ID();"  );
		if ( res->Next() ){
				idUserRecorder = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		}
	}
	catch ( ... ) {
		LOGGER_ERROR("Mysql failed -> Insert User Table");
		_DataBase->_IsInsertingRow = false;
		return 0x00;
	}

	req = SSTR ( "INSERT INTO " << _RecorderUserTable.name << "( " << _RecorderUserTable.id.value << " , " << _RecorderUserTable.dateBegin.value << " , " << _RecorderUserTable.dateEnd.value << " ) values ( " << idUserRecorder  << " , '" << dateBegin << "' , '" << dateEnd << "' );");  
	try{
		_DataBase->_Connection->Query ( (char*) req.c_str());
	}
	catch ( ... ) {
		LOGGER_ERROR("Mysql failed -> Insert UserRecorder Table");
		// TODO: DELETE FROM USER TABLE
		_DataBase->_IsInsertingRow = false;
		return 0x00;
	}

	_DataBase->_IsInsertingRow = false;
    LOGGER_INFO("Create UserRecorder:" << firstName << "," << lastName << ":" << idUserRecorder );
	
    return idUserRecorder;
}



/****************  Administration - Updates ****************/
bool Mysql::updateCard ( uint64_t idCard , uint64_t idUser ){
	if ( idCard == 0 )
		return false;
	string req;
	if ( idUser == 0 )
		req = SSTR ( "UPDATE " << _CardTable.name << " SET " << _CardTable.iduser.value << "=NULL" << " WHERE " << _CardTable.idcard.value << "=" << idCard << ";" );
	else	
		req = SSTR ( "UPDATE " << _CardTable.name << " SET " << _CardTable.iduser.value << "=" << idUser << " WHERE " << _CardTable.idcard.value << "=" << idCard << ";" );
	
	bool succes = false;
	while ( _DataBase->_IsInsertingRow ) usleep(10);
	 _DataBase->_IsInsertingRow = true;
	try{
		_DataBase->_Connection->Query(( char*) req.c_str()  );
		succes = (countUpdatedRows() == 1); 	
	}
	catch ( ... ){
		LOGGER_ERROR("MYSQL failed -> Update card");
	}
	
	_DataBase->_IsInsertingRow = false;
	return succes;
}

bool Mysql::updateRecorder ( uint64_t idRecorder , uint64_t idRoom ){
	string req = SSTR ( "UPDATE " << _RecorderTable.name << " SET " << _RecorderTable.idRoom.value << "=" << idRoom << " WHERE " << _RecorderTable.idRecorder.value << "=" << idRecorder << ";" );

	while ( _DataBase->_IsInsertingRow ) usleep(10);
   	_DataBase->_IsInsertingRow = true;
	bool succes = false;
	
	try{
		_DataBase->_Connection->Query(( char*) req.c_str()  );
		succes = ( countUpdatedRows() == 1);
	}
	catch ( ... ){
		LOGGER_ERROR("MYSQL failed -> Update card");
	}
	_DataBase->_IsInsertingRow = false;
	return succes;
}

bool Mysql::updateUserRecorderTable ( uint64_t Id, bool eDBegin, string dateBegin, bool eDEnd, string dateEnd ){
	if ( Id == 0 || (!eDBegin && !eDEnd ) )
		return false;
	
	string req = SSTR ( "UPDATE " << _RecorderUserTable.name << " SET " );
	if ( eDBegin ) 
		req = SSTR ( req << _RecorderUserTable.dateBegin.value << "='" << dateBegin << "'," );
	
	if ( eDEnd ) 
		req = SSTR ( req << _RecorderUserTable.dateEnd.value << "='" << dateEnd << "'," );

	
	req = SSTR ( req.substr(0,req.length()-1) << " WHERE " << _UserTable.id.value << "=" << Id << ";" );
	
	while ( _DataBase->_IsInsertingRow ) usleep(10);
   	_DataBase->_IsInsertingRow = true;
	bool succes = false;
	try{
		_DataBase->_Connection->Query(( char*) req.c_str()  );
		succes = ( countUpdatedRows() == 1);
	}
	catch ( ... ){
		LOGGER_ERROR("MYSQL failed -> Update userRecorder");
	}
	
	_DataBase->_IsInsertingRow = false;
	return succes;
}

bool Mysql::updateRoom ( uint64_t roomId, bool enableName, string name, bool enableDescription, string description ){
	if ( roomId == 0 || (!enableName && !enableDescription) )
		return false;
	
	string req = SSTR ( "UPDATE " << _RoomTable.name << " SET " );
	if ( enableName ) {
		req = SSTR ( req << _RoomTable.roomName.value << "='" << name << "'," );
	}
	if ( enableDescription ) {
		req = SSTR ( req << _RoomTable.description.value << "='" << description << "'," );
	}
	req = SSTR ( req.substr(0,req.length()-1) << " WHERE " << _RoomTable.id.value << "=" << roomId << ";" );
	
	while ( _DataBase->_IsInsertingRow ) usleep(10);
	 _DataBase->_IsInsertingRow = true;
	bool succes = false;
	
	try{
		_DataBase->_Connection->Query(( char*) req.c_str()  );
		succes = (countUpdatedRows() == 1);
	}
	catch ( ... ) {
		LOGGER_ERROR("MYSQL failed -> Update room");
	}
	
	_DataBase->_IsInsertingRow = false;
	return succes;
}

bool Mysql::updateUserTable ( uint64_t Id, bool eFName, string fName, bool eLName, string lName, bool ePwd, string pwd, bool eMail, string email ){
	if ( Id == 0 || (!eFName && !eLName && !ePwd && !eMail ) )
		return false;
	
	string req = SSTR ( "UPDATE " << _UserTable.name << " SET " );
	if ( eFName ) 
		req = SSTR ( req << _UserTable.firstName.value << "='" << fName << "'," );
	
	if ( eLName ) 
		req = SSTR ( req << _UserTable.lastName.value << "='" << lName << "'," );
	
	if ( ePwd )
		req = SSTR ( req << _UserTable.password.value << "='" << pwd << "'," );
	
	if ( eMail )
		req = SSTR ( req << _UserTable.email.value << "='" << email << "'," );

	
	req = SSTR ( req.substr(0,req.length()-1) << " WHERE " << _UserTable.id.value << "=" << Id << ";" );
	
	while ( _DataBase->_IsInsertingRow ) usleep(10);
   	_DataBase->_IsInsertingRow = true;
	
	bool succes =  false;
	try{
		_DataBase->_Connection->Query(( char*) req.c_str()  );
		succes = ( countUpdatedRows() == 1);
	}
	catch( ...  ){
		LOGGER_ERROR("MYSQL failed -> Update User");
	}
	
	_DataBase->_IsInsertingRow = false;
	return succes;
}

/*
*/
