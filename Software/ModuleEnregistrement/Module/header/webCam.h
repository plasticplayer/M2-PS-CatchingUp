#ifndef __WebCam__h__
#define __WebCam__h__

#include <pthread.h>
#include "libcam.h"
#include "Recording.h"

class Webcam
{
	public :
		Webcam(string dev,unsigned int imageWidth, unsigned int imageHeight,uint8_t frameRate);
		void generate_test_card(char *buf, int32_t * filledLen, int frame);
		void grabImage( char *buf, int32_t * filledLen);
		bool testWebcam();

		void setSplitTime(unsigned int seconds); // Set the split time for each file
		bool startRecording(Recording * recording);
		bool stopRecording();
		bool isRecording();
		void close();
		static bool isInUse();
		static Webcam * getWebcam();
	protected :

	private :
		static void * _threadRecord( void * arg);
		string _folderRecording;
		int _RecordingNumber;
		bool _recording;
		unsigned int _imageHeight;
		unsigned int _imageWidth;
		uint8_t _frameRate;
		Camera  * _Camera;
		unsigned int _splitTimeSeconds;
		pthread_t _ThreadRecording;
		static Webcam *_instance;
		Recording * _CurrentRecording;
};

#endif
