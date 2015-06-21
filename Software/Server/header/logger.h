#ifndef LOGGER_H__INCLUDE
#define LOGGER_H__INCLUDE

#include <string>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <queue>
using namespace std;
class Logger
{
	public:
		// log priorities
		enum Priority
		{
			VERB,
			DEBUG,
			CONFIG,
			INFO,
			WARNING,
			ERROR
		};

		// start/stop logging
		// - messages with priority >= minPriority will be written in log
		// - set logFile = "" to write to standard output
		static void Start(Priority minPriority, const string& logFile,bool outputBoth);
		static void Start(Priority minPriority, const string& logFile);
		static void Stop();

		// write message
		static void Write(Priority priority, const string& message);

	private:
		// Logger adheres to the singleton design pattern, hence the private
		// constructor, copy constructor and assignment operator.
		Logger();
		Logger(const Logger& logger) {}
		static void * _threadFunc (void*);
		queue<string> fifo;
		// private instance data
		bool        active;
		ofstream    fileStream;
		Priority    minPriority;
			bool        outputBoth;

		pthread_t * _threadWrite;
		// names describing the items in enum Priority
		static const string PRIORITY_NAMES[];
		// the sole Logger instance (singleton)
		static Logger instance;
};


#endif // LOGGER_H__INCLUDE
