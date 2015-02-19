

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RaspiStill.h"

/**
 * main
 */
int main(int argc, const char **argv)
{
   // Our main data storage vessel..
   RASPISTILL_STATE state;

   MMAL_STATUS_T status = MMAL_SUCCESS;
   MMAL_PORT_T *camera_preview_port = NULL;
   MMAL_PORT_T *camera_video_port = NULL;
   MMAL_PORT_T *camera_still_port = NULL;
   MMAL_PORT_T *preview_input_port = NULL;
   MMAL_PORT_T *encoder_input_port = NULL;
   MMAL_PORT_T *encoder_output_port = NULL;

   bcm_host_init();

   // Register our application with the logging system
   vcos_log_register("RaspiStill", VCOS_LOG_CATEGORY);

   signal(SIGINT, signal_handler);

   default_status(&state);

   // Do we have any parameters
   if (argc == 1)
   {
      fprintf(stderr, "\%s Camera App %s\n\n", basename(argv[0]), VERSION_STRING);

      display_valid_parameters((char*)basename(argv[0]));
      exit(0);
   }

   // Parse the command line and put options in to our status structure
   if (parse_cmdline(argc, argv, &state))
   {
      exit(0);
   }

   if (state.verbose)
   {
      fprintf(stderr, "\n%s Camera App %s\n\n", basename(argv[0]), VERSION_STRING);

      dump_status(&state);
   }

   // OK, we have a nice set of parameters. Now set up our components
   // We have three components. Camera, Preview and encoder.
   // Camera and encoder are different in stills/video, but preview
   // is the same so handed off to a separate module

   if (!create_camera_component(&state))
   {
      vcos_log_error("%s: Failed to create camera component", __func__);
   }
   else if (!raspipreview_create(&state.preview_parameters))
   {
      vcos_log_error("%s: Failed to create preview component", __func__);
      destroy_camera_component(&state);
   }
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

      camera_preview_port = state.camera_component->output[MMAL_CAMERA_PREVIEW_PORT];
      camera_video_port   = state.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
      camera_still_port   = state.camera_component->output[MMAL_CAMERA_CAPTURE_PORT];
      preview_input_port  = state.preview_parameters.preview_component->input[0];
      encoder_input_port  = state.encoder_component->input[0];
      encoder_output_port = state.encoder_component->output[0];

      if (state.preview_parameters.wantPreview )
      {
         if (state.verbose)
         {
            fprintf(stderr, "Connecting camera preview port to preview input port\n");
            fprintf(stderr, "Starting video preview\n");
         }

         // Connect camera to preview
         status = connect_ports(camera_preview_port, preview_input_port, &state.preview_connection);
      }
      else
      {
         status = MMAL_SUCCESS;
      }

      if (status == MMAL_SUCCESS)
      {
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
         vcos_status = vcos_semaphore_create(&callback_data.complete_semaphore, "RaspiStill-sem", 0);

         vcos_assert(vcos_status == VCOS_SUCCESS);

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

         if (state.demoMode)
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
         else
         {
            int num_iterations =  state.timelapse ? state.timeout / state.timelapse : 1;
            int frame;
            FILE *output_file = NULL;

            for (frame = 1;frame<=num_iterations; frame++)
            {
               if (state.timelapse)
                  vcos_sleep(state.timelapse);
               else
                  vcos_sleep(state.timeout);

               // Open the file
               if (state.filename)
               {
		            if (state.filename[0] == '-')
    		         {
		               output_file = stdout;

		               // Ensure we don't upset the output stream with diagnostics/info
		               state.verbose = 0;
    		         }
                  else
                  {
                     char *use_filename = state.filename;
	
	                  if (state.timelapse)
	                     asprintf(&use_filename, state.filename, frame);
	
	                  if (state.verbose)
	                     fprintf(stderr, "Opening output file %s\n", use_filename);
	
	                  output_file = fopen(use_filename, "wb");
	
	                  if (!output_file)
	                  {
	                     // Notify user, carry on but discarding encoded output buffers
	                     vcos_log_error("%s: Error opening output file: %s\nNo output file will be generated\n", __func__, use_filename);
	                  }

	                  // asprintf used in timelapse mode allocates its own memory which we need to free
	                  if (state.timelapse)
	                     free(use_filename);
                  }
									
                  add_exif_tags(&state);

                  callback_data.file_handle = output_file;
               }

               // We only capture if a filename was specified and it opened
               if (output_file)
               {
                  // Send all the buffers to the encoder output port
                  int num = mmal_queue_length(state.encoder_pool->queue);
                  int q;

                  for (q=0;q<num;q++)
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

            } // end for (frame)

            vcos_semaphore_delete(&callback_data.complete_semaphore);
         }
      }
      else
      {
         mmal_status_to_int(status);
         vcos_log_error("%s: Failed to connect camera to preview", __func__);
      }

error:

      mmal_status_to_int(status);

      if (state.verbose)
         fprintf(stderr, "Closing down\n");

      // Disable all our ports that are not handled by connections
      check_disable_port(camera_video_port);
      check_disable_port(encoder_output_port);

      if (state.preview_parameters.wantPreview )
         mmal_connection_destroy(state.preview_connection);

      mmal_connection_destroy(state.encoder_connection);

      /* Disable components */
      if (state.encoder_component)
         mmal_component_disable(state.encoder_component);

      if (state.preview_parameters.preview_component)
         mmal_component_disable(state.preview_parameters.preview_component);

      if (state.camera_component)
         mmal_component_disable(state.camera_component);

      destroy_encoder_component(&state);
      raspipreview_destroy(&state.preview_parameters);
      destroy_camera_component(&state);

      if (state.verbose)
         fprintf(stderr, "Close down completed, all components disconnected, disabled and destroyed\n\n");
   }
   if (status != 0)
      raspicamcontrol_check_configuration(128);

   return 0;
}