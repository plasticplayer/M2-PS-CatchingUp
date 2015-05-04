#include "Config.h"
#include <iostream>
#include <string>
#include "StillCamera.h"
#include "RaspiStill.h"
#include "Recording.h"


StillCamera * StillCamera::_instance;

StillCamera::StillCamera(RASPISTILL_STATE newState)
{
	if(StillCamera::_instance == NULL)
	{
		state = newState;
		camera_still_port = NULL;
		encoder_input_port = NULL;
		encoder_output_port= NULL;

		_recording = false;
		_frameID = 1;
		_sleepTimeMS = 1000;
		StillCamera::_instance = this;
	}
}
StillCamera * StillCamera::getCamera()
{
	return StillCamera::_instance;
}
void StillCamera::setSleepTime(unsigned int milliseconds)
{
    _sleepTimeMS = milliseconds;
}
bool StillCamera::init()
{
	status = MMAL_SUCCESS;
	_frameID = 1;

	if (!create_camera_component(&state))
	{
		//vcos_log_error("%s: Failed to create camera component", __func__);
		LOGGER_ERROR("Failed to create camera component");
	}
	else if (!create_encoder_component(&state))
	{
		//vcos_log_error("%s: Failed to create encode component", __func__);
		LOGGER_ERROR("Failed to create encode component");
		raspipreview_destroy(&state.preview_parameters);
		destroy_camera_component(&state);
	}
	else
	{
		LOGGER_DEBUG("Starting component connection stage");

		//camera_preview_port = state.camera_component->output[MMAL_CAMERA_PREVIEW_PORT];
		//camera_video_port   = state.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
		camera_still_port   = state.camera_component->output[MMAL_CAMERA_CAPTURE_PORT];
		//preview_input_port  = state.preview_parameters.preview_component->input[0];
		encoder_input_port  = state.encoder_component->input[0];
		encoder_output_port = state.encoder_component->output[0];




		LOGGER_DEBUG("Connecting camera stills port to encoder input port");
		// Now connect the camera to the encoder
		status = connect_ports(camera_still_port, encoder_input_port, &state.encoder_connection);

		if (status != MMAL_SUCCESS)
		{
			LOGGER_ERROR("Failed to connect camera video port to encoder input");
			destroy();
			return false;
		}

		// Set up our userdata - this is passed though to the callback where we need the information.
		// Null until we open our filename
		callback_data.file_handle = NULL;
		callback_data.pstate = &state;


		encoder_output_port->userdata = (struct MMAL_PORT_USERDATA_T *)&callback_data;


		LOGGER_DEBUG("Enabling encoder output port");
		// Enable the encoder output port and tell it its callback function
		status = mmal_port_enable(encoder_output_port, encoder_buffer_callback);

		if (status != MMAL_SUCCESS)
		{
			LOGGER_ERROR("Failed to setup encoder output");
			destroy();
			return false;
		}
		vcos_sleep(2000); // Wait for the Caméra to start
		return true;
	}
	return false;

}

