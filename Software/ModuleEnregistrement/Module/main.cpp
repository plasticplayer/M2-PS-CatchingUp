//
//  main.cpp
//  ProtocolCommunication
//
//  Created by maxime max on 28/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "Config.h"

#include "INIReader.h"
#include "Nrf24.h"
#include "Communication.h"
#include "CallBack.h"
#include "StillCamera.h"
#include "Udp.h"


/** .ini file configuration : **/
#define FILE_INI "config.ini"
#define INI_CONFIG_SECTION_NAME     "config"
#define INI_SERVER_ID               "server"
#define INI_CLIENT_ID               "client"
#define INI_SERVER_UDP_PORT         "udpPort"

typedef unsigned char BYTE ;

using namespace std;

/**
    Structure defining the configuration of the application
**/
typedef struct applicationConfiguration
{
	string iniFileName;
	uint64_t NRF24_ServerId;
	uint64_t NRF24_ClientId;
	uint32_t UDP_serverPort;
} applicationConfiguration;
static applicationConfiguration CurrentApplicationConfig;



/**
**/
bool loadConfigFromIniFile(applicationConfiguration& conf);
bool startNRF24Communication(applicationConfiguration& conf);
bool startUDPCommunication(applicationConfiguration& conf);
bool startTCPCommunication(applicationConfiguration& conf);
bool startCAM(applicationConfiguration& conf);
bool startWebCam(applicationConfiguration& conf);


/** Entry point **/
int main(int argc, char * argv[])
{

	/* TODO : test arg to enable debug mode and others settings */

	/* Starting the Logger first */
	LOGGER_START(Logger::VERB,"");

	/* Change directory to be inside the folder of the executable (in case of starting like "bin/app") */
	char *dirsep = strrchr( argv[0], '/' );
	if( dirsep != NULL )
		*dirsep = 0;
	if(chdir(argv[0]) == 0)
		LOGGER_DEBUG("Changing Directory to "<< argv[0]);
	else
		LOGGER_WARN("Failed to change directory to "<< argv[0]);

	/* Test if the user is currently the root => to use the BCM we must be root */
	if(getuid() != 0)
	{
		LOGGER_ERROR("Error this program must be started as root");
		LOGGER_ERROR("Current user id : " << getuid());
		return -1;
	}

	/* Init the bcm for the video stream */
	bcm_host_init();
	/* Register our application with the logging system */
	vcos_log_register(APP_NAME,VCOS_LOG_CATEGORY);

	/* Printf appName and application version */
	LOGGER_INFO("Program name : " << APP_NAME << " version : " << APP_VERSION);

	/* Init of the pseudo-random generator from the current time */
	srand(time(NULL));

	/* Setting the default ini file */
	CurrentApplicationConfig.iniFileName  = FILE_INI;

	if(!loadConfigFromIniFile(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot load configuration form the .ini file ");
		LOGGER_ERROR("Exiting");
		//return -1;
	}

	if(!startNRF24Communication(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start NRF24 ");
		LOGGER_ERROR("Exiting");
		//return -1;
	}

	if(!startUDPCommunication(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start UDP communication ");
		LOGGER_ERROR("Exiting");
		//return -1;
	}

	if(!startTCPCommunication(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start TCP communication ");
		LOGGER_ERROR("Exiting");
		//return -1;
	}

	if(!startCAM(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start Raspberry Camera ");
		LOGGER_ERROR("Exiting");
		//return -1;
	}

	if(!startWebCam(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start Webcam ");
		LOGGER_ERROR("Exiting");
		//return -1;
	}



	while ( char c = getchar() )
	{

		if(c == 'a' )
		{
			appairage();
			fflush(stdin);
		}
		if(c == 'q' )
		{
			break;
		}
	}
	return EXIT_SUCCESS;
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

	string s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_ID,"");
	conf.NRF24_ServerId = strtoull(s.c_str(), NULL, 16);

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_CLIENT_ID,"");
	conf.NRF24_ClientId = strtoull(s.c_str(), NULL, 16);

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_UDP_PORT,"1902");
	conf.UDP_serverPort = strtoull(s.c_str(), NULL, 10);

	LOGGER_INFO("Loading configuration from "<<conf.iniFileName);
	LOGGER_INFO("=================START CONFIGURATION======================");
	LOGGER_INFO("-------------NRF24 WIRELESS CONFIGURATION-----------------");
	LOGGER_INFO("ID NRF24 RECORDER : \t0x" <<  std::uppercase <<std::hex << conf.NRF24_ServerId );
	LOGGER_INFO("ID NRF24 CONTROL : \t0x"  << std::uppercase << std::hex << conf.NRF24_ClientId );
	LOGGER_INFO("-------------    NETWORK CONFIGURATION   -----------------");
	LOGGER_INFO("SERVER UDP PORT : \t"<<conf.UDP_serverPort );
	LOGGER_INFO("===================END CONFIGURATION======================");

	return true;
}

bool startNRF24Communication(applicationConfiguration& conf)
{
	Nrf24* nrfCom = new Nrf24();
	if(!nrfCom->isOk())
	{
		LOGGER_ERROR("Error NRF24 not available !");
		return false;
	}

	nrfCom->updateAddr(conf.NRF24_ClientId, conf.NRF24_ServerId,  false );
	nrfCom->start();
	setComm( nrfCom );

	nrfCom->setFunction(&getAck,ACK);
	nrfCom->setFunction(&authentificationRequest, AUTH_ASK);
	nrfCom->setFunction(&statusRequest, STATUS_ASK);
	nrfCom->setFunction(&statusAns, STATUS_ANS);
	nrfCom->setFunction(&ControlRequest, ACTIVATE_RECORDING);
	nrfCom->setFunction(&AppariagePoolingStep1, POOL_PARK_ADDR_ANS);
	nrfCom->setFunction(&AppariageValidConfig, POOL_VALIDATE_CONFIG);
	nrfCom->setTransactionErrorFunction(&transactionError);
	return true;
}

bool startUDPCommunication(applicationConfiguration& conf)
{
	Udp *udp = new Udp( conf.UDP_serverPort );

	LOGGER_INFO("Trying to find a server on port " << conf.UDP_serverPort );
	InfoTCP *info = udp->getInfoSrv();

	if ( info != NULL )
	{
		LOGGER_INFO("A server has been found : " << info->ipAddress << ":" <<  info->port );
		return true;
	}
	else
	{
		LOGGER_ERROR("Cannot join a server on the "<< conf.UDP_serverPort << " port !");
		return false;
	}
}


bool startTCPCommunication(applicationConfiguration& conf)
{
	return true;
}
bool startCAM(applicationConfiguration& conf)
{
    if(StillCamera::isInUse())
    {
        LOGGER_ERROR("Can't create the camera it's already in use !");
        return false;
    }
    bool resultTest = true;
	RASPISTILL_STATE state;
	default_status(&state);
	state.timeout = 0;                  /// Time between snapshoots, in ms
	state.verbose = 0;
	state.width = 400;                          /// Requested width of image
	state.height = 300;                         /// requested height of image
	state.preview_parameters.wantPreview=0;

	StillCamera cam(state);
	uint8_t * buff = NULL;
	int sizeBuff = 0;
	if(cam.init())
	{
		resultTest = cam.takeSnapshot(&buff,&sizeBuff);
		usleep(1000);
	}
	else
        resultTest = false;
	cam.destroy();
    free (buff);
	return resultTest;
}
bool startWebCam(applicationConfiguration& conf)
{
	return true;
}
