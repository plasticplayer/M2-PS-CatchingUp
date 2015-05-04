#ifndef __STILL_CAMERA_H_INCLUDED__
#define __STILL_CAMERA_H_INCLUDED__
#include <iostream>
#include <string>
#include "RaspiStill.h"

using namespace std;

class StillCamera{
public:
    StillCamera(RASPISTILL_STATE newState);
    bool init();
    void destroy();
    bool saveSnapshot(string baseFileName,bool counter = true);
    bool takeSnapshot(uint8_t ** bufferOut,int * bufferSize);
    static bool isInUse();
    static StillCamera * getCamera();
private:
    RASPISTILL_STATE state;
    MMAL_PORT_T *camera_still_port;
	MMAL_PORT_T *encoder_input_port;
	MMAL_PORT_T *encoder_output_port;
    PORT_USERDATA callback_data;
    MMAL_STATUS_T status;
    uint64_t _frameID;
    static StillCamera *_instance;
};


#endif // __STILL_CAMERA_H_INCLUDED__
