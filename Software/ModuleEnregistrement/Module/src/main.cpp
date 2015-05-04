//  main.cpp
//  ProtocolCommunication
//
//  Created by maxime max on 28/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//


//#define EXIT_ON_ERROR 1
#define ENABLE_CAMERA 1
//#define ENABLE_COMM 1

#include "Config.h"
#include "define.h"
#include "INIReader.h"
#include "Nrf24.h"
#include "Communication.h"
#include "Linker.h"
#include "StillCamera.h"
#include "webCam.h"

#include <getopt.h>

/** .ini file configuration : **/
#define FILE_INI "config.ini"
#define INI_CONFIG_SECTION_NAME     "config"
#define INI_SERVER_ID               "server"
#define INI_CLIENT_ID               "client"
#define INI_SERVER_UDP_PORT         "udpPort"
#define INI_SERVER_UDP_INTERFACE    "iface"
#define INI_SERVER_DATA_PATH	    "data_path"

#define INI_CAMERA_SECTION          "CAMERA"
#define INI_CAMERA_RES_WIDTH        "WIDTH"
#define INI_CAMERA_RES_HEIGHT       "HEIGHT"
#define INI_CAMERA_DELAY_SEC        "DELAY"

#define INI_WEBCAM_SECTION          "WEBCAM"
#define INI_WEBCAM_RES_WIDTH        "WIDTH"
#define INI_WEBCAM_RES_HEIGHT       "HEIGHT"
#define INI_WEBCAM_FPS              "FPS"
#define INI_WEBCAM_DEVICE           "DEV"
#define INI_WEBCAM_SPLIT	    "SPLIT"

typedef unsigned char BYTE ;

using namespace std;

/**
    Structure defining the configuration of the application
**/
/*typedef struct applicationConfiguration
{
	string iniFileName;
	string Data_path;
	uint64_t NRF24_ServerId;
	uint64_t NRF24_ClientId;
	uint32_t UDP_serverPort;
	string UDP_interface;
	int f_Debug; // Used as bool
	int f_Verbose; // Used as bool
} applicationConfiguration;
*/



/** Functions **/

void signal_handler(int signal_number);
bool parseCommandLine(int argc, char * argv[], applicationConfiguration& conf);

bool loadConfigFromIniFile(applicationConfiguration& conf);
bool startNRF24Communication(applicationConfiguration& conf);
bool startUDPCommunication(applicationConfiguration& conf);
bool startTCPCommunication(applicationConfiguration& conf);
bool startCAM(applicationConfiguration& conf);
bool startWebCam(applicationConfiguration& conf);
bool startServo(applicationConfiguration& conf);

/** Globaly available variables **/
//static
applicationConfiguration CurrentApplicationConfig;
static int CurrentIdRecording;

/*** CODE ***/

void signal_handler(int signal_number)
{
	LOGGER_INFO("Closing requested");

	if(Webcam::isInUse())
	{
		LOGGER_DEBUG("Closing webcam ");
		Webcam * w = Webcam::getWebcam();
		if(w->isRecording())
		{
			LOGGER_DEBUG("Stopping recording webcam ");
			w->stopRecording();
		}
		w->close();
	}
	exit(255);
}


