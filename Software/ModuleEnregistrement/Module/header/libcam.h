/*
 * Copyright (C) 2009 Giacomo Spigler
 * CopyPolicy: Released under the terms of the GNU GPL v3.0.
 */

#ifndef __LIBCAM__H__
#define __LIBCAM__H__

//#define USE_OPENCV 1

#ifdef USE_OPENCV
#include <cv.h>
#endif



struct buffer
{
	void *                  start;
	size_t                  length;
};

typedef enum
{
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR
} io_method;





class Camera
{
private:


	void init_userp(unsigned int buffer_size);
	void init_mmap();
	void init_read(unsigned int buffer_size);

	bool initialised;

	char *name;  //dev_name
public:

	bool Open();
	void Close();

	bool Init();
	void UnInit();

	bool Start();
	void Stop();
	bool isInit();


	int width;
	int height;
	int fps;

	int w2;

	unsigned char *data;

	io_method io;
	int fd;
	buffer *buffers;
	int n_buffers;

	int mb, Mb, db, mc, Mc, dc, ms, Ms, ds, mh, Mh, dh, msh, Msh, dsh;
	bool ha;


	Camera(const char *name, int w, int h, int fps=30);
	~Camera();

	unsigned char *Get();    //deprecated
	bool Update(unsigned int t=100, int timeout_ms=500); //better  (t=0.1ms, in usecs)
	bool Update(Camera *c2, unsigned int t=100, int timeout_ms=500);

#ifdef USE_OPENCV
	void toIplImage(IplImage *im);
#endif


	void StopCam();

	int minBrightness();
	int maxBrightness();
	int defaultBrightness();
	int minContrast();
	int maxContrast();
	int defaultContrast();
	int minSaturation();
	int maxSaturation();
	int defaultSaturation();
	int minHue();
	int maxHue();
	int defaultHue();
	bool isHueAuto();
	int minSharpness();
	int maxSharpness();
	int defaultSharpness();

	int setBrightness(int v);
	int setContrast(int v);
	int setSaturation(int v);
	int setHue(int v);
	int setHueAuto(bool v);
	int setSharpness(int v);



};





#endif
