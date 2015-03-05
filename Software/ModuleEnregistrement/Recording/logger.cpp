#include <string>
#include <fstream>
#include <iostream>
#include "logger.h"
#include <ctime>
#include <pthread.h>
#include <sys/types.h>

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
    "DEBUG",
    "CONFIG",
    "INFO",
    "WARNING",
    "ERROR"
};

Logger Logger::instance;

Logger::Logger() : active(false) {}

void Logger::Start(Priority minPriority, const string& logFile)
{
    instance.active = true;
    instance.minPriority = minPriority;
    if (logFile.compare("") != 0)
    {
        instance.fileStream.open(logFile.c_str());
    }
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
        // Current date/time based on current system
        time_t now = time(0);

        // Convert now to tm struct for local timezone
        tm* localtm = localtime(&now);
        char buffer [80];
        strftime (buffer,80,"%F %T",localtm);

        // identify current output stream
        ostream& stream
            = instance.fileStream.is_open() ? instance.fileStream : std::cout;

        stream  << "[" << PRIORITY_NAMES[priority] <<"]"
                << "[" <<  buffer <<"]";

        if(instance.minPriority <= DEBUG)
            stream  << "[" << pthread_self() << "]";

        stream << " : "
                << message
                << endl;

    }
}

