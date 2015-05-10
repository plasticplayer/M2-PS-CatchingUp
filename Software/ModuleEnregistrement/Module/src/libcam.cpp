/*
   Copyright (C) 2009 Giacomo Spigler

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))



#include "libcam.h"

#ifdef USE_OPENCV
#include <cv.h>
#endif


static int xioctl(int fd, int request, void *arg)
{
	int r,itt=0;

	do
	{
		r = ioctl (fd, request, arg);
		itt++;
	}
	while ((-1 == r) && (EINTR == errno) && (itt<100));

	return r;
}

Camera::Camera(const char *n, int w, int h, int f)
{
	name = new char[strlen(n)+1];
	memcpy (this->name, n, strlen(n)+1 );
	//name=n;
	width=w;
	height=h;
	fps=f;

	w2=w/2;


	io=IO_METHOD_MMAP;



	/*this->Open();
	  this->Init();
	  this->Start();
	  initialised = true;*/
}

Camera::~Camera()
{
	this->StopCam();
}

void Camera::StopCam()
{
	if (initialised)
	{
		this->Stop();
		this->UnInit();
		this->Close();
		LOGGER_VERB("Closing Webcam Done");
		free(data);
		initialised = false;
	}
}

bool Camera::Open()
{
	LOGGER_VERB("Opening Camera ");
	struct stat st;
	if(-1==stat(name, &st))
	{
		LOGGER_ERROR("Openning Camera : Cannot identify "<< name << " : "<<errno << ", ("<<strerror(errno)<<")");
		return false;
	}

	if(!S_ISCHR(st.st_mode))
	{
		LOGGER_ERROR("Openning Camera : "<< name << "is not a device ");
		return false;
	}

	fd=open(name, O_RDWR | O_NONBLOCK, 0);

	if(-1 == fd)
	{
		LOGGER_ERROR("Openning Camera : Cannot open "<< name << " : "<<errno << ", ("<<strerror(errno)<<")");
		return false;
	}

	data=(unsigned char *)malloc(width*height*4);
	return true;
}

void Camera::Close()
{
	if(-1==close(fd))
	{
		LOGGER_ERROR("Closing Camera : Cannot close :"<<errno << ", ("<<strerror(errno)<<")");
	}
	fd=-1;

}
bool Camera::isInit()
{
	return initialised;
}

