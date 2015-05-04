/*
   Copyright (c) 2012, Broadcom Europe Ltd
   Copyright (c) 2012, Kalle Vahlman <zuh@iki>
   Tuomas Kulve <tuomas@kulve.fi>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this ComponentsList of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this ComponentsList of conditions and the following disclaimer in the
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

// Video encode demo using OpenMAX IL though the ilcient helper library


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bcm_host.h"
#include "ilclient.h"

#include "encode.h"

COMPONENT_T *video_encode = NULL;
COMPONENT_T *ComponentsList[5];
ILCLIENT_T *client;


void print_def(OMX_PARAM_PORTDEFINITIONTYPE def)
{
	printf("Port %u: %s %u/%u %u %u %s,%s,%s %ux%u %ux%u @%u %u\n",
		   def.nPortIndex,
		   def.eDir == OMX_DirInput ? "in" : "out",
		   def.nBufferCountActual,
		   def.nBufferCountMin,
		   def.nBufferSize,
		   def.nBufferAlignment,
		   def.bEnabled ? "enabled" : "disabled",
		   def.bPopulated ? "populated" : "not pop.",
		   def.bBuffersContiguous ? "contig." : "not cont.",
		   def.format.video.nFrameWidth,
		   def.format.video.nFrameHeight,
		   def.format.video.nStride,
		   def.format.video.nSliceHeight,
		   def.format.video.xFramerate, def.format.video.eColorFormat);
}

bool initH264(int _height,int _width,unsigned char _frameRate)
{

	OMX_VIDEO_PARAM_PORTFORMATTYPE format;
	OMX_PARAM_PORTDEFINITIONTYPE def;
	OMX_ERRORTYPE r;
	memset(ComponentsList, 0, sizeof(ComponentsList));

	if ((client = ilclient_init()) == NULL)
	{
		return false;
	}

	if (OMX_Init() != OMX_ErrorNone)
	{
		ilclient_destroy(client);
		return false;
	}

	// create video_encode
	r = ilclient_create_component(client, &video_encode, "video_encode",
								  ILCLIENT_DISABLE_ALL_PORTS |
								  ILCLIENT_ENABLE_INPUT_BUFFERS |
								  ILCLIENT_ENABLE_OUTPUT_BUFFERS);
	if (r != 0)
	{
		printf
		("ilclient_create_component() for video_encode failed with %x!\n",
		 r);
		return false;
	}
	ComponentsList[0] = video_encode;

	// get current settings of video_encode component from port 200
	memset(&def, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
	def.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	def.nVersion.nVersion = OMX_VERSION;
	def.nPortIndex = 200;

	if (OMX_GetParameter
			(ILC_GET_HANDLE(video_encode), OMX_IndexParamPortDefinition,
			 &def) != OMX_ErrorNone)
	{
		printf("%s:%d: OMX_GetParameter() for video_encode port 200 failed!\n",
			   __FUNCTION__, __LINE__);
		return false;
	}

	print_def(def);

	// Port 200: in 1/1 115200 16 enabled,not pop.,not cont. 320x240 320x240 @1966080 20
	def.format.video.nFrameWidth = _width;
	def.format.video.nFrameHeight = _height;
	def.format.video.xFramerate = _frameRate << 16;
	def.format.video.nSliceHeight = def.format.video.nFrameHeight;
	def.format.video.nStride = def.format.video.nFrameWidth;
	def.format.video.eColorFormat = OMX_COLOR_Format24bitBGR888;
	//def.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;

	print_def(def);

	r = OMX_SetParameter(ILC_GET_HANDLE(video_encode),
						 OMX_IndexParamPortDefinition, &def);
	if (r != OMX_ErrorNone)
	{
		printf
		("%s:%d: OMX_SetParameter() for video_encode port 200 failed with %x!\n",
		 __FUNCTION__, __LINE__, r);
		return false;
	}

	memset(&format, 0, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
	format.nSize = sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE);
	format.nVersion.nVersion = OMX_VERSION;
	format.nPortIndex = 201;
	format.eCompressionFormat = OMX_VIDEO_CodingAVC;

	printf("OMX_SetParameter for video_encode:201...\n");
	r = OMX_SetParameter(ILC_GET_HANDLE(video_encode),
						 OMX_IndexParamVideoPortFormat, &format);
	if (r != OMX_ErrorNone)
	{
		printf
		("%s:%d: OMX_SetParameter() for video_encode port 201 failed with %x!\n",
		 __FUNCTION__, __LINE__, r);
		return false;
	}

	OMX_VIDEO_PARAM_BITRATETYPE bitrateType;
	// set current bitrate to 1Mbit
	memset(&bitrateType, 0, sizeof(OMX_VIDEO_PARAM_BITRATETYPE));
	bitrateType.nSize = sizeof(OMX_VIDEO_PARAM_BITRATETYPE);
	bitrateType.nVersion.nVersion = OMX_VERSION;
	bitrateType.eControlRate = OMX_Video_ControlRateVariable;
	bitrateType.nTargetBitrate = 5000000;
	bitrateType.nPortIndex = 201;
	r = OMX_SetParameter(ILC_GET_HANDLE(video_encode),
						 OMX_IndexParamVideoBitrate, &bitrateType);
	if (r != OMX_ErrorNone)
	{
		printf
		("%s:%d: OMX_SetParameter() for bitrate for video_encode port 201 failed with %x!\n",
		 __FUNCTION__, __LINE__, r);
		return false;
	}


	// get current bitrate
	memset(&bitrateType, 0, sizeof(OMX_VIDEO_PARAM_BITRATETYPE));
	bitrateType.nSize = sizeof(OMX_VIDEO_PARAM_BITRATETYPE);
	bitrateType.nVersion.nVersion = OMX_VERSION;
	bitrateType.nPortIndex = 201;

	if (OMX_GetParameter
			(ILC_GET_HANDLE(video_encode), OMX_IndexParamVideoBitrate,
			 &bitrateType) != OMX_ErrorNone)
	{
		printf("%s:%d: OMX_GetParameter() for video_encode for bitrate port 201 failed!\n",
			   __FUNCTION__, __LINE__);
		return false;
	}
	printf("Current Bitrate=%u\n",bitrateType.nTargetBitrate);



	printf("encode to idle...\n");
	if (ilclient_change_component_state(video_encode, OMX_StateIdle) == -1)
	{
		printf
		("%s:%d: ilclient_change_component_state(video_encode, OMX_StateIdle) failed",
		 __FUNCTION__, __LINE__);
	}

	printf("enabling port buffers for 200...\n");
	if (ilclient_enable_port_buffers(video_encode, 200, NULL, NULL, NULL) != 0)
	{
		printf("enabling port buffers for 200 failed!\n");
		return false;
	}

	printf("enabling port buffers for 201...\n");
	if (ilclient_enable_port_buffers(video_encode, 201, NULL, NULL, NULL) != 0)
	{
		printf("enabling port buffers for 201 failed!\n");
		return false;
	}

	printf("encode to executing...\n");
	ilclient_change_component_state(video_encode, OMX_StateExecuting);

	return true;
}
void deinitH264()
{
	//fclose(outf);

	printf("Teardown.\n");

	printf("disabling port buffers for 200 and 201...\n");
	ilclient_disable_port_buffers(video_encode, 200, NULL, NULL, NULL);
	ilclient_disable_port_buffers(video_encode, 201, NULL, NULL, NULL);

	ilclient_state_transition(ComponentsList, OMX_StateIdle);
	ilclient_state_transition(ComponentsList, OMX_StateLoaded);

	ilclient_cleanup_components(ComponentsList);

	OMX_Deinit();

	ilclient_destroy(client);
}

void writeImageH264(char *image, int32_t filledLen,FILE * fileDesc)
{
	OMX_BUFFERHEADERTYPE *buf;
	OMX_BUFFERHEADERTYPE *out;
	OMX_ERRORTYPE r;

	/*while((buf = ilclient_get_input_buffer(video_encode, 200, 1) )== NULL)
	{
		printf("Doh, no buffers for me!\n");
	}*/
	buf = ilclient_get_input_buffer(video_encode, 200, 1);
	if(buf != NULL)
	{
		/* fill it */
		memcpy((unsigned char*)buf->pBuffer,(unsigned char*)image,filledLen);
		buf->nFilledLen = filledLen;

		if ((r= OMX_EmptyThisBuffer(ILC_GET_HANDLE(video_encode), buf) )!=
				OMX_ErrorNone)
		{
			printf("Error emptying buffer! :%x\n",r);
		}

		out = ilclient_get_output_buffer(video_encode, 201, 1);

		r = OMX_FillThisBuffer(ILC_GET_HANDLE(video_encode), out);
		if (r != OMX_ErrorNone)
		{
			printf("Error filling buffer: %x\n", r);
		}

		if (out != NULL)
		{
			if(fileDesc != NULL )
			{
				r = fwrite(out->pBuffer, 1, out->nFilledLen, fileDesc);
				if (r != out->nFilledLen)
				{
					printf("fwrite: Error emptying buffer: %d!\n", r);
				}

			}
			out->nFilledLen = 0;
		}
		else
		{
			printf("Not getting it :(\n");
		}

	}

}


