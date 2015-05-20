#define APP_NAME "CatchingUpRecorder"
#define APP_VERSION 0.1


#ifdef GIT_VERSION

//#define APP_VERSION GIT_VERSION

#endif // GIT_VERSION


#define DEBUG_ENABLED

/** Toggle function name and line in file printing for debugging **/
//#define DEBUG_WITH_FUNCT_NAME

/** Definie if OpenCV is used to view image of the tracking algorithm (must link with openCV)**/
#define DEBUG_IMAGE

/** Set if SSL support is enabled */
//#define SSL_ENABLE 1






#ifdef DEBUG_ENABLED
#define LOGGER_START(MIN_PRIORITY, ...) Logger::Start(MIN_PRIORITY, __VA_ARGS__)
//#define LOGGER_START(MIN_PRIORITY, FILE, BOTH) Logger::Start(MIN_PRIORITY, FILE, BOTH)
#define LOGGER_STOP() Logger::Stop()
#define LOGGER_WRITE(PRIORITY, MESSAGE) Logger::Write(PRIORITY, MESSAGE)


#ifdef DEBUG_WITH_FUNCT_NAME
#define LOGGER_ERROR(MESSAGE)   Logger::Write(Logger::ERROR,    SSTR("Function : "<< setw(30) << left <<__FUNCTION__ << " @ " << setw(20) << left << __FILE__ << ":" << __LINE__  << "\t" << MESSAGE))
#define LOGGER_WARN(MESSAGE)    Logger::Write(Logger::WARNING,  SSTR("Function : "<< setw(30) << left <<__FUNCTION__ << " @ " << setw(20) << left << __FILE__ << ":" << __LINE__  << "\t" << MESSAGE))
#define LOGGER_INFO(MESSAGE)    Logger::Write(Logger::INFO,     SSTR("Function : "<< setw(30) << left <<__FUNCTION__ << " @ " << setw(20) << left << __FILE__ << ":" << __LINE__  << "\t" << MESSAGE))
#define LOGGER_CONFIG(MESSAGE)  Logger::Write(Logger::CONFIG,   SSTR("Function : "<< setw(30) << left <<__FUNCTION__ << " @ " << setw(20) << left << __FILE__ << ":" << __LINE__  << "\t" << MESSAGE))
#define LOGGER_DEBUG(MESSAGE)   Logger::Write(Logger::DEBUG,    SSTR("Function : "<< setw(30) << left <<__FUNCTION__ << " @ " << setw(20) << left << __FILE__ << ":" << __LINE__  << "\t" << MESSAGE))
#define LOGGER_VERB(MESSAGE)    Logger::Write(Logger::VERB,     SSTR("Function : "<< setw(30) << left <<__FUNCTION__ << " @ " << setw(20) << left << __FILE__ << ":" << __LINE__  << "\t" << MESSAGE))

#else

#define LOGGER_ERROR(MESSAGE) Logger::Write(Logger::ERROR, SSTR(MESSAGE))
#define LOGGER_WARN(MESSAGE) Logger::Write(Logger::WARNING, SSTR(MESSAGE))
#define LOGGER_INFO(MESSAGE) Logger::Write(Logger::INFO, SSTR(MESSAGE))
#define LOGGER_CONFIG(MESSAGE) Logger::Write(Logger::CONFIG, SSTR(MESSAGE))
#define LOGGER_DEBUG(MESSAGE) Logger::Write(Logger::DEBUG, SSTR(MESSAGE))
#define LOGGER_VERB(MESSAGE) Logger::Write(Logger::VERB, SSTR(MESSAGE))

#endif
#else

#define LOGGER_START(MIN_PRIORITY, ...)
//#define LOGGER_START(MIN_PRIORITY, FILE, BOTH)
#define LOGGER_STOP()

#define LOGGER_WRITE(PRIORITY, MESSAGE)
#define LOGGER_ERROR(MESSAGE)
#define LOGGER_WARN(MESSAGE)
#define LOGGER_INFO(MESSAGE)
#define LOGGER_CONFIG(MESSAGE)
#define LOGGER_DEBUG(MESSAGE)
#define LOGGER_VERB(MESSAGE)
#endif


// casting integer to string easyly
#define SSTR( x ) (dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str())





#include <sstream>

#include <iostream>

#include <iomanip>
#include "logger.h"