bool Camera::Init()
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if(-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap))
	{
		if (EINVAL == errno)
		{
			LOGGER_ERROR("Init Camera : "<<name << " is not a V4L2 device ");
			return false;
		}
		else
		{
			LOGGER_ERROR("Init Camera : "<<name << " error : "<<errno << ", ("<<strerror(errno)<<")");
			return false;
		}
	}

	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		LOGGER_ERROR("Init Camera : "<<name << " is no video capture device");
		return false;
	}

	switch(io)
	{
		case IO_METHOD_READ:
			if(!(cap.capabilities & V4L2_CAP_READWRITE))
			{
				LOGGER_ERROR("Init Camera : " << name << " does not support read i/o");
				return false;
			}

			break;

		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
			if(!(cap.capabilities & V4L2_CAP_STREAMING))
			{
				LOGGER_ERROR("Init Camera : " << name << " does not support streaming i/o");
				return false;
			}
			break;
	}


	CLEAR (cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if(0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap))
	{
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; /* reset to default */

		if(-1 == xioctl (fd, VIDIOC_S_CROP, &crop))
		{
			switch (errno)
			{
				case EINVAL:
					/* Cropping not supported. */
					break;
				default:
					/* Errors ignored. */
					break;
			}
		}
	}
	else
	{
		/* Errors ignored. */
	}

	CLEAR (fmt);

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = width;
	fmt.fmt.pix.height      = height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	//fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
	char retry = 5;
	int ret = xioctl (fd, VIDIOC_S_FMT, &fmt); 
	while(retry > 0 && -1 == ret)
	{

		CLEAR (fmt);
		fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width       = width;
		fmt.fmt.pix.height      = height;
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		LOGGER_ERROR("Init Camera : Try "<< (int) retry);
		retry--;
		ret = xioctl (fd, VIDIOC_S_FMT, &fmt);
	}
	if(ret == -1)
	{
		LOGGER_ERROR("Init Camera : " << name << " cannot set image size : error : "<<errno << ", ("<<strerror(errno)<<")");
		return false;
	}


	/*
	   struct v4l2_standard s;
	   s.name[0]='A';
	   s.frameperiod.numerator=1;
	   s.frameperiod.denominator=fps;

	   if(-1==xioctl(fd, VIDIOC_S_STD, &s))
	   errno_exit("VIDIOC_S_STD");
	 */


	struct v4l2_streamparm p;
	p.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	//p.parm.capture.capability=V4L2_CAP_TIMEPERFRAME;
	//p.parm.capture.capturemode=V4L2_MODE_HIGHQUALITY;
	p.parm.capture.timeperframe.numerator=1;
	p.parm.capture.timeperframe.denominator=fps;
	p.parm.output.timeperframe.numerator=1;
	p.parm.output.timeperframe.denominator=fps;
	//p.parm.output.outputmode=V4L2_MODE_HIGHQUALITY;
	//p.parm.capture.extendedmode=0;
	//p.parm.capture.readbuffers=n_buffers;


	if(-1==xioctl(fd, VIDIOC_S_PARM, &p))
	{
		LOGGER_ERROR("Init Camera : " << name << " cannot set frame rate : error : "<<errno << ", ("<<strerror(errno)<<")");
		return false;
	}


	//default values, mins and maxes
	struct v4l2_queryctrl queryctrl;

	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CID_BRIGHTNESS;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			LOGGER_WARN("brightness error");
		}
		else
		{
			LOGGER_DEBUG("brightness is not supported");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		LOGGER_DEBUG ("brightness is not supported");
	}
	mb=queryctrl.minimum;
	Mb=queryctrl.maximum;
	db=queryctrl.default_value;


	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CID_CONTRAST;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			LOGGER_WARN("contrast error");
		}
		else
		{
			LOGGER_DEBUG("contrast is not supported");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		LOGGER_DEBUG ("contrast is not supported");
	}
	mc=queryctrl.minimum;
	Mc=queryctrl.maximum;
	dc=queryctrl.default_value;


	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CID_SATURATION;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			LOGGER_WARN("saturation error\n");
		}
		else
		{
			LOGGER_DEBUG("saturation is not supported");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		LOGGER_DEBUG ("saturation is not supported");
	}
	ms=queryctrl.minimum;
	Ms=queryctrl.maximum;
	ds=queryctrl.default_value;


	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CID_HUE;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			LOGGER_WARN("hue error");
		}
		else
		{
			LOGGER_DEBUG("hue is not supported");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		LOGGER_DEBUG ("hue is not supported");
	}
	mh=queryctrl.minimum;
	Mh=queryctrl.maximum;
	dh=queryctrl.default_value;


	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CID_HUE_AUTO;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			LOGGER_WARN("hueauto error");
		}
		else
		{
			LOGGER_DEBUG("hueauto is not supported");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		LOGGER_DEBUG ("hueauto is not supported");
	}
	ha=queryctrl.default_value;


	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CID_SHARPNESS;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			LOGGER_WARN("sharpness error");
		}
		else
		{
			LOGGER_DEBUG("sharpness is not supported");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		LOGGER_DEBUG ("sharpness is not supported");
	}
	msh=queryctrl.minimum;
	Msh=queryctrl.maximum;
	dsh=queryctrl.default_value;

	//TODO: TO ADD SETTINGS
	//here should go custom calls to xioctl

	//END TO ADD SETTINGS

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if(fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if(fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	switch(io)
	{
		case IO_METHOD_READ:
			init_read(fmt.fmt.pix.sizeimage);
			break;

		case IO_METHOD_MMAP:
			init_mmap();
			break;

		case IO_METHOD_USERPTR:
			init_userp(fmt.fmt.pix.sizeimage);
			break;
	}
	initialised = true;
	return true;
}

void Camera::init_userp(unsigned int buffer_size)
{
	/*
	   struct v4l2_requestbuffers req;
	   unsigned int page_size;

	   page_size = getpagesize();
	   buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

	   CLEAR (req);

	   req.count               = 4;
	   req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	   req.memory              = V4L2_MEMORY_USERPTR;

	   if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
	   if (EINVAL == errno) {
	   fprintf (stderr, "%s does not support user pointer i/o\n", name);
	//exit (EXIT_FAILURE);
	} else {
	errno_exit ("VIDIOC_REQBUFS");
	}
	}

	buffers = calloc (4, sizeof (*buffers));

	if (!buffers) {
	fprintf (stderr, "Out of memory\n");
	exit (EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
	buffers[n_buffers].length = buffer_size;
	buffers[n_buffers].start = memalign (page_size,
	buffer_size);

	if (!buffers[n_buffers].start) {
	fprintf (stderr, "Out of memory\n");
	exit (EXIT_FAILURE);
	}
	}


	 */
}

void Camera::init_mmap()
{
	struct v4l2_requestbuffers req;

	CLEAR (req);

	req.count               = 4;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;

	if(-1 == xioctl (fd, VIDIOC_REQBUFS, &req))
	{
		if(EINVAL == errno)
		{
			LOGGER_ERROR (name<<" does not support memory mapping");
			return;
		}
		else
		{
			LOGGER_ERROR (name<<" does not support memory mapping error : "<<errno << ", ("<<strerror(errno)<<")");
			return;
		}
	}

	if(req.count < 2)
	{
		LOGGER_ERROR ("Insufficient buffer memory on "<< name);
		//fprintf (stderr, "Insufficient buffer memory on %s\n", name);
		return;
	}

	buffers = (buffer *)calloc(req.count, sizeof (*buffers));

	if(!buffers)
	{
		LOGGER_ERROR ("Out of memory ");
		//fprintf (stderr, "Insufficient buffer memory on %s\n", name);
		return;
	}

	for(n_buffers = 0; n_buffers < (int)req.count; ++n_buffers)
	{
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if(-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))
		{
			LOGGER_ERROR ("VIDIOC_QUERYBUF error : "<<errno << ", ("<<strerror(errno)<<")");
			//fprintf (stderr, "Insufficient buffer memory on %s\n", name);
			return;
		}


		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap (NULL /* start anywhere */,
				buf.length,
				PROT_READ | PROT_WRITE /* required */,
				MAP_SHARED /* recommended */,
				fd, buf.m.offset);

		if(MAP_FAILED == buffers[n_buffers].start)
		{
			LOGGER_ERROR ("mmap error : "<<errno << ", ("<<strerror(errno)<<")");
			//fprintf (stderr, "Insufficient buffer memory on %s\n", name);
			return;
		}
	}

}

