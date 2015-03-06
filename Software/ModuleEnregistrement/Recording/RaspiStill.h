/*
Copyright (c) 2013, Broadcom Europe Ltd
Copyright (c) 2013, James Hughes
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef RASPISTILLE_H_
#define RASPISTILLE_H_
/**
 * \file RaspiStill.c
 * Command line program to capture a still frame and encode it to file.
 * Also optionally display a preview/viewfinder of current camera input.
 *
 * \date 31 Jan 2013
 * \Author: James Hughes
 *
 * Description
 *
 * 3 components are created; camera, preview and JPG encoder.
 * Camera component has three ports, preview, video and stills.
 * This program connects preview and stills to the preview and jpg
 * encoder. Using mmal we don't need to worry about buffers between these
 * components, but we do need to handle buffers from the encoder, which
 * are simply written straight to the file in the requisite buffer callback.
 *
 * We use the RaspiCamControl code to handle the specific camera settings.
 */

// We use some GNU extensions (asprintf, basename)
//#define _GNU_SOURCE

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <memory.h>

#define VERSION_STRING "v1.1"

#include "bcm_host.h"
#include "interface/vcos/vcos.h"

#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_logging.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"


#include "RaspiCamControl.h"
#include "RaspiPreview.h"
#include "RaspiCLI.h"
//#include "RaspiStill.h"
//#include <semaphore.h>

/// Camera number to use - we only have one camera, indexed from 0.
#define CAMERA_NUMBER 0

// Standard port setting for the camera component
#define MMAL_CAMERA_PREVIEW_PORT 0
#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2


// Stills format information
#define STILLS_FRAME_RATE_NUM 3
#define STILLS_FRAME_RATE_DEN 1

// Video render needs at least 2 buffers.
#define VIDEO_OUTPUT_BUFFERS_NUM 3

#define MAX_USER_EXIF_TAGS      32
#define MAX_EXIF_PAYLOAD_LENGTH 128
#ifdef __cplusplus
extern "C" {
#include <string.h>
#endif
int mmal_status_to_int(MMAL_STATUS_T status);

/** Structure containing all state information for the current run
 */
typedef struct
{
   int timeout;                        /// Time taken before frame is grabbed and app then shuts down. Units are milliseconds
   int width;                          /// Requested width of image
   int height;                         /// requested height of image
   int quality;                        /// JPEG quality setting (1-100)
   int wantRAW;                        /// Flag for whether the JPEG metadata also contains the RAW bayer image
   char *filename;                     /// filename of output file
   MMAL_PARAM_THUMBNAIL_CONFIG_T thumbnailConfig;
   int verbose;                        /// !0 if want detailed run information
   int demoMode;                       /// Run app in demo mode
   int demoInterval;                   /// Interval between camera settings changes
   MMAL_FOURCC_T encoding;             /// Encoding to use for the output file.
   const char *exifTags[MAX_USER_EXIF_TAGS]; /// Array of pointers to tags supplied from the command line
   int numExifTags;                    /// Number of supplied tags
   int timelapse;                      /// Delay between each picture in timelapse mode. If 0, disable timelapse

   RASPIPREVIEW_PARAMETERS preview_parameters;    /// Preview setup parameters
   RASPICAM_CAMERA_PARAMETERS camera_parameters; /// Camera setup parameters

   MMAL_COMPONENT_T *camera_component;    /// Pointer to the camera component
   MMAL_COMPONENT_T *encoder_component;   /// Pointer to the encoder component
   MMAL_CONNECTION_T *preview_connection; /// Pointer to the connection from camera to preview
   MMAL_CONNECTION_T *encoder_connection; /// Pointer to the connection from camera to encoder

   MMAL_POOL_T *encoder_pool; /// Pointer to the pool of buffers used by encoder output port

} RASPISTILL_STATE;

/** Struct used to pass information in encoder port userdata to callback
 */
typedef struct
{
   FILE *file_handle;                   /// File handle to write buffer data to.
   VCOS_SEMAPHORE_T complete_semaphore; /// semaphore which is posted when we reach end of frame (indicates end of capture or fault)
   RASPISTILL_STATE *pstate;            /// pointer to our state in case required in callback
   uint8_t bMemOutput;
   uint8_t ** pBuffer;
   int * pBufferSize;
} PORT_USERDATA;


/*static*/ void display_valid_parameters(const char *app_name);
/*static*/ void default_status(RASPISTILL_STATE *state);
/*static*/ void dump_status(RASPISTILL_STATE *state);
/*static*/ int parse_cmdline(int argc, const char **argv, RASPISTILL_STATE *state);
/*static*/ void camera_control_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer);
/*static*/ void encoder_buffer_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer);
/*static*/ MMAL_COMPONENT_T *create_camera_component(RASPISTILL_STATE *state);
/*static*/ void destroy_camera_component(RASPISTILL_STATE *state);
/*static*/ MMAL_COMPONENT_T *create_encoder_component(RASPISTILL_STATE *state);
/*static*/ void destroy_encoder_component(RASPISTILL_STATE *state);
/*static*/ MMAL_STATUS_T add_exif_tag(RASPISTILL_STATE *state, const char *exif_tag);
/*static*/ void store_exif_tag(RASPISTILL_STATE *state, const char *exif_tag);
/*static*/ void add_exif_tags(RASPISTILL_STATE *state);
/*static*/ MMAL_STATUS_T connect_ports(MMAL_PORT_T *output_port, MMAL_PORT_T *input_port, MMAL_CONNECTION_T **connection);
/*static*/ void check_disable_port(MMAL_PORT_T *port);
/*static*/ void signal_handler(int signal_number);
/// Comamnd ID's and Structure defining our command line options
#define CommandHelp         0
#define CommandWidth        1
#define CommandHeight       2
#define CommandQuality      3
#define CommandRaw          4
#define CommandOutput       5
#define CommandVerbose      6
#define CommandTimeout      7
#define CommandThumbnail    8
#define CommandDemoMode     9
#define CommandEncoding     10
#define CommandExifTag      11
#define CommandTimelapse    12



#ifdef __cplusplus
}
#endif
#endif
