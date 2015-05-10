#include "../header/Config.h"
#include <string>
#include <fstream>
#include <iostream>
#include "../header/logger.h"
#include <ctime>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;
// --------------------------------------
// function implementations
// --------------------------------------


// --------------------------------------
// static members initialization
// --------------------------------------

const string Logger::PRIORITY_NAMES[] =
{
	"VERBOSE",
	"DEBUG  ",
	"CONFIG ",
	"INFO   ",
	"WARNING",
	"ERROR  "

};

Logger Logger::instance;

Logger::Logger() : active(false) {}

void Logger::Start(Priority minPriority, const string& logFile,bool outputBoth)
{
    instance.active = true;
	instance.outputBoth = outputBoth;
	instance.minPriority = minPriority;
	if (logFile.compare("") != 0)
	{
		instance.fileStream.open(logFile.c_str());
	}
	LOGGER_INFO("Starting logger in "<< PRIORITY_NAMES[minPriority] << " mode !");

}
void Logger::Start(Priority minPriority, const string& logFile)
{
	instance.active = true;
	instance.outputBoth = false;
	instance.minPriority = minPriority;
	if (logFile.compare("") != 0)
	{
		instance.fileStream.open(logFile.c_str());
	}
	LOGGER_INFO("Starting logger in "<< PRIORITY_NAMES[minPriority] << " mode !");
}

void Logger::Stop()
{
	instance.active = false;
	if (instance.fileStream.is_open())
	{
		instance.fileStream.close();
	}
}

void Logger::Write(Priority priority, const string& message)
{
	if (instance.active && priority >= instance.minPriority)
	{
		char            fmt[64], buf[64];
		struct timeval  tv;
		struct tm       *tm;

		gettimeofday(&tv, NULL);
		if((tm = localtime(&tv.tv_sec)) != NULL)
		{
			/* Print to uS resolution */
			//strftime(fmt, sizeof fmt, "%Y-%m-%d %H:%M:%S.%%06u", tm);
			//  snprintf(buf, sizeof buf, fmt, tv.tv_usec);
			/* Print to mS resolution */
			strftime(fmt, sizeof fmt, "%Y-%m-%d %H:%M:%S.%%03u", tm);
			snprintf(buf, sizeof buf, fmt, (tv.tv_usec/1000));
		}
		// identify current output stream
		ostream& stream
			= instance.fileStream.is_open() ? instance.fileStream : std::cout;



		stream  << "[" << PRIORITY_NAMES[priority] <<"]"
				<< "[" <<  buf <<"]";

		if(instance.minPriority <= DEBUG)
			stream  << "[" << pthread_self() << "]";

		stream << " : "
			   << message
			   << endl;

		if(instance.outputBoth && instance.fileStream.is_open())
		{

			std::cout  << "[" << PRIORITY_NAMES[priority] <<"]"
					<< "[" <<  buf <<"]";

			if(instance.minPriority <= DEBUG)
				std::cout  << "[" << pthread_self() << "]";

			std::cout << " : "
				   << message
				   << endl;
		}

	}
}