bool StillCamera::saveSnapshot(string& baseFileName,bool counter)
{
	bool statusSnap = true;
	VCOS_STATUS_T vcos_status;
	vcos_status = vcos_semaphore_create(&callback_data.complete_semaphore, "RaspiStill-sem", 0);

	vcos_assert(vcos_status == VCOS_SUCCESS);
	string use_filename;
	if(counter)
		use_filename = SSTR(baseFileName << _frameID << ".jpeg");
	else
		use_filename = baseFileName ;

    baseFileName = use_filename;
	//if (state.timelapse)
	//asprintf(&use_filename, state.filename, frame);


	LOGGER_DEBUG("Opening output file " << use_filename);
	FILE * output_file = fopen(use_filename.c_str(), "wb");

	if (!output_file)
	{
		LOGGER_ERROR("Error opening output file: " << use_filename <<"No output file will be generated" << use_filename);
		statusSnap = false;
		// Notify user, carry on but discarding encoded output buffers
		//vcos_log_error("%s: Error opening output file: %s\n No output file will be generated\n", __func__, use_filename.c_str());
	}

	// asprintf used in timelapse mode allocates its own memory which we need to free
	//if (state.timelapse)
	//   free(use_filename);


	add_exif_tags(&state);
	callback_data.file_handle = output_file;


	// We only capture if a filename was specified and it opened
	if (output_file)
	{
		// Send all the buffers to the encoder output port
		int num = mmal_queue_length(state.encoder_pool->queue);
		int q;

		for (q=0; q<num; q++)
		{
			MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(state.encoder_pool->queue);

			if (!buffer)
			{
				LOGGER_ERROR("Unable to get a required buffer " << q << " from pool queue");
				statusSnap = false;
			}

			//vcos_log_error("Unable to get a required buffer %d from pool queue", q);

			if (mmal_port_send_buffer(encoder_output_port, buffer)!= MMAL_SUCCESS)
			{
				LOGGER_ERROR("Unable to send a buffer to encoder output port ("<<q<<")");
				statusSnap = false;
			}


		}
		if(counter)
			LOGGER_DEBUG("Starting capture" << _frameID);
		else
			LOGGER_DEBUG("Starting capture");
		if (mmal_port_parameter_set_boolean(camera_still_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
		{
			LOGGER_ERROR("Failed to start capture");
			statusSnap = false;
			//vcos_log_error("%s: Failed to start capture", __func__);
		}
		else
		{
			// Wait for capture to complete
			// For some reason using vcos_semaphore_wait_timeout sometimes returns immediately with bad parameter error
			// even though it appears to be all correct, so reverting to untimed one until figure out why its erratic

			vcos_semaphore_wait(&callback_data.complete_semaphore);
			if(counter)
				LOGGER_DEBUG("Finished capture " << _frameID);
			else
				LOGGER_DEBUG("Finished capture");
		}

		// Ensure we don't die if get callback with no open file
		callback_data.file_handle = NULL;

		if (output_file != stdout)
			fclose(output_file);
	}
	vcos_semaphore_delete(&callback_data.complete_semaphore);
	if(counter)
		_frameID++;
	return statusSnap;
}

bool StillCamera::takeSnapshot(uint8_t ** bufferOut,int * bufferSize)
{
	// Use a temporary file
	string fName = "tmp.jpeg.data";
	bool statusSnap = saveSnapshot(fName,false);
	if(statusSnap)
	{
		ifstream in(fName.c_str(), std::ifstream::ate | std::ifstream::binary);
		*bufferSize = in.tellg();
		in.close();

		*bufferOut = (uint8_t *)malloc(sizeof(uint8_t) * (*bufferSize));

		ifstream myfile(fName.c_str());

		if (myfile.is_open())
		{
			char c;
			int i = 0;
			while (myfile.get(c))
			{
				(*bufferOut)[i++] = (uint8_t)c;
			}
			if(i != *bufferSize)
				LOGGER_ERROR("Size Read not equals to size of file : "<< (*bufferSize) << " =!= " << i);
			myfile.close();
		}
		else
		{
			LOGGER_ERROR("Can't reopen temporary file for reading "<< fName );
			statusSnap = false;
		}

		if( remove( fName.c_str()) != 0 )
			LOGGER_ERROR("Can't delete temporary file "<< fName );
	}
	return statusSnap;
}
/*bool StillCamera::takeSnapshot(uint8_t ** bufferOut,int * bufferSize)
{
	bool statusSnap = true;
	VCOS_STATUS_T vcos_status;
	vcos_status = vcos_semaphore_create(&callback_data.complete_semaphore, "RaspiStill-sem", 0);

	vcos_assert(vcos_status == VCOS_SUCCESS);

	callback_data.file_handle = NULL;
	callback_data.bMemOutput = true;
	callback_data.pBuffer = bufferOut;
	callback_data.pBufferSize = bufferSize;


	// We only capture if a filename was specified and it opened
	//if (output_file)
	{
		// Send all the buffers to the encoder output port
		int num = mmal_queue_length(state.encoder_pool->queue);
		int q;

		for (q=0; q<num; q++)
		{
			MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(state.encoder_pool->queue);

			if (!buffer)
			{
				LOGGER_ERROR("Unable to get a required buffer " << q << " from pool queue");
				statusSnap = false;
			}

			//vcos_log_error("Unable to get a required buffer %d from pool queue", q);

			if (mmal_port_send_buffer(encoder_output_port, buffer)!= MMAL_SUCCESS)
			{
				LOGGER_ERROR("Unable to send a buffer to encoder output port ("<<q<<")");
				statusSnap = false;
			}


		}
		LOGGER_DEBUG("Starting capture On RAM ADDR : " << (int)(bufferOut) );

		if (mmal_port_parameter_set_boolean(camera_still_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
		{
			LOGGER_ERROR("Failed to start capture");
			statusSnap = false;
			//vcos_log_error("%s: Failed to start capture", __func__);
		}
		else
		{
		    LOGGER_DEBUG("Capture process");
			// Wait for capture to complete
			// For some reason using vcos_semaphore_wait_timeout sometimes returns immediately with bad parameter error
			// even though it appears to be all correct, so reverting to untimed one until figure out why its erratic

			vcos_semaphore_wait(&callback_data.complete_semaphore);
			LOGGER_DEBUG("Finished capture " );
		}
        LOGGER_DEBUG("Sizeof pBuffer : " << *bufferSize);

		// Ensure we don't die if get callback with no open file
		callback_data.file_handle = NULL;
		callback_data.bMemOutput = false;
        callback_data.pBuffer = NULL;
	}
	vcos_semaphore_delete(&callback_data.complete_semaphore);
	return statusSnap;
}*/

void StillCamera::destroy()
{
	LOGGER_DEBUG("Destroying camera component");

	// Disable all our ports that are not handled by connections
	//check_disable_port(camera_video_port);
	if(encoder_output_port != NULL)
		check_disable_port(encoder_output_port);
	else
		return;

	//if (state.preview_parameters.wantPreview )
	//	mmal_connection_destroy(state.preview_connection);

	mmal_connection_destroy(state.encoder_connection);
	/* Disable components */
	if (state.encoder_component)
		mmal_component_disable(state.encoder_component);

	//if (state.preview_parameters.preview_component)
	//	mmal_component_disable(state.preview_parameters.preview_component);

	if (state.camera_component)
		mmal_component_disable(state.camera_component);

	destroy_encoder_component(&state);

	//raspipreview_destroy(&state.preview_parameters);
	destroy_camera_component(&state);

	LOGGER_DEBUG("Still Camera Close down completed, all components disconnected, disabled and destroyed");

	mmal_status_to_int(status);

	if (status != 0)
		raspicamcontrol_check_configuration(128);

	StillCamera::_instance = NULL;

}

bool StillCamera::isInUse()
{
	return StillCamera::_instance != NULL;
}


bool StillCamera::startRecording(Recording * recording)
{
	_CurrentRecording = recording;
	if(_CurrentRecording == NULL)
		return false;
	_recording = true;
	_folderRecording = _CurrentRecording->_folderRecording;
	_RecordingNumber = 1;
	LOGGER_VERB("Creating Image Recording Thread ! Folder : " << this->_folderRecording );
	int ret = pthread_create(&_ThreadRecording , NULL, &(this->_threadRecord), (void *) this) ;
	return ret == 0;
}
bool StillCamera::isRecording()
{
    return _recording;
}
bool StillCamera::stopRecording()
{
	if(_recording)
	{
		_recording = false;
		pthread_join(_ThreadRecording,NULL);
		destroy();
		return true;
	}
	destroy();
	return false;

}

void * StillCamera::_threadRecord( void * arg)
{
	StillCamera * thisObj = (StillCamera *) arg;
	LOGGER_DEBUG("Starting Image Recording Thread !");
	while(thisObj->_recording)
	{
        string filePath = SSTR(thisObj->_CurrentRecording->_folderRecording << "/snap" );
        thisObj->saveSnapshot(filePath,true);
        RecordingFile * file = new RecordingFile();
        file->isInRecord = false;
        unsigned int foundPos = filePath.find_last_of("/");
        file->fileName = filePath.substr(foundPos+1);
        file->path = filePath;
        //LOGGER_VERB("File path : " << filePath << " file name : " << file->fileName);
        thisObj->_CurrentRecording->addFile(file);
		usleep(thisObj->_sleepTimeMS * 1000);
	}
	return NULL;
}


