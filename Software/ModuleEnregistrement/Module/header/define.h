#ifndef __define_applicatiR
#define __define_applicationconfiguration
#include <stdint.h>
#include <string.h>

struct _cameraConfiguration
{
	unsigned int width;
	unsigned int height;
	unsigned int delay;

};
struct _webcamConfiguration
{
    string device;
	unsigned int width;
	unsigned int height;
	unsigned int fps;
	unsigned int split;
};

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
	struct _cameraConfiguration camera;
	struct _webcamConfiguration webcam;
} applicationConfiguration;

extern applicationConfiguration CurrentApplicationConfig;

#endif