void Camera::init_read (unsigned int buffer_size)
{
	/*
	   buffers = calloc (1, sizeof (*buffers));

	   if (!buffers) {
	   fprintf (stderr, "Out of memory\n");
	   exit (EXIT_FAILURE);
	   }

	   buffers[0].length = buffer_size;
	   buffers[0].start = malloc (buffer_size);

	   if (!buffers[0].start) {
	   fprintf (stderr, "Out of memory\n");
	   exit (EXIT_FAILURE);
	   }
	 */
}

void Camera::UnInit()
{
	unsigned int i;

	switch(io)
	{
		case IO_METHOD_READ:
			free (buffers[0].start);
			break;

		case IO_METHOD_MMAP:
			for(i = 0; i < (unsigned int)n_buffers; ++i)
				if(-1 == munmap (buffers[i].start, buffers[i].length))
				{
					LOGGER_ERROR ("munmap error : "<<errno << ", ("<<strerror(errno)<<")");
					//fprintf (stderr, "Insufficient buffer memory on %s\n", name);
					return;
				}
			break;

		case IO_METHOD_USERPTR:
			for (i = 0; i < (unsigned int)n_buffers; ++i)
				free (buffers[i].start);
			break;
	}

	free (buffers);
}

bool Camera::Start()
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch(io)
	{
		case IO_METHOD_READ:
			/* Nothing to do. */
			break;

		case IO_METHOD_MMAP:
			for(i = 0; i < (unsigned int)n_buffers; ++i)
			{
				struct v4l2_buffer buf;

				CLEAR (buf);

				buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory      = V4L2_MEMORY_MMAP;
				buf.index       = i;

				if(-1 == xioctl (fd, VIDIOC_QBUF, &buf))
					return false;
			}

			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if(-1 == xioctl (fd, VIDIOC_STREAMON, &type))
				return false;

			break;

		case IO_METHOD_USERPTR:
			for(i = 0; i < (unsigned int)n_buffers; ++i)
			{
				struct v4l2_buffer buf;

				CLEAR (buf);

				buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory      = V4L2_MEMORY_USERPTR;
				buf.index       = i;
				buf.m.userptr	= (unsigned long) buffers[i].start;
				buf.length      = buffers[i].length;

				if(-1 == xioctl (fd, VIDIOC_QBUF, &buf))
					return false;
			}

			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if(-1 == xioctl (fd, VIDIOC_STREAMON, &type))
				return false;

			break;
	}
	return true;

}

