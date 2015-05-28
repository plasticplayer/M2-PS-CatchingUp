// //  main.cpp
// 
// //  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//
#define CFTPSERVER_ENABLE_EVENTS

#include "Mysql.h"
#include "Config.h"
#include <sys/signal.h> 
#include "INIReader.h"
#include "Udp.h"
#include "Ftp.h"
#include "define.h"
#include "Tcp_Socket_Server.h"
#include "CFtpServer.h"
#include "ConfigAppli.h"

#include <getopt.h>
#include <iostream>
using namespace std;
typedef unsigned char BYTE ;


/** .ini file configuration : **/
#define FILE_INI 			"config.ini"
#define INI_CONFIG_SECTION_NAME     	"config"
#define INI_SERVER_TCP_PORT     	"tcpPort"
#define INI_SERVER_UDP_PORT         	"udpPort"
#define INI_SERVER_FTP_PORT		"ftpPort"
#define INI_SERVER_FOLDER_TMP	        "FolderPath_Ftp"
#define INI_SERVER_FOLDER_MEDIA		"FolderPath_Media"
#define INI_SERVER_FTP_MAX_UPLOAD	"FTP_MAX_UPLOAD"
#define INI_SERVER_MYSQL_DATABASE	"SQL_DATABASE"
#define INI_SERVER_MYSQL_HOST		"SQL_HOST"
#define INI_SERVER_MYSQL_USER		"SQL_USER"
#define INI_SERVER_MYSQL_PASSWORD	"SQL_PASSWORD"	
#define INI_ADMIN_PASS			"ADMIN_PASSWORD"
#define INI_SERVER_APPLI_CONF_TCP_PORT	"tcpport_appliconf"

Udp *_Udp = NULL;
Ftp *_Ftp;
Tcp_Socket_Server* _TcpSocketSrv = NULL;
applicationConfiguration CurrentApplicationConfig;

/**
  Structure defining the configuration of the application
 **/



bool loadConfigFromIniFile(applicationConfiguration &conf);
bool loadUdpServer( int port );
bool loadTcpServer( int port );
bool loadFtpServer( int port );
bool loadMysql( );
bool loadAppliConfig( int port );

void signal_handler(int signal_number)
{
	LOGGER_INFO("Closing requested");
	exit(255);
}

