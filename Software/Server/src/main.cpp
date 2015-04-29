// //  main.cpp
// 
// //  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//
#define CFTPSERVER_ENABLE_EVENTS


#include "Config.h"
#include <sys/signal.h> 
#include "INIReader.h"
#include "../header/Udp.h"
#include "../header/Ftp.h"
#include "../header/define.h"
#include "../header/Tcp_Socket_Server.h"
#include "../header/CFtpServer.h"

#pragma message ( "C Preprocessor got here!" )
#include <getopt.h>
#include <iostream>
using namespace std;
typedef unsigned char BYTE ;


/** .ini file configuration : **/
#define FILE_INI 					"config.ini"
#define INI_CONFIG_SECTION_NAME     "config"
#define INI_SERVER_TCP_PORT     	"tcpPort"
#define INI_SERVER_UDP_PORT         "udpPort"
#define INI_SERVER_FTP_PORT			"ftpPort"
#define INI_SERVER_FOLDER	        "FolderPath"
#define INI_SERVER_FTP_MAX_UPLOAD	"FTP_MAX_UPLOAD"	

Udp *_Udp = NULL;
Ftp *_Ftp;
Tcp_Socket_Server* _TcpSocketSrv = NULL;
/**
  Structure defining the configuration of the application
 **/



bool loadConfigFromIniFile(applicationConfiguration &conf);
bool loadUdpServer( int port );
bool loadTcpServer( int port );
bool loadFtpServer( int port );


void signal_handler(int signal_number)
{
	LOGGER_INFO("Closing requested");
	exit(255);
}

int main(int argc, const char * argv[]) {
	/** Registering a signal handler (for CTRL-C) **/
	signal(SIGINT, signal_handler);
	/* Setting the default configuration file */
	/* Do not edit this, edit the .ini file */
	CurrentApplicationConfig.iniFileName  = FILE_INI;
	CurrentApplicationConfig.f_Verbose = true;
	CurrentApplicationConfig.f_Debug = true;
	CurrentApplicationConfig.TCP_serverPort = 1903;
	CurrentApplicationConfig.UDP_serverPort = 1902;
	CurrentApplicationConfig.FTP_serverPort = 1904;
	CurrentApplicationConfig.FolderPath = "/Temp";
	CurrentApplicationConfig.FTP_maxUpload = 10;
	Logger::Priority loggerLevel = Logger::CONFIG;
	if ( CurrentApplicationConfig.f_Verbose )
		loggerLevel = Logger::VERB;
	if ( CurrentApplicationConfig.f_Debug )
		loggerLevel = Logger::DEBUG;
	
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
	
	while ( true ){
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

	string s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_FOLDER,"/Temp");
	conf.FolderPath = s.c_str();

	s= reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_FTP_MAX_UPLOAD,"10");
	conf.FTP_maxUpload = strtoull(s.c_str(),NULL,10);

	s = reader.Get(INI_CONFIG_SECTION_NAME, INI_SERVER_FTP_PORT,"1904");
	conf.FTP_serverPort = strtoull(s.c_str(),NULL,10);

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_UDP_PORT,"1902");
	conf.UDP_serverPort = strtoull(s.c_str(), NULL, 10);

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_TCP_PORT,"1903");
	conf.TCP_serverPort = strtoull(s.c_str(), NULL, 10);

	LOGGER_CONFIG("Loading configuration from " << conf.iniFileName);
	LOGGER_CONFIG("=================START CONFIGURATION======================");
	LOGGER_CONFIG("-------------     FTP CONFIGURATION      -----------------");
	LOGGER_CONFIG("Port            : \t" << conf.FTP_serverPort);
	LOGGER_CONFIG("Path            : \t" << conf.FolderPath );
	LOGGER_CONFIG("Max Upload      : \t" << conf.FTP_maxUpload);
	LOGGER_CONFIG("-------------    NETWORK CONFIGURATION   -----------------");
	LOGGER_CONFIG("SERVER TCP PORT : \t" << conf.TCP_serverPort );
	LOGGER_CONFIG("SERVER UDP PORT : \t"  << conf.UDP_serverPort );
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
