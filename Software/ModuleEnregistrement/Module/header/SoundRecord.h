#ifndef __SOUND_RECORD_H_INCLUDED__
#define __SOUND_RECORD_H_INCLUDED__

#include <iostream>
#include <string>

#include "Recording.h"


using namespace std;

class SoundRecord
{

public:
	SoundRecord(string dev);
	bool startRecording(Recording * recording);
	bool stopRecording();
    bool isRecording();
    int getPid();
    static SoundRecord * getSoundRecord();
private :
    string _device;
	Recording * _CurrentRecording;
	static SoundRecord * _instance;
};
#endif
