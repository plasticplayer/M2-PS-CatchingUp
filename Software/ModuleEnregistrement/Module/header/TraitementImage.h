#ifndef __TRAITEMENT_IMAGE_H__
#define __TRAITEMENT_IMAGE_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct etiquettePixel
{
	unsigned int id;
	unsigned int pixelCount;
	unsigned int color;
	unsigned int minX;
	unsigned int maxX;
	unsigned int minY;
	unsigned int maxY;
}etiquettePixel;

class Mat
{
public :
	Mat()
	{
		this->cols = 0;
		this->rows = 0;
		this->channels = 0;
		this->data = NULL;
	}

	Mat(unsigned int _cols,unsigned int _rows, unsigned char _channels)
	{
		this->cols = _cols;
		this->rows = _rows;
		this->channels = _channels;
		this->data = new unsigned char[cols*rows*channels];
	}
	~Mat()
	{
		if(data != NULL)
		delete[] data;
	}
	void clone(Mat& src)
	{
		if(data != NULL)
			delete[] data;
		this->cols = src.cols;
		this->rows = src.rows;
		this->channels = src.channels;
		this->data = new unsigned char[cols*rows*channels];
		printf("Clonning Matrix\n");
		memcpy(this->data,src.data,(cols * rows * channels));
	}
/*	static Mat clone(Mat& src)
	{
		Mat m();
		m.clone(src);
		return m; 
	}*/
	unsigned int cols;
	unsigned int rows;
	unsigned char channels;
	unsigned char * data;
};


bool track(Mat * frame);

#endif