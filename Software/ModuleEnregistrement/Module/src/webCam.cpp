#include "Config.h"
#include "define.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/time.h>
#include <stdbool.h>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "libcam.h"
#include "bcm_host.h"

#include "encode.h"
#include "webCam.h"

using namespace std;

Webcam * Webcam::_instance;

Webcam::Webcam(string dev,unsigned int imageWidth, unsigned int imageHeight,uint8_t frameRate)
{
	if(Webcam::_instance == NULL)
	{


		_imageHeight = imageHeight;
		_imageWidth = imageWidth;
		_frameRate = frameRate;

		LOGGER_VERB("Creating webcam : "<< dev << " size asked [" << _imageWidth << " x " <<_imageHeight <<"] @ " << (int)_frameRate << " FPS" );
		_Camera = new Camera(dev.c_str(), _imageWidth, _imageHeight, _frameRate);
		_recording = false;

		_splitTimeSeconds = 0; // No split
		Webcam::_instance = this;
	}
}

bool Webcam::isInUse()
{
	return Webcam::_instance != NULL;
}
Webcam * Webcam::getWebcam()
{
	return Webcam::_instance;
}

void Webcam::setSplitTime(unsigned int seconds)
{
	_splitTimeSeconds = seconds;
}
bool Webcam::isRecording()
{
	return _recording;
}
bool Webcam::testWebcam()
{
	bool st = true;
	if(!_Camera->Open() || !_Camera->Init() || !_Camera->Start())
		return false;

	char tmpData[_imageHeight * _imageWidth * 3];
	int taille;
	grabImage(tmpData,&taille);
	if(taille == 0)
		st = false;

	_Camera->StopCam();
	return st;
}

void Webcam::close()
{
	LOGGER_DEBUG("Closing webcam");
	_Camera->StopCam();
	deinitH264();
}

void Webcam::generate_test_card(char *buf, int32_t * filledLen, int frame)
{
	unsigned int i, j;
	char *b = buf, *g = b+1, *r = g+1;
	for (j = 0; j <_imageHeight ; j++)
	{
		b = buf + j*3 * _imageWidth;
		g = b+1;
		r = g+1;
		for(i = 0; i< _imageWidth  ; i++)
		{
			*b = j % 255;
			*g = i % 255;
			*r = 0 % 255;
			b += 3;
			g = b+1;
			r = g+1;
		}
	}
	*filledLen = _imageHeight * _imageWidth * 3;
}

void Webcam::grabImage( char *buf, int32_t * filledLen)
{
	char * dataPtr= NULL;
	while((dataPtr = ((char*)_Camera->Get()))==0)
	{
		usleep(10);   	// get the image
	}
	char *Y ,*Cb, *Cr, *Y2 ,*r,*g,*b;

	unsigned int pix = 0;
	unsigned int pixY = 0;
	unsigned int w2 = _imageWidth/2;

	for(unsigned int x=0; x<w2; x++)
	{
		for(unsigned int posy=0; posy<_imageHeight; posy++)
		{
			Y = dataPtr + (pixY *4);
			//Y = dataPtr + (pix *4);
			Cb = Y +1;
			Y2 = Cb +1;
			Cr = Y2 +1;

			r = buf + (pix * 3);
			g = r+1;
			b = g+1;
			// TODO: Optimisation (without float) (YCbYCr to RGB)
			int rt = (int) (*Y + 1.40200 * (*Cr - 0x80));
			int gt = (int) (*Y - 0.34414 * (*Cb - 0x80) - 0.71414 * (*Cr - 0x80));
			int bt = (int) (*Y + 1.77200 * (*Cb - 0x80));
			int rt2 = (int) (*Y2 + 1.40200 * (*Cr - 0x80));
			int gt2 = (int) (*Y2 - 0.34414 * (*Cb - 0x80) - 0.71414 * (*Cr - 0x80));
			int bt2 = (int) (*Y2 + 1.77200 * (*Cb - 0x80));
			*r = max(0, min(255, rt));
			*g = max(0, min(255, gt));
			*b = max(0, min(255, bt));

			pix ++;

			r = buf + (pix * 3);
			g = r+1;
			b = g+1;
			*r = max(0, min(255, rt2));
			*g = max(0, min(255, gt2));
			*b = max(0, min(255, bt2));
			pixY++;
			pix++;
		}
	}
	*filledLen = _imageHeight * _imageWidth * 3;
}

bool Webcam::startRecording(string folder)
{
	_recording = true;
	_folderRecording = folder;
	_RecordingNumer = 1;
	LOGGER_VERB("Creating Video Recording Thread ! Folder : " << this->_folderRecording << " R : " << this->_RecordingNumer );
	int ret = pthread_create(&_ThreadRecording , NULL, &(this->_threadRecord), (void *) this) ;
	return ret == 0;
}
bool Webcam::stopRecording()
{
	if(_recording)
	{
		_recording = false;
		pthread_join(_ThreadRecording,NULL);
		close();
		return true;
	}
close();
	return false;

}

