#include <stdint.h>
typedef struct applicationConfiguration
{
	string iniFileName;
	string FolderPath;
	uint32_t TCP_serverPort;
	uint32_t UDP_serverPort;
	uint32_t FTP_serverPort;
	uint32_t FTP_maxUpload;
	int f_Debug; // Used as bool
	int f_Verbose; // Used as bool
} applicationConfiguration;

static applicationConfiguration CurrentApplicationConfig;
