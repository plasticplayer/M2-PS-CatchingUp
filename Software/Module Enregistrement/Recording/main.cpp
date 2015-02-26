
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "RaspiStill.h"
#include <sstream>
#include <pthread.h>
#include <sstream>
#include <iostream>

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

	bcm_host_init();
	// Register our application with the logging system
	vcos_log_register("RaspiStill", VCOS_LOG_CATEGORY);



	default_status(&state);
	state.timeout = 60*1000;                  /// Time between snapshoots, in ms
	state.filename = "test_";
	state.verbose = 1;
	state.width = 400;                          /// Requested width of image
	state.height = 300;                         /// requested height of image
	state.preview_parameters.wantPreview=0;

	if (!create_camera_component(&state))
	{
		vcos_log_error("%s: Failed to create camera component", __func__);
	}
	/*else if (!raspipreview_create(&state.preview_parameters))
	{
	   vcos_log_error("%s: Failed to create preview component", __func__);
	   destroy_camera_component(&state);
	}*/
	else if (!create_encoder_component(&state))
	{
		vcos_log_error("%s: Failed to create encode component", __func__);
		raspipreview_destroy(&state.preview_parameters);
		destroy_camera_component(&state);
	}
	else
	{
		PORT_USERDATA callback_data;

		if (state.verbose)
			fprintf(stderr, "Starting component connection stage\n");

		//camera_preview_port = state.camera_component->output[MMAL_CAMERA_PREVIEW_PORT];
		//camera_video_port   = state.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
		camera_still_port   = state.camera_component->output[MMAL_CAMERA_CAPTURE_PORT];
		//preview_input_port  = state.preview_parameters.preview_component->input[0];
		encoder_input_port  = state.encoder_component->input[0];
		encoder_output_port = state.encoder_component->output[0];

		VCOS_STATUS_T vcos_status;

		if (state.verbose)
			fprintf(stderr, "Connecting camera stills port to encoder input port\n");

		// Now connect the camera to the encoder
		status = connect_ports(camera_still_port, encoder_input_port, &state.encoder_connection);

		if (status != MMAL_SUCCESS)
		{
			vcos_log_error("%s: Failed to connect camera video port to encoder input", __func__);
			goto error;
		}

		// Set up our userdata - this is passed though to the callback where we need the information.
		// Null until we open our filename
		callback_data.file_handle = NULL;
		callback_data.pstate = &state;


		encoder_output_port->userdata = (struct MMAL_PORT_USERDATA_T *)&callback_data;

		if (state.verbose)
			fprintf(stderr, "Enabling encoder output port\n");

		// Enable the encoder output port and tell it its callback function
		status = mmal_port_enable(encoder_output_port, encoder_buffer_callback);

		if (status != MMAL_SUCCESS)
		{
			vcos_log_error("Failed to setup encoder output");
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

			if (state.verbose)
				fprintf(stderr, "Opening output file %s\n", use_filename.c_str());

			output_file = fopen(use_filename.c_str(), "wb");

			if (!output_file)
			{
				// Notify user, carry on but discarding encoded output buffers
				vcos_log_error("%s: Error opening output file: %s\nNo output file will be generated\n", __func__, use_filename.c_str());
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
						vcos_log_error("Unable to get a required buffer %d from pool queue", q);

					if (mmal_port_send_buffer(encoder_output_port, buffer)!= MMAL_SUCCESS)
						vcos_log_error("Unable to send a buffer to encoder output port (%d)", q);
				}

				if (state.verbose)
					fprintf(stderr, "Starting capture %d\n", frame);

				if (mmal_port_parameter_set_boolean(camera_still_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
				{
					vcos_log_error("%s: Failed to start capture", __func__);
				}
				else
				{
					// Wait for capture to complete
					// For some reason using vcos_semaphore_wait_timeout sometimes returns immediately with bad parameter error
					// even though it appears to be all correct, so reverting to untimed one until figure out why its erratic

					vcos_semaphore_wait(&callback_data.complete_semaphore);
					if (state.verbose)
						fprintf(stderr, "Finished capture %d\n", frame);
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

		if (state.verbose)
			fprintf(stderr, "Closing down\n");

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

		if (state.verbose)
			fprintf(stderr, "Close down completed, all components disconnected, disabled and destroyed\n\n");
	}
	if (status != 0)
		raspicamcontrol_check_configuration(128);

	pthread_exit (0);
	//return 0;
}

int main(int argc, const char **argv)
{

	signal(SIGINT, signal_handler);

	pthread_t threadRecording;
	void *ret;

	char ch;
	cout << "Program started waiting command "<<endl;
	while((ch = getchar()) != NULL)
	{
		if(ch == 's')
		{
			if(threadRecording == NULL)
			{
				cout << "Starting capture thread"<<endl;
				bRecording = true;
				if (pthread_create (&threadRecording, NULL, recording_thread, NULL) < 0)
				{
					cerr <<"pthread_create error for thread " << endl;
					break;
				}
			}
		}
		if(ch == 'p')
		{
			cout << "Stopping capture thread"<<endl;
			bRecording = false;
			if(threadRecording != NULL)
				pthread_join (threadRecording, NULL);
			threadRecording = NULL;
		}
		if(ch == 'q')
		{
			cout << "Quit"<<endl;
			bRecording = false;
			break;
		}
		usleep(100);
	}
	cout << "Exiting, killing threads "<<endl;
	if(threadRecording != NULL)
	{
		cout << "Waiting for the thread to end "<<endl;
		pthread_join (threadRecording, NULL);
	}
	else
		cout << "Thread already dead skipping"<<endl;

	//pthread_exit(NULL);
	return EXIT_SUCCESS;
}