void * Webcam::_threadRecord( void * arg)
{
	Webcam * thisObj = (Webcam *) arg;

	LOGGER_DEBUG("Starting Video Recording Thread !");
	FILE * fichierH264;
	string fileName = SSTR(thisObj->_folderRecording << "/" << thisObj->_RecordingNumer << ".h264");
	fichierH264 = fopen (fileName.c_str()  ,"wb+");
	if(fichierH264 == NULL)
	{
		LOGGER_ERROR("Cannot open file for writting : " << fileName);
		thisObj->_recording = false;
		return NULL;
	}

	if(!initH264(thisObj->_imageHeight,thisObj->_imageWidth,thisObj->_frameRate))
	{
		LOGGER_ERROR("Cannot init hardware H.264 encoder !");
		thisObj->_recording = false;
		if(fichierH264 != NULL)
			fclose (fichierH264);
		return NULL;
	}

	if(!thisObj->_Camera->Open() || !thisObj->_Camera->Init() || !thisObj->_Camera->Start())
	{
		LOGGER_ERROR("Cannot start webcam !");
		thisObj->_recording = false;
		deinitH264();
		if(fichierH264 != NULL)
			fclose (fichierH264);
		return NULL;

	}

	LOGGER_DEBUG("Recording Webcam init done, starting recording");
	char tmpData[thisObj->_imageHeight * thisObj->_imageWidth * 3];
	int taille;

	struct timeval now;
	double timeLastSplit, timeAct,timePast,timeDiff;
	gettimeofday(&now,NULL);
	timePast = now.tv_sec + now.tv_usec*1e-6;
	timeLastSplit = timePast;
	unsigned int frameCounter = 0;
	while(thisObj->_recording)
	{
		thisObj->grabImage(tmpData,&taille);
		writeImageH264(tmpData, taille,fichierH264);

		gettimeofday(&now,NULL);
		timeAct = now.tv_sec + now.tv_usec*1e-6;
		
		if(thisObj->_splitTimeSeconds > 0)
		{
			timeDiff = timeAct - timeLastSplit;
			if(timeDiff >= thisObj->_splitTimeSeconds)
			{
				timeLastSplit = timeAct;
				LOGGER_VERB("We need to split the recording now");

				if(fichierH264 != NULL)
					fclose (fichierH264);
				string fileName = SSTR(thisObj->_folderRecording << "/" << ++thisObj->_RecordingNumer << ".h264");
				fichierH264 = fopen (fileName.c_str()  ,"wb+");
				if(fichierH264 == NULL)
				{
					LOGGER_ERROR("Cannot open file for writting : " << fileName);
					thisObj->_recording = false;
					return NULL;
				}
			}
		}	
		frameCounter ++;

		//if (frameCounter%10) // On ne verifie que les FPS que toutes les 10 images
		{
			timeDiff = timeAct -timePast;
			if ( timeDiff > 1) { // Calcul des FPS une fois par seconde
				//lastFPS = (float)frameCounter/timeDiff;
				LOGGER_VERB("FPS :" << (float)frameCounter/timeDiff);
				timePast = timeAct;
				frameCounter = 0;
			}
		}

	}

	if(fichierH264 != NULL)
		fclose (fichierH264);
	return NULL;
}

/*int main(int argc, char **argv)
  {
  if (argc < 2)
  {
  printf("Usage: %s <filename>\n", argv[0]);
  exit(1);
  }
  bcm_host_init();

  Camera A("/dev/video0", WIDTH, HEIGHT,25);


  initH264(argv[1],HEIGHT,WIDTH);

  int a;
  char tmpData[HEIGHT * WIDTH * 3];
  int tmp;


  int frameCounter = 0;
  struct timeval now;
  double timeAct,timePast,timeDiff,timeStart;
  gettimeofday(&now,NULL);
  timePast = now.tv_sec + now.tv_usec*1e-6;
  float lastFPS = 0;
  for(a = 0; a < 300 ; a++)
  {

  frameCounter ++;

//if (frameCounter%10) // On ne verifie que les FPS que toutes les 10 images
{
gettimeofday(&now,NULL);
timeAct = now.tv_sec + now.tv_usec*1e-6;
timeStart = timeAct;
timeDiff = timeAct -timePast;
if ( timeDiff > 1) { // Calcul des FPS une fois par seconde
lastFPS = (float)frameCounter/timeDiff;
printf("FPS : %.2f \n",lastFPS);
timePast = timeAct;
frameCounter = 0;
}
}
//		generate_test_card(tmpData,&tmp,a);
grabImage( A,tmpData,&tmp);
writeImage(tmpData, tmp);
}
return 0;
}*/