void Camera::Stop()
{
	enum v4l2_buf_type type;

	switch(io)
	{
		case IO_METHOD_READ:
			/* Nothing to do. */
			break;

		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if(-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
			{
				LOGGER_ERROR ("VIDIOC_STREAMOFF error : "<<errno << ", ("<<strerror(errno)<<")");
				//fprintf (stderr, "Insufficient buffer memory on %s\n", name);
				return;
			}

			break;
	}

}

unsigned char *Camera::Get()
{
	struct v4l2_buffer buf;

	switch(io)
	{
		case IO_METHOD_READ:
			/*
			   if (-1 == read (fd, buffers[0].start, buffers[0].length)) {
			   switch (errno) {
			   case EAGAIN:
			   return 0;

			   case EIO:


			   default:
			   errno_exit ("read");
			   }
			   }

			   process_image (buffers[0].start);
			 */
			break;

		case IO_METHOD_MMAP:
			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			if(-1 == xioctl (fd, VIDIOC_DQBUF, &buf))
			{
				switch (errno)
				{
					case EAGAIN:
						return 0;
					case EIO:
					default:
						return 0; //errno_exit ("VIDIOC_DQBUF");
				}
			}

			assert(buf.index < (unsigned int)n_buffers);

			memcpy(data, (unsigned char *)buffers[buf.index].start, buffers[buf.index].length);

			if(-1 == xioctl (fd, VIDIOC_QBUF, &buf))
				return 0; //errno_exit ("VIDIOC_QBUF");

			return data;


			break;

		case IO_METHOD_USERPTR:
			/*
			   CLEAR (buf);

			   buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			   buf.memory = V4L2_MEMORY_USERPTR;

			   if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
			   switch (errno) {
			   case EAGAIN:
			   return 0;

			   case EIO:


			   default:
			   errno_exit ("VIDIOC_DQBUF");
			   }
			   }

			   for (i = 0; i < n_buffers; ++i)
			   if (buf.m.userptr == (unsigned long) buffers[i].start
			   && buf.length == buffers[i].length)
			   break;

			   assert (i < n_buffers);

			   process_image ((void *) buf.m.userptr);

			   if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			   errno_exit ("VIDIOC_QBUF");
			 */
			break;
	}

	return 0;
}

bool Camera::Update(unsigned int t, int timeout_ms)
{
	bool grabbed = false;
	int grab_time_uS = 0;
	while (!grabbed)
	{
		if ((!grabbed) && (this->Get()!=0)) grabbed = true;
		if (!grabbed)
		{
			usleep(t);
			grab_time_uS+=(int)t;
			if (grab_time_uS > timeout_ms * 1000)
			{
				break;
			}
		}
	}

	return grabbed;

}

bool Camera::Update(Camera *c2, unsigned int t, int timeout_ms)
{
	bool left_grabbed = false;
	bool right_grabbed = false;
	int grab_time_uS = 0;
	while (!(left_grabbed && right_grabbed))
	{
		if ((!left_grabbed) && (this->Get()!=0)) left_grabbed = true;
		if ((!right_grabbed) && (c2->Get()!=0)) right_grabbed = true;
		if (!(left_grabbed && right_grabbed))
		{
			usleep(t);
			grab_time_uS+=(int)t;
			if (grab_time_uS > timeout_ms * 1000)
			{
				break;
			}
		}
	}

	return left_grabbed & right_grabbed;

}

#ifdef USE_OPENCV
void Camera::toIplImage(IplImage *l)
{
	unsigned char *l_=(unsigned char *)l->imageData;


	for(int x=0; x<w2; x++)
	{
		for(int y=0; y<height; y++)
		{
			int y0, y1, u, v; //y0 u y1 v

			int i=(y*w2+x)*4;
			y0=data[i];
			u=data[i+1];
			y1=data[i+2];
			v=data[i+3];

			int r, g, b;
			r = y0 + (1.370705 * (v-128));
			g = y0 - (0.698001 * (v-128)) - (0.337633 * (u-128));
			b = y0 + (1.732446 * (u-128));

			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;
			if(r < 0) r = 0;
			if(g < 0) g = 0;
			if(b < 0) b = 0;

			i=(y*l->width+2*x)*3;
			l_[i] = (unsigned char)(b); //B
			l_[i+1] = (unsigned char)(g); //G
			l_[i+2] = (unsigned char)(r); //R


			r = y1 + (1.370705 * (v-128));
			g = y1 - (0.698001 * (v-128)) - (0.337633 * (u-128));
			b = y1 + (1.732446 * (u-128));

			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;
			if(r < 0) r = 0;
			if(g < 0) g = 0;
			if(b < 0) b = 0;

			l_[i+3] = (unsigned char)(b); //B
			l_[i+4] = (unsigned char)(g); //G
			l_[i+5] = (unsigned char)(r); //R

		}
	}

}
#endif

int Camera::minBrightness()
{
	return mb;
}


int Camera::maxBrightness()
{
	return Mb;
}


int Camera::defaultBrightness()
{
	return db;
}


int Camera::minContrast()
{
	return mc;
}


int Camera::maxContrast()
{
	return Mc;
}


int Camera::defaultContrast()
{
	return dc;
}


int Camera::minSaturation()
{
	return ms;
}


int Camera::maxSaturation()
{
	return Ms;
}


int Camera::defaultSaturation()
{
	return ds;
}


int Camera::minHue()
{
	return mh;
}


int Camera::maxHue()
{
	return Mh;
}


int Camera::defaultHue()
{
	return dh;
}


bool Camera::isHueAuto()
{
	return ha;
}


int Camera::minSharpness()
{
	return msh;
}


int Camera::maxSharpness()
{
	return Msh;
}


int Camera::defaultSharpness()
{
	return dsh;
}

int Camera::setBrightness(int v)
{
	if(v<mb || v>Mb) return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_BRIGHTNESS;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		LOGGER_ERROR ("Error setting brigtness error : "<<errno << ", ("<<strerror(errno)<<")");
		return -1;

	}

	return 1;
}