int main(int argc, const char * argv[]) {
	/** Registering a signal handler (for CTRL-C) **/
	signal(SIGINT, signal_handler);
	system("echo 7 > /proc/sys/net/ipv4/tcp_fin_timeout");

	/* Setting the default configuration file */
	/* Do not edit this, edit the .ini file */
	CurrentApplicationConfig.iniFileName  = FILE_INI;
	CurrentApplicationConfig.f_Verbose = true;
	CurrentApplicationConfig.f_Debug = false;
	CurrentApplicationConfig.TCP_serverPort = 1901;
	CurrentApplicationConfig.UDP_serverPort = 1902;
	CurrentApplicationConfig.FTP_serverPort = 1903;
	CurrentApplicationConfig.TCP_APPLI_PORT = 1904;
	CurrentApplicationConfig.FolderPathTmp = "/Tmp";
	CurrentApplicationConfig.FTP_maxUpload = 10;
	CurrentApplicationConfig.MysqlHost = "localhost";
	CurrentApplicationConfig.MysqlDatabase = "mydb" ;
	CurrentApplicationConfig.MysqlUser = "priseCours";
	CurrentApplicationConfig.MysqlPassword = "priseCours";


	Logger::Priority loggerLevel = Logger::CONFIG;
	if ( CurrentApplicationConfig.f_Verbose )
		loggerLevel = Logger::VERB;
	if ( CurrentApplicationConfig.f_Debug )
		loggerLevel = Logger::DEBUG;
	
	/* Change directory to be inside the folder of the executable (in case of starting like "bin/app") */
	char *dirsep = (char*) strrchr( argv[0], '/' );
	if( dirsep != NULL )
		*dirsep = 0;
	if(chdir(argv[0]) == 0)
		LOGGER_DEBUG("Changing Directory to "<< argv[0]);
	else
		LOGGER_WARN("Failed to change directory to "<< argv[0]);
	
	LOGGER_START(loggerLevel,"log.log",true);


	LOGGER_INFO("Start Server ");

	/** Loading the configuration from the .ini file **/
	LOGGER_DEBUG("Loading configuration");
	if(!loadConfigFromIniFile(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot load configuration form the .ini file ");
#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}
	else
		LOGGER_DEBUG("Loading configuration OK");
	
	if ( !loadMysql() ){
		LOGGER_ERROR( "Cannot connect to SQL");
		return -1;
	}
	else {
		LOGGER_INFO("MySql connection OK");
	}

	if ( !loadAppliConfig( CurrentApplicationConfig.TCP_APPLI_PORT) ){
		LOGGER_ERROR( "Cannot load AppliConfig");
		return -1;
	}
	
	
	if ( !loadUdpServer( CurrentApplicationConfig.UDP_serverPort ) ){
		LOGGER_ERROR("Cannot Start Udp Server");
#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}

	if ( !loadTcpServer( CurrentApplicationConfig.TCP_serverPort ) ){
		LOGGER_ERROR("Cannot Start TCP Server");
#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}

	if ( !loadFtpServer( CurrentApplicationConfig.FTP_serverPort ) ){
		LOGGER_ERROR("Cannot Start TCP Server");
#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}

	sleep( 5 );
	while ( true ){
		//ConfigAppli::decodeRequest( (char*) "<type>need_rooms</type>" );
		//ConfigAppli::decodeRequest( (char*) "<type>create_rooms</type>\n<name>A465></name><description>test</description>\n<name>A466></name><description></description>");
		//ConfigAppli::decodeRequest( (char*) "<type>need_users_website</type>");
		
		//ConfigAppli::decodeRequest( (char*) "<type>update_rooms</type>\n<id>1</id><description>Update3</description>");
		//ConfigAppli::decodeRequest( (char*) "<type>need_rooms</type>" );
		
		//cout << endl << "/****** GET RECORDERS ******/" << endl;
		//ConfigAppli::getRecorders();
		//cout << endl << "/****** GET Users Recorders ******/" << endl;
		//ConfigAppli::getUsersRecorders();
		//cout << endl << "/****** GET Users WebSite ******/" << endl;
		//ConfigAppli::getUsersWebsite();
		//cout << endl << "/****** GET Rooms ******/" << endl;
		//ConfigAppli::getRooms();
		//cout << endl << "/****** GET Cards ******/" << endl;
		//ConfigAppli::getCards();
		sleep(60);
		
	}
	return 0;
}

bool loadConfigFromIniFile(applicationConfiguration& conf)
{
	/* Openning the .ini configuration file */
	INIReader reader(conf.iniFileName);
	if(reader.ParseError() < 0)
	{
		LOGGER_ERROR("Error parsing .ini file : ("<< conf.iniFileName << ") parser error : " << reader.ParseError());
		return false;
	}

	string s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_FOLDER_TMP,"/Temp");
	conf.FolderPathTmp = s.c_str();
	
	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_FOLDER_MEDIA,"/Temp");
	conf.FolderPathMedia = s.c_str();

	s= reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_FTP_MAX_UPLOAD,"10");
	conf.FTP_maxUpload = strtoull(s.c_str(),NULL,10);

	s = reader.Get(INI_CONFIG_SECTION_NAME, INI_SERVER_FTP_PORT,"1901");
	conf.FTP_serverPort = strtoull(s.c_str(),NULL,10);

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_UDP_PORT,"1902");
	conf.UDP_serverPort = strtoull(s.c_str(), NULL, 10);

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_TCP_PORT,"1903");
	conf.TCP_serverPort = strtoull(s.c_str(), NULL, 10);

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_APPLI_CONF_TCP_PORT,"1904");
	conf.TCP_APPLI_PORT = strtoull(s.c_str(), NULL, 10);
	
	s = reader.Get(INI_CONFIG_SECTION_NAME , INI_SERVER_MYSQL_HOST , conf.MysqlHost);
	conf.MysqlHost = s.c_str();
	
	s = reader.Get(INI_CONFIG_SECTION_NAME , INI_SERVER_MYSQL_DATABASE , conf.MysqlDatabase);
	conf.MysqlDatabase = s.c_str();
	
	s = reader.Get(INI_CONFIG_SECTION_NAME , INI_SERVER_MYSQL_USER , conf.MysqlUser);
	conf.MysqlUser = s.c_str();
	
	s = reader.Get(INI_CONFIG_SECTION_NAME , INI_SERVER_MYSQL_PASSWORD , conf.MysqlPassword);
	conf.MysqlPassword = s.c_str();

	s = reader.Get(INI_CONFIG_SECTION_NAME, INI_ADMIN_PASS, "pass");
	conf.AdminPassword = s.c_str();

	LOGGER_CONFIG("Loading configuration from " << conf.iniFileName);
	LOGGER_CONFIG("=================START CONFIGURATION======================");
	LOGGER_CONFIG("-------------     FTP CONFIGURATION      -----------------");
	LOGGER_CONFIG("Port            : \t" << conf.FTP_serverPort);
	LOGGER_CONFIG("Path FTP        : \t" << conf.FolderPathTmp );
	LOGGER_CONFIG("Path MEDIA      : \t" << conf.FolderPathMedia );
	LOGGER_CONFIG("Max Upload      : \t" << conf.FTP_maxUpload);
	LOGGER_CONFIG("-------------    NETWORK CONFIGURATION   -----------------");
	LOGGER_CONFIG("SERVER TCP PORT : \t" << conf.TCP_serverPort );
	LOGGER_CONFIG("CONFIG TCP PORT : \t" << conf.TCP_APPLI_PORT );
	LOGGER_CONFIG("SERVER UDP PORT : \t" << conf.UDP_serverPort );
	LOGGER_CONFIG("-------------    DATABASE CONFIGURATION   -----------------");
	LOGGER_CONFIG("HOST            : \t" << conf.MysqlHost );
	LOGGER_CONFIG("DATABASE	       : \t" << conf.MysqlDatabase );	
	LOGGER_CONFIG("USER            : \t" << conf.MysqlUser );
	LOGGER_CONFIG("PASSWORD        : \t" << conf.MysqlPassword );
	LOGGER_CONFIG("===================END CONFIGURATION======================");

	return true;
}


