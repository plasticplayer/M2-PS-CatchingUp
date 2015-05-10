#ifndef __STILL_CAMERA_H_INCLUDED__
#define __STILL_CAMERA_H_INCLUDED__

#include <iostream>
#include <pthread.h>
#include <string>

#include "RaspiStill.h"
#include "Recording.h"


using namespace std;



class StillCamera{

	public:
		StillCamera(RASPISTILL_STATE newState);

		bool init();
		void destroy();
		bool saveSnapshot(string& baseFileName,bool counter = true);
		bool takeSnapshot(uint8_t ** bufferOut,int * bufferSize);
		void setSleepTime(unsigned int seconds); // Set the split time for each file
		bool startRecording(Recording * recording);
		bool stopRecording();
		bool isRecording();

		static bool isInUse();
		static StillCamera * getCamera();
	private:

        static StillCamera *_instance;
		static void * _threadRecord( void * arg);
		string _folderRecording;
		int _RecordingNumber;
		bool _recording;
		unsigned int _sleepTimeMS;
		pthread_t _ThreadRecording;
		Recording * _CurrentRecording;

		RASPISTILL_STATE state;

		MMAL_PORT_T *camera_still_port;
		MMAL_PORT_T *encoder_input_port;
		MMAL_PORT_T *encoder_output_port;

		PORT_USERDATA callback_data;

		MMAL_STATUS_T status;

		uint64_t _frameID;



};





#endif // __STILL_CAMERA_H_INCLUDED__