int Camera::setContrast(int v)
{
	if(v<mc || v>Mc) return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_CONTRAST;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		LOGGER_ERROR ("Error setting contrast error : "<<errno << ", ("<<strerror(errno)<<")");
		return -1;
	}

	return 1;
}

int Camera::setSaturation(int v)
{
	if(v<ms || v>Ms) return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_SATURATION;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		LOGGER_ERROR ("Error setting saturation error : "<<errno << ", ("<<strerror(errno)<<")");
		return -1;
	}

	return 1;
}

int Camera::setHue(int v)
{
	if(v<mh || v>Mh) return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_HUE;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		LOGGER_ERROR ("Error setting hue error : "<<errno << ", ("<<strerror(errno)<<")");
		return -1;
	}

	return 1;
}

int Camera::setHueAuto(bool v)
{
	if(v<mh || v>Mh) return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_HUE_AUTO;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		LOGGER_ERROR ("Error setting auto hue error : "<<errno << ", ("<<strerror(errno)<<")");
		return -1;
	}

	return 1;
}

int Camera::setSharpness(int v)
{
	if(v<mh || v>Mh) return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_SHARPNESS;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		LOGGER_ERROR ("Error setting sharpness error : "<<errno << ", ("<<strerror(errno)<<")");
		return -1;
	}

	return 1;
}

