
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "RaspiStill.h"
#include <sstream>
#include <pthread.h>
#include <sstream>
#include <iostream>



#define APP_NAME "CatchingUpRecorder"
#define APP_VERSION 0.1

#ifdef GIT_VERSION
#define APP_VERSION GIT_VERSION
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

#include "logger.h"

// casting integer to string easyly
#define SSTR( x ) (dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str())

using namespace std;
/**
 * main
 */
bool bRecording = true;
RASPISTILL_STATE state;

void *recording_thread (void * arg)
{
	MMAL_STATUS_T status = MMAL_SUCCESS;
	MMAL_PORT_T *camera_preview_port = NULL;
	MMAL_PORT_T *camera_video_port = NULL;
	MMAL_PORT_T *camera_still_port = NULL;
	MMAL_PORT_T *preview_input_port = NULL;
	MMAL_PORT_T *encoder_input_port = NULL;
	MMAL_PORT_T *encoder_output_port = NULL;

	uint64_t frame = 1;
	FILE *output_file = NULL;


	if (!create_camera_component(&state))
	{
		//vcos_log_error("%s: Failed to create camera component", __func__);
		LOGGER_ERROR("Failed to create camera component \n");
	}
	else if (!create_encoder_component(&state))
	{
		//vcos_log_error("%s: Failed to create encode component", __func__);
		LOGGER_ERROR("Failed to create encode component \n");
		raspipreview_destroy(&state.preview_parameters);
		destroy_camera_component(&state);
	}
	else
	{
		PORT_USERDATA callback_data;

        LOGGER_DEBUG("Starting component connection stage \n");

		//camera_preview_port = state.camera_component->output[MMAL_CAMERA_PREVIEW_PORT];
		//camera_video_port   = state.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
		camera_still_port   = state.camera_component->output[MMAL_CAMERA_CAPTURE_PORT];
		//preview_input_port  = state.preview_parameters.preview_component->input[0];
		encoder_input_port  = state.encoder_component->input[0];
		encoder_output_port = state.encoder_component->output[0];

		VCOS_STATUS_T vcos_status;


        LOGGER_DEBUG("Connecting camera stills port to encoder input port");
		// Now connect the camera to the encoder
		status = connect_ports(camera_still_port, encoder_input_port, &state.encoder_connection);

		if (status != MMAL_SUCCESS)
		{
			 LOGGER_ERROR("Failed to connect camera video port to encoder input");
			goto error;
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

			goto error;
		}

		/*if (state.demoMode)
		{
		   // Run for the user specific time..
		   int num_iterations = state.timeout / state.demoInterval;
		   int i;
		   for (i=0;i<num_iterations;i++)
		   {
		      raspicamcontrol_cycle_test(state.camera_component);
		      vcos_sleep(state.demoInterval);
		   }
		}
		else*/

		vcos_sleep(2000); // Wait for the Caméra to start
		//int num_iterations =  1;


		while (bRecording)
		{
			vcos_status = vcos_semaphore_create(&callback_data.complete_semaphore, "RaspiStill-sem", 0);

			vcos_assert(vcos_status == VCOS_SUCCESS);
			/*if (state.timelapse)

			else
			   vcos_sleep(state.timeout);
			 */
			// Open the file

			string use_filename = SSTR(state.filename << frame << ".jpeg");

			//if (state.timelapse)
			//asprintf(&use_filename, state.filename, frame);


            LOGGER_DEBUG("Opening output file " << use_filename);
			output_file = fopen(use_filename.c_str(), "wb");

			if (!output_file)
			{
			    LOGGER_ERROR("Error opening output file: " << use_filename <<"No output file will be generated" << use_filename);
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
                        LOGGER_ERROR("Unable to get a required buffer " << q << " from pool queue");
						//vcos_log_error("Unable to get a required buffer %d from pool queue", q);

					if (mmal_port_send_buffer(encoder_output_port, buffer)!= MMAL_SUCCESS)
						LOGGER_ERROR("Unable to send a buffer to encoder output port ("<<q<<")");
				}
                LOGGER_DEBUG("Starting capture" << frame);

				if (mmal_port_parameter_set_boolean(camera_still_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
				{
				    LOGGER_ERROR("Failed to start capture");
					//vcos_log_error("%s: Failed to start capture", __func__);
				}
				else
				{
					// Wait for capture to complete
					// For some reason using vcos_semaphore_wait_timeout sometimes returns immediately with bad parameter error
					// even though it appears to be all correct, so reverting to untimed one until figure out why its erratic

					vcos_semaphore_wait(&callback_data.complete_semaphore);
					LOGGER_DEBUG("Finished capture " << frame);
				}

				// Ensure we don't die if get callback with no open file
				callback_data.file_handle = NULL;

				if (output_file != stdout)
					fclose(output_file);
			}
			vcos_semaphore_delete(&callback_data.complete_semaphore);
			vcos_sleep(state.timeout);
			frame++;

		}
error:
		mmal_status_to_int(status);


			LOGGER_DEBUG("Closing down");

		// Disable all our ports that are not handled by connections
		//check_disable_port(camera_video_port);
		check_disable_port(encoder_output_port);

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

			LOGGER_DEBUG("Close down completed, all components disconnected, disabled and destroyed");
	}
	if (status != 0)
		raspicamcontrol_check_configuration(128);

	pthread_exit (0);
	//return 0;
}

int main(int argc, const char **argv)
{
    LOGGER_START(Logger::DEBUG,"");

    bcm_host_init();
	// Register our application with the logging system
	vcos_log_register(APP_NAME,VCOS_LOG_CATEGORY);

	default_status(&state);
	state.timeout = 10000;                  /// Time between snapshoots, in ms
	state.filename = (char*) "test_";
	state.verbose = 1;
	state.width = 400;                          /// Requested width of image
	state.height = 300;                         /// requested height of image
	state.preview_parameters.wantPreview=0;

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
