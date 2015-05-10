#ifndef __defines_application_Configuration_
#define __defines_application_Configuration_

#include <stdint.h>
typedef struct applicationConfiguration
{
	string iniFileName;
	string FolderPathTmp;
	string FolderPathMedia;
	string MysqlHost;
	string MysqlDatabase;
	string MysqlUser;
	string MysqlPassword;
	uint32_t TCP_serverPort;
	uint32_t UDP_serverPort;
	uint32_t FTP_serverPort;
	uint32_t FTP_maxUpload;
	int f_Debug; // Used as bool
	int f_Verbose; // Used as bool
} applicationConfiguration;

extern applicationConfiguration CurrentApplicationConfig;

#endif
