
#include "Config.h"
#include "SoundRecord.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

//arecord --quiet -M -D plughw:1 -f S16_LE -c1 -r22050 -t raw | lame --quiet -r -s 22.05 -m m -b 64 - Output.
SoundRecord * SoundRecord::_instance;

SoundRecord::SoundRecord(string dev)
{
    if(_instance == NULL)
        _instance = this;

    _device = dev;
}
bool SoundRecord::startRecording(Recording * recording)
{
    _CurrentRecording = recording;
	string command = SSTR("arecord --quiet -M -D " << _device <<" -f S16_LE -c1 -r22050 -t raw |" << " lame --quiet -r -s 22.05 -m m -b 64 - '" << _CurrentRecording->_folderRecording << "/sound.mp3' &");
    LOGGER_VERB("Starting sound record : cmdline : "<< command);
	system(command.c_str());
	return isRecording();
}
SoundRecord * SoundRecord::getSoundRecord()
{
    return SoundRecord::_instance;
}
bool SoundRecord::stopRecording()
{
    if(isRecording())
    {
        int pid = getPid();
        LOGGER_VERB("Sending SIGINT to process  " << pid);
        if(0==kill(pid,SIGINT))
        {
            // Ok send
            while(isRecording())
            {
                usleep(100);
            }
            LOGGER_VERB("Recording stopped ");
            return true;
        }
        else
        {
            LOGGER_ERROR("Recording cannot be stopped : "<<errno << ", ("<<strerror(errno)<<")");
            return false;
        }
    }
    return true;
}
bool SoundRecord::isRecording()
{
    string command = SSTR("pidof -x arecord -s");
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
        return false;
    string ligne;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    while((read = getline(&line, &len, pipe)) != -1)
    {
        // If it got a line => recording
        return true;
    }

    return false;
}
int SoundRecord::getPid()
{
    string command = SSTR("pidof -x arecord -s");
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
        return 0;
    string ligne;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    while((read = getline(&line, &len, pipe)) != -1)
    {
        // If it got a line => recording
        return atoi(line);
    }

    return 0;
}