/** Entry point **/
int main(int argc, char * argv[])
{
	/** Registering a signal handler (for CTRL-C) **/
	signal(SIGINT, signal_handler);

	/* Setting the default configuration file */
	/* Do not edit this, edit the .ini file */
	CurrentApplicationConfig.iniFileName  = FILE_INI;
	CurrentApplicationConfig.f_Verbose = false;
	CurrentApplicationConfig.f_Debug = false;
	CurrentApplicationConfig.UDP_interface = "eth0";
	CurrentApplicationConfig.UDP_serverPort = 1902;
	CurrentApplicationConfig.Data_path	= "/tmp";
	CurrentIdRecording = 0;

	/** parsing Command line arguments **/
	parseCommandLine(argc,argv,CurrentApplicationConfig);

	Logger::Priority loggerLevel = Logger::CONFIG;
	if(CurrentApplicationConfig.f_Verbose)
		loggerLevel = Logger::VERB;
	else if(CurrentApplicationConfig.f_Debug)
		loggerLevel = Logger::DEBUG;

	/* Change directory to be inside the folder of the executable (in case of starting like "bin/app") */
	char *dirsep = strrchr( argv[0], '/' );
	if( dirsep != NULL )
		*dirsep = 0;
	if(chdir(argv[0]) == 0)
		LOGGER_DEBUG("Changing Directory to "<< argv[0]);
	else
		LOGGER_WARN("Failed to change directory to "<< argv[0]);

	/* Starting the Logger first */
	LOGGER_START(loggerLevel,"log.log",true);


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


	LOGGER_DEBUG("Init Recording");
	if ( !initRecording() )
	{
		LOGGER_INFO("Can't init Recordings !");
	}
	else
		LOGGER_DEBUG("Recording OK");


#ifdef ENABLE_COMM
	LOGGER_DEBUG("Starting NRF24 test");
	if(!startNRF24Communication(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start NRF24 ");

#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}
	else
		LOGGER_DEBUG("NRF24 OK");

	LOGGER_DEBUG("Starting UDP test");
	if(!startUDPCommunication(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start UDP communication ");

#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}
	else
		LOGGER_DEBUG("UDP OK");

	LOGGER_DEBUG("Starting Ftp test");
	if ( !ftpCheck() )
	{
		LOGGER_INFO("Can't init Ftp !");
#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}
	else
	{
		LOGGER_DEBUG("Ftp OK");
		ftpSenderStart();
	}
	LOGGER_DEBUG("Starting TCP test");
	if(!startTCPCommunication(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start TCP communication ");
#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}
	else
	{
		LOGGER_DEBUG("TCP OK");

	}
#endif

#ifdef ENABLE_CAMERA
	LOGGER_DEBUG("Starting Camera test");
	if(!startCAM(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start Raspberry Camera ");
#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}
	else
		LOGGER_DEBUG("Camera OK");

	LOGGER_DEBUG("Starting webCam test");
	if(!startWebCam(CurrentApplicationConfig))
	{
		LOGGER_ERROR("Cannot start Webcam ");
#ifdef EXIT_ON_ERROR
		LOGGER_ERROR("Exiting");
		return -1;
#endif
	}
	else
		LOGGER_DEBUG("WebCam OK");
#endif


	while ( char c = getchar() )
	{
		switch ( c )
		{
		case 'a':
			LOGGER_INFO("Force NRF paring");
			ParringNrf();
			break;
		case 'z':
			LOGGER_INFO("Force Start Recording");
			forceStartRecording();
			break;
		case 'e':
			LOGGER_INFO("Force Stop Recording");
			forceStopRecording();
			break;
		case 'q':
			break;
		}
		fflush(stdin);
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

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_UDP_INTERFACE,"eth0");
	conf.UDP_interface = s;

	s = reader.Get(INI_CONFIG_SECTION_NAME,INI_SERVER_DATA_PATH,"/tmp");
	conf.Data_path = s;



	s = reader.Get(INI_CAMERA_SECTION,INI_CAMERA_RES_WIDTH,"2592");
	conf.camera.width = strtoull(s.c_str(), NULL, 10);

	s = reader.Get(INI_CAMERA_SECTION,INI_CAMERA_RES_HEIGHT,"1944");
	conf.camera.height = strtoull(s.c_str(), NULL, 10);

	s = reader.Get(INI_CAMERA_SECTION,INI_CAMERA_DELAY_SEC,"1");
	conf.camera.delay = strtoull(s.c_str(), NULL, 10);


    s = reader.Get(INI_WEBCAM_SECTION,INI_WEBCAM_DEVICE,"/dev/video0");
    conf.webcam.device = s;

	s = reader.Get(INI_WEBCAM_SECTION,INI_WEBCAM_RES_WIDTH,"800");
	conf.webcam.width = strtoull(s.c_str(), NULL, 10);

	s = reader.Get(INI_WEBCAM_SECTION,INI_WEBCAM_RES_HEIGHT,"600");
	conf.webcam.height = strtoull(s.c_str(), NULL, 10);

	s = reader.Get(INI_WEBCAM_SECTION,INI_WEBCAM_FPS,"30");
	conf.webcam.fps = strtoull(s.c_str(), NULL, 10);

	s = reader.Get(INI_WEBCAM_SECTION,INI_WEBCAM_SPLIT,"600");
	conf.webcam.split = strtoull(s.c_str(), NULL, 10);


	LOGGER_CONFIG("Loading configuration from "<<conf.iniFileName);
	LOGGER_CONFIG("=================START CONFIGURATION======================");
	LOGGER_CONFIG("-------------NRF24 WIRELESS CONFIGURATION-----------------");
	LOGGER_CONFIG("ID NRF24 RECORDER   : \t0x" <<  std::uppercase <<std::hex << conf.NRF24_ServerId );
	LOGGER_CONFIG("ID NRF24 CONTROL    : \t0x"  << std::uppercase << std::hex << conf.NRF24_ClientId );
	LOGGER_CONFIG("-------------    NETWORK CONFIGURATION   -----------------");
	LOGGER_CONFIG("NETWORK INTERFACE   : \t "<< conf.UDP_interface);
	LOGGER_CONFIG("SERVER UDP PORT     : \t "<< conf.UDP_serverPort );
	LOGGER_CONFIG("-------------     MEDIA CONFIGURATION    -----------------");
	LOGGER_CONFIG("PATH                : \t "<< conf.Data_path);
	LOGGER_CONFIG("-------------    CAMERA CONFIGURATION    -----------------");
	LOGGER_CONFIG("RESOLUTION          : \t ["<< conf.camera.width << " x "<< conf.camera.height << "]");
	LOGGER_CONFIG("DELAY (SEC)         : \t "<< conf.camera.delay );
	LOGGER_CONFIG("-------------    WEBCAM CONFIGURATION    -----------------");
	LOGGER_CONFIG("DEVICE              : \t "<< conf.webcam.device );
	LOGGER_CONFIG("RESOLUTION          : \t ["<< conf.webcam.width << " x "<< conf.webcam.height << "]");
	LOGGER_CONFIG("FRAME RATE          : \t "<< conf.webcam.fps );
	LOGGER_CONFIG("SPLITTING EACH      : \t " << conf.webcam.split << " seconds");
	LOGGER_CONFIG("===================END CONFIGURATION======================");

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
	LOGGER_VERB("NRF24 Connection OK");
	LOGGER_VERB("NRF24 Updating Adresses");
	nrfCom->updateAddr(conf.NRF24_ClientId, conf.NRF24_ServerId,  false );
	initNrfCallBacks();
	LOGGER_VERB("NRF24 Starting listenning thread");
	nrfCom->start();

	return true;
}

bool startUDPCommunication(applicationConfiguration& conf)
{
	Udp *udp = new Udp( conf.UDP_interface ,conf.UDP_serverPort );
	initUdpCallBacks();
	udp->start();

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
	if ( Udp::_TcpInfo == NULL || Udp::_TcpInfo->port == -1 )
		return false;

	Tcp* tcp = new Tcp(Udp::_TcpInfo);
	initTcpCallBacks();
	tcp->start();

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
	state.timeout = 0;                  /// Time between snapshots, in ms
	state.verbose = 0;
	state.width = conf.camera.width;                          /// Requested width of image
	state.height = conf.camera.height;                         /// requested height of image
	state.preview_parameters.wantPreview=0;

	StillCamera * cam = new StillCamera(state);
	uint8_t * buff = NULL;
	int sizeBuff = 0;
	if(cam->init())
	{
		resultTest = cam->takeSnapshot(&buff,&sizeBuff);
		//usleep(1000);
	}
	else
		resultTest = false;
	//cam.destroy();
	free (buff);
	return resultTest;
}

bool startWebCam(applicationConfiguration& conf)
{
	if(Webcam::isInUse())
	{
		LOGGER_ERROR("Can't create the webcam it's already in use !");
		return false;
	}

	bool resultTest = false ;

	if(!Webcam::isInUse())
	{
		Webcam * cam = new Webcam(conf.webcam.device,conf.webcam.width, conf.webcam.height,conf.webcam.fps);
		cam->setSplitTime(conf.webcam.split);
		resultTest = cam->testWebcam();
		//cam->startRecording(CurrentApplicationConfig.Data_path);
	}


	return resultTest;
}

bool parseCommandLine(int argc, char * argv[], applicationConfiguration& conf)
{
	/* **************************
	*** ARGS ***
	*************************** */
	static struct option long_options[] =
	{
		{"debug",no_argument    ,&CurrentApplicationConfig.f_Debug,     'd'},
		{"verbose",no_argument  ,&CurrentApplicationConfig.f_Verbose,   'v'},
		{0,0,0,0}
	};
	opterr = 0;
	int c;
	int option_index =0;

	while ((c = getopt_long(argc,argv,"dv",long_options,&option_index)) != -1)
	{
		switch (c)
		{
		case 0:
			if (long_options[option_index].flag != 0)
				break;
			if (strcmp(long_options[option_index].name,"debug") == 0)
				CurrentApplicationConfig.f_Debug = true;
			else if (strcmp(long_options[option_index].name,"verbose") == 0)
				CurrentApplicationConfig.f_Verbose = true;
			else
			{
				printf("Option : %s ",long_options[option_index].name);
				if (optarg)
					printf("with arg : %s",optarg);
				printf("\n");
			}
			break;
			break;
		case 'd':
			CurrentApplicationConfig.f_Debug = true;
			break;
		case 'v':
			CurrentApplicationConfig.f_Verbose = true;
			break;
		case '?':
			break;
		default :
			return false;
		}
	}
	if (optind < argc)
	{
		printf("Argument unknown : \n");
		while (optind < argc)
			printf("%s \n",argv[optind++]);
	}
	return true;
}
