#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <pthread.h>

#include "Config.h"
#include "StillCamera.h"





using namespace std;
/**
 * main
 */
bool bRecording = true;
void takeSnapshoot()
{
    if(StillCamera::isInUse())
    {
        LOGGER_WARN("Can't create the camera it's already in use !");
        return;
    }
	RASPISTILL_STATE state;
	default_status(&state);
	state.timeout = 0;                  /// Time between snapshoots, in ms
	//state.filename = (char*) "test_";
	state.verbose = 0;
	//state.width = 400;                          /// Requested width of image
	//state.height = 300;                         /// requested height of image
	state.preview_parameters.wantPreview=0;

	StillCamera cam(state);
	uint8_t * buff = NULL;
	int sizeBuff = 0;
	if(cam.init())
	{
		cam.takeSnapshot(&buff,&sizeBuff);
		usleep(1000);

		ofstream myfile("ahhah.jpeg");
		for(int i = 0 ; i < sizeBuff; i++)
			myfile << (char)(buff[i]);
		myfile.close();
	}
	cam.destroy();
}
void *recording_thread (void * arg)
{
	RASPISTILL_STATE state;
	default_status(&state);
	state.timeout = 10000;                  /// Time between snapshoots, in ms
	state.filename = (char*) "test_";
	state.verbose = 0;
	state.width = 400;                          /// Requested width of image
	state.height = 300;                         /// requested height of image
	state.preview_parameters.wantPreview=0;

	StillCamera cam(state);
	if(cam.init())
	{
		while (bRecording)
		{
			if(!cam.saveSnapshot("test"))
				break;
			usleep(state.timeout*1000);
		}
	}
	cam.destroy();
	pthread_exit (0);
	//return 0;
}

int main(int argc, const char **argv)
{
	LOGGER_START(Logger::DEBUG,"");

	bcm_host_init();
	// Register our application with the logging system
	vcos_log_register(APP_NAME,VCOS_LOG_CATEGORY);


	signal(SIGINT, signal_handler);

	pthread_t * threadRecording ;
	void *ret;

	LOGGER_INFO("Program name : " << APP_NAME << " version : " << APP_VERSION);
	LOGGER_INFO("Program started waiting command");

	char ch;
	while((ch = getchar()) != 0)
	{
		if(ch == 's')
		{
			if(threadRecording == NULL)
			{
				LOGGER_DEBUG("Starting capture thread");

				bRecording = true;
				threadRecording = new pthread_t;
				if (pthread_create (threadRecording, NULL, recording_thread, NULL) < 0)
				{
					LOGGER_ERROR("pthread_create error for thread ");
					break;
				}
			}
		}
		if(ch == 'p')
		{
			LOGGER_DEBUG("Stopping capture thread");

			bRecording = false;
			if(threadRecording != NULL)
			{
				pthread_join (*threadRecording, NULL);
				delete threadRecording;
			}
			threadRecording = NULL;
		}
		if(ch == 'q')
		{
			LOGGER_DEBUG("Quit");
			bRecording = false;
			break;
		}
		if(ch == 'i')
		{
			takeSnapshoot();

		}
		usleep(100);
	}
	LOGGER_DEBUG("Exiting, killing threads");
	if(threadRecording != NULL)
	{
		LOGGER_DEBUG("Waiting for the thread to end ");
		pthread_join (*threadRecording, NULL);
		delete threadRecording;
	}
	else
		LOGGER_DEBUG("Thread already dead skipping");

	//pthread_exit(NULL);
	return EXIT_SUCCESS;
}
