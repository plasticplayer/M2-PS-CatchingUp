#ifndef __define_applicationconfiguration
#define __define_applicationconfiguration
#include <stdint.h>
#include <string.h>

typedef struct applicationConfiguration
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

extern applicationConfiguration CurrentApplicationConfig;

#endif