bool loadUdpServer( int port ){
	_Udp = new Udp( port );
	return true;
}

bool loadFtpServer( int port ){
	_Ftp = new Ftp( port );
	return true;
}

bool loadTcpServer( int port ){
	_TcpSocketSrv = new Tcp_Socket_Server( port );
	_TcpSocketSrv->start();
	return true;
}

bool loadAppliConfig( int port ){
	new ConfigAppli( port );
	bool res = ConfigAppli::createSocket();
	return res;
}

bool loadMysql( ){	
	Mysql *mysql = new Mysql( );
	if ( ! mysql->connect(CurrentApplicationConfig.MysqlHost , CurrentApplicationConfig.MysqlDatabase , CurrentApplicationConfig.MysqlUser , CurrentApplicationConfig.MysqlPassword ) )
		return false;
//	Mysql::createRoom("A465","");
//	Mysql::createRecorder( 1 , SSTR("b827eb115727"));
//	Mysql::createUserRecorder ( "GHILES" , "MOSTAFAOUI" , "PASSWD" , "gm@ucp.net" , "2015-01-01" , "2016-01-01" )
//	Mysql::createCard( 0x3124, 1 );
//	uint64_t idUserRecorder = Mysql::getIdUserRecorderFromTag(0x3124);
//	cout << std::hex << idUserRecorder << endl;
/*	uint64_t idRecording = Mysql::createRecording(1,1);
	if ( idRecording != 0x00 )
		Mysql::stopRecording(idRecording);
*/	return true;
}
