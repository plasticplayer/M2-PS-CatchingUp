#define APP_NAME "CatchingUpRecorder"
#define APP_VERSION 0.1

#ifdef GIT_VERSION
//#define APP_VERSION GIT_VERSION
#endif // GIT_VERSION

#define DEBUG_LEVEL


#ifdef DEBUG_LEVEL

#define LOGGER_START(MIN_PRIORITY, FILE) Logger::Start(MIN_PRIORITY, FILE)
#define LOGGER_STOP() Logger::Stop()
#define LOGGER_WRITE(PRIORITY, MESSAGE) Logger::Write(PRIORITY, MESSAGE)

#define LOGGER_ERROR(MESSAGE) Logger::Write(Logger::ERROR, SSTR(MESSAGE))
#define LOGGER_WARN(MESSAGE) Logger::Write(Logger::WARNING, SSTR(MESSAGE))
#define LOGGER_INFO(MESSAGE) Logger::Write(Logger::INFO, SSTR(MESSAGE))
#define LOGGER_CONFIG(MESSAGE) Logger::Write(Logger::CONFIG, SSTR(MESSAGE))
#define LOGGER_DEBUG(MESSAGE) Logger::Write(Logger::DEBUG, SSTR(MESSAGE))

#else

#define LOGGER_START(MIN_PRIORITY, FILE)
#define LOGGER_STOP()

#define LOGGER_WRITE(PRIORITY, MESSAGE)
#define LOGGER_ERROR(MESSAGE)
#define LOGGER_WARN(MESSAGE)
#define LOGGER_INFO(MESSAGE)
#define LOGGER_CONFIG(MESSAGE)
#define LOGGER_DEBUG(MESSAGE)

#endif


// casting integer to string easyly
#define SSTR( x ) (dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str())


#include <sstream>
#include <iostream>
#include "logger.h"

