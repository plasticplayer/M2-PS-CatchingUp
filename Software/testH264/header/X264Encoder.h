/*
 
  # X264Encoder
 
  Simple wrapper for x264 that you can use to encode into x264. Make
  sure to set all the params before calling `open()`. See belwo for the 
  members that the you need to set. 
 
 */
#ifndef ROXLU_X264_ENCODER_H
#define ROXLU_X264_ENCODER_H
#include <inttypes.h>
#include <stdio.h>
#include <string>
 
//extern "C" {
#include <x264.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixfmt.h>
#define AV_PIX_FMT_YUV420P PIX_FMT_YUV420P
#define AV_PIX_FMT_NONE PIX_FMT_NONE
//}
#define AVPixelFormat PixelFormat
class X264Encoder {
 public:
  X264Encoder();
  ~X264Encoder();
  bool open(std::string filename, bool datapath);               /* open for encoding */
  bool encode(char* pixels);                                    /* encode the given data */
  bool close();                                                 /* close the encoder and file, frees all memory */
 private:
  bool validateSettings();                                      /* validates if all params are set correctly, like width,height, etc.. */
  void setParams();                                             /* sets the x264 params */
 
 public:
  /* params the user should set */
  int in_width;
  int in_height;
  int out_width;
  int out_height;
  int fps;                                                       /* e.g. 25, 60, etc.. */
  AVPixelFormat in_pixel_format;
  AVPixelFormat out_pixel_format;
 
  /* x264 */
  AVPicture pic_raw;                                            /* used for our "raw" input container */
  x264_picture_t pic_in;
  x264_picture_t pic_out;
  x264_param_t params;
  x264_nal_t* nals;
  x264_t* encoder;
  int num_nals;
 
  /* input / output */
  int pts;
  struct SwsContext* sws;
  FILE* fp;
};
 
#endif
