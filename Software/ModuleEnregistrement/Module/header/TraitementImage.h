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
	Mat(unsigned int _cols,unsigned int _rows, unsigned char _channels,unsigned char * data)
	{
		this->cols = _cols;
		this->rows = _rows;
		this->channels = _channels;
		this->data = new unsigned char[cols*rows*channels];
		memcpy(this->data,data,(cols*rows*channels));

	}
	Mat(unsigned int _cols,unsigned int _rows, unsigned char _channels,unsigned char * dataIn,int ratio)
	{
		this->cols = _cols/ratio;
		this->rows = _rows/ratio;
		this->channels = _channels;
		this->data = new unsigned char[(cols*rows*channels)];
		unsigned int x=0,y=0;
		for ( unsigned int i = 0 ; i < ( _rows ); i+=ratio){
			for ( unsigned int j = 0; j < ( _cols ) ; j+=ratio ){
				*(this->data + (((y*cols)+x)*channels)+0) = *(dataIn +(((i*_cols)+j)*channels)+0);
				*(this->data + (((y*cols)+x)*channels)+1) = *(dataIn +(((i*_cols)+j)*channels)+1);
				*(this->data + (((y*cols)+x)*channels)+2) = *(dataIn +(((i*_cols)+j)*channels)+2);
				x++;
			}
			y++;
			x = 0;
		}

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
		memcpy(this->data,src.data,(cols * rows * channels));
	}
	void clone(Mat& src, int ratio)
	{
		if(data != NULL)
			delete[] data;
		this->cols = src.cols/ratio;
		this->rows = src.rows/ratio;
		this->channels = src.channels;
		this->data = new unsigned char[(cols*rows*channels)];
		unsigned int x=0,y=0;
		for ( unsigned int i = 0 ; i < ( src.rows ); i+=ratio){
			for ( unsigned int j = 0; j < ( src.cols ) ; j+=ratio ){
				*(this->data + (((y*cols)+x)*channels)+0) = *(src.data +(((i*src.cols)+j)*channels)+0);
				*(this->data + (((y*cols)+x)*channels)+1) = *(src.data +(((i*src.cols)+j)*channels)+1);
				*(this->data + (((y*cols)+x)*channels)+2) = *(src.data +(((i*src.cols)+j)*channels)+2);
				x++;
			}
			y++;
			x = 0;
		}
	}
	/*static Mat clone(Mat& src)
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


void initImageRefs();
void setCameraPan(unsigned char step);
void track(Mat * frame);

#endif
