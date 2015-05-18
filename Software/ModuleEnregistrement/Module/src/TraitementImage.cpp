#include "Config.h"

#ifdef DEBUG_IMAGE
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

#include "TraitementImage.h"
bool RefOk = false;

Mat frameRef;
Mat diff;

using namespace std;

void track(Mat * frame)
{
	if(!RefOk)
	{
		RefOk = true;

		frameRef.clone(*frame);
		diff.clone(*frame);

	}
	else
	{
		// Taille de l'image
		unsigned int sizeImage = frameRef.cols*frameRef.rows;
		unsigned int widthImage = frameRef.cols;
		//unsigned int sizePixels = sizeImage *frameRef.channels ;

		// pointeurs vers les données de l'image
		unsigned char *pixReference = (unsigned char*)(frameRef.data);
		unsigned char *pixFrame = (unsigned char*)(frame->data);
		unsigned char *pixOutput = (unsigned char*)(diff.data);

		// Utilisé pour les calculs
		unsigned int tmpData;
		unsigned int indexR=0,indexG=0,indexB=widthImage * 3;
		unsigned int indexPixA=0,indexPixB=0;
		unsigned int matEtiquette[sizeImage];
		unsigned int corrEtiquette[10000]; //alouer de la place pour les etiquettes


		unsigned int currentEtiquette = 0;
		unsigned char valPixA=0,valPixB=0,valPixC=0;

		memset(matEtiquette,0,widthImage * sizeof(unsigned int));
		memset(corrEtiquette,0,10000 * sizeof(unsigned int));
		memset(pixOutput,0,widthImage*3);

		corrEtiquette[0] = 0;

		for( unsigned int i = widthImage; i < sizeImage; i++,indexB += 3)
		{

			// On est en BGR

			//indexB = i+i+i;

			indexG = indexB+1;

			indexR = indexG+1;

			if( (i%widthImage) == 0)
			{
				pixOutput[indexR] = 0;
				pixOutput[indexG] = 0;
				pixOutput[indexB] = 0;
				matEtiquette[i] = 0;
				continue;
			}

			// Soustraction des images
			pixOutput[indexB] = max(0,((int)pixReference[indexB] - (int)pixFrame[indexB]));
			pixOutput[indexR] = max(0,((int)pixReference[indexR] - (int)pixFrame[indexR]));
			pixOutput[indexG] = max(0,((int)pixReference[indexG] - (int)pixFrame[indexG]));



			// conversion de RGB vers Niveau De Gris
			tmpData = (pixOutput[indexR]*77)+(pixOutput[indexG]*151)+(pixOutput[indexB]*28);
			tmpData = (tmpData>>8);


			// Seuillage
			if(tmpData < 70)
				tmpData = 0;
			else
				tmpData = 255;

			pixOutput[indexR] = tmpData;
			pixOutput[indexG] = tmpData;
			pixOutput[indexB] = tmpData;


			if(tmpData == 255) // Pixel courant allumé
			{

				// Si on est pas sur la pemiere ligne et la premiere collonne
				//if(i > widthImage && (i%widthImage) > 0)
				{

					/*_B
					  AC */

					indexPixA = i - 1;
					indexPixB = i - widthImage;

					valPixA = pixOutput[indexPixA*3];
					valPixB = pixOutput[indexPixB*3];
					valPixC = pixOutput[i*3];



					/*si pix(c)=pix(a) et pix(c)!=pix(b)
					  ->e(c)=e(a)*/

					if(valPixC == valPixA && valPixC != valPixB)
					{
						matEtiquette[i] = matEtiquette[indexPixA];
					}

					/*si pix(c)=pix(b) et pix(c)!=pix(a)
					  ->e(c)=e(b)*/
					else if(valPixC == valPixB && valPixC != valPixA)
					{
						matEtiquette[i] = matEtiquette[indexPixB];
					}

					/*si pix(c)!=pix(b) et pix(c) !=pix(a)
					  ->e(c)=new*/
					else if(valPixC != valPixB && valPixC != valPixA)
					{
						++currentEtiquette;
						corrEtiquette[currentEtiquette] = currentEtiquette;
						matEtiquette[i] = currentEtiquette;
					}

					/*si pix(c)=pix(b) et pix(c)=pix(a) et e(a)=e(b)
					  ->e(c)=e(a)*/
					else if(valPixC == valPixB && valPixC == valPixA && matEtiquette[indexPixB] == matEtiquette[indexPixA])
					{
						matEtiquette[i] = matEtiquette[indexPixA];
					}

					else /*if(valPixC == valPixB && valPixC == valPixA && matEtiquette[indexPixB] != matEtiquette[indexPixA])*/
					{
						matEtiquette[i] = min(corrEtiquette[matEtiquette[indexPixB]],matEtiquette[indexPixA]);
						corrEtiquette[ max(corrEtiquette[matEtiquette[indexPixB]],matEtiquette[indexPixA])] = matEtiquette[i];
					}

				}

			}
			else
				matEtiquette[i] = 0;
		}

		unsigned int cpt_nb_eti = 1;
		unsigned int posX,posY;

		for(unsigned int cpt_tab=1; cpt_tab <= currentEtiquette; cpt_tab++)
		{
			if(corrEtiquette[cpt_tab]==cpt_tab)
			{
				corrEtiquette[cpt_tab]=cpt_nb_eti++;
			}
			else
			{
				corrEtiquette[cpt_tab]=corrEtiquette[corrEtiquette[cpt_tab]];
			}
		}

		int indexMaxEti = -1;
		etiquettePixel tabEtiquettes[cpt_nb_eti];
		memset(tabEtiquettes,0,cpt_nb_eti*sizeof(etiquettePixel));
		for( unsigned int i = widthImage,indexB =  widthImage * 3; i < sizeImage; i++,indexB += 3)
		{
			// On est en BGR
			//indexB = i+i+i;
			indexG = indexB+1;
			indexR = indexG+1;

			tmpData = corrEtiquette[matEtiquette[i]];

			matEtiquette[i]=tmpData;
			if(tmpData != 0)
			{
				posX = i%widthImage;
				posY = i/widthImage;

				if(tabEtiquettes[tmpData].id != tmpData)
				{
					tabEtiquettes[tmpData].id = tmpData;
					tabEtiquettes[tmpData].pixelCount = 1;
					tabEtiquettes[tmpData].color =rand() & 0xFFFFFF;
					tabEtiquettes[tmpData].minX = posX;
					tabEtiquettes[tmpData].maxX = posX;
					tabEtiquettes[tmpData].minY = posY;
					tabEtiquettes[tmpData].maxY = posY;
				}
				else
				{
					tabEtiquettes[tmpData].pixelCount++;
					if(tabEtiquettes[tmpData].minX > posX)
						tabEtiquettes[tmpData].minX = posX;
					else if(tabEtiquettes[tmpData].maxX < posX)
						tabEtiquettes[tmpData].maxX = posX;

					if(tabEtiquettes[tmpData].minY > posY)
						tabEtiquettes[tmpData].minY = posY;
					else if(tabEtiquettes[tmpData].maxY < posY)
						tabEtiquettes[tmpData].maxY = posY;

					if(indexMaxEti == -1 || tabEtiquettes[tmpData].pixelCount > tabEtiquettes[indexMaxEti].pixelCount)
						indexMaxEti = tmpData;
				}

				pixOutput[indexR] = tabEtiquettes[tmpData].color & 0xFF;
				pixOutput[indexG] = (tabEtiquettes[tmpData].color >> 8)& 0xFF;
				pixOutput[indexB] = (tabEtiquettes[tmpData].color >> 16)& 0xFF;
			}
			else
			{
				pixOutput[indexR] = 0;
				pixOutput[indexG] = 0;
				pixOutput[indexB] = 0;
			}
			// Renvoi de la valeur dans l'image
		}


		if(indexMaxEti != -1)
		{
			LOGGER_VERB("Traking : Xmin :" << tabEtiquettes[indexMaxEti].minX<< " Xmax :" <<tabEtiquettes[indexMaxEti].maxX << " Ymin : " << tabEtiquettes[indexMaxEti].minY<< " Ymax : " <<tabEtiquettes[indexMaxEti].maxY);
			for( unsigned int i =0,indexB = 0; i < sizeImage; i++,indexB += 3)
			{
				posX = i%widthImage;
				posY = i/widthImage;
				if(posX > tabEtiquettes[indexMaxEti].minX && posX < tabEtiquettes[indexMaxEti].maxX &&
						posY > tabEtiquettes[indexMaxEti].minY && posY < tabEtiquettes[indexMaxEti].maxY )
				{
					//pixOutput[indexB] = 0;
					//pixOutput[indexB+1] = 0;
					//pixOutput[indexB+2] = 0;

				}
				else if((posX == tabEtiquettes[indexMaxEti].minX || posX == tabEtiquettes[indexMaxEti].maxX ) || (
							posY == tabEtiquettes[indexMaxEti].minY || posY == tabEtiquettes[indexMaxEti].maxY))
				{
					pixOutput[indexB] = 0;
					pixOutput[indexB+1] = 0;
					pixOutput[indexB+2] = 255;
				}
				else if((posX < tabEtiquettes[indexMaxEti].minX -10  || posX > tabEtiquettes[indexMaxEti].maxX +10) /* ^
																       (posY < tabEtiquettes[indexMaxEti].minY -10  || posY > tabEtiquettes[indexMaxEti].maxY +10)*/)
				{
					pixReference[indexB] =   (0.9 * pixReference[indexB]) +   ( 0.1* pixFrame[indexB]);
					pixReference[indexB+1] = (0.9 * pixReference[indexB+1]) + ( 0.1* pixFrame[indexB+1]);
					pixReference[indexB+2] = (0.9 * pixReference[indexB+2]) + ( 0.1* pixFrame[indexB+2]);
				}
			}

			/*for(unsigned int x= tabEtiquettes[indexMaxEti].minX ; x < tabEtiquettes[indexMaxEti].maxX  ; x++) {

			  for(unsigned int y= tabEtiquettes[indexMaxEti].minY ; y < tabEtiquettes[indexMaxEti].maxY ; y++) {

			  pixOutput[((y*widthImage + x)*3)] = 0;

			  pixOutput[((y*widthImage + x)*3)+1] = 0;

			  pixOutput[((y*widthImage + x)*3)+2] = 255;



			  }



			  }*/


			unsigned int centerX = (tabEtiquettes[indexMaxEti].maxX + tabEtiquettes[indexMaxEti].minX) /2;
			unsigned int centerY = (tabEtiquettes[indexMaxEti].maxY + tabEtiquettes[indexMaxEti].minY) /2;
			LOGGER_VERB("Moyenne : X" << centerX << " Y " << centerY );
			unsigned int thresholdLowX = ((frameRef.cols * 25) / 100); // 10 %
			unsigned int thresholdHighX = frameRef.cols - thresholdLowX; // 1 - 10%

#ifdef DEBUG_IMAGE
			cv::Mat image;
			image = cv::Mat(frame->rows, frame->cols, CV_8UC3 /*,&frame->data*/);
			for( unsigned int x = 0; x < frame->rows  ; x++)
			{
				for( unsigned int y = 0; y < frame->cols ; y++)
				{
					unsigned char * out = (pixOutput   + (x * frame->cols + y)*3);
					if(x == centerY || y == centerX)
					{
						*((image.data) + (x * frame->cols + y) * 3 +0) = 255;
						*((image.data) + (x * frame->cols + y) * 3 +1) = 255;
						*((image.data) + (x * frame->cols + y) * 3 +2) = 255;
					}
					else
					{
						if(*out == 0)
							*((image.data) + (x * frame->cols + y) * 3) =  *(pixFrame + (x * frame->cols + y)*3);
						else
							*((image.data) + (x * frame->cols + y) * 3) =  *out;

						if( y == thresholdLowX || y == thresholdHighX)
							*((image.data) + (x * frame->cols + y) * 3 +2 ) = 255;
						else
							*((image.data) + (x * frame->cols + y) * 3 +2 ) = *(pixOutput   + (x * frame->cols + y)*3+ 2);

						*((image.data) + (x * frame->cols + y) * 3 +1 ) = *out;
					}
				}
			}
			cv::imshow( "Display window", image );
			cv::waitKey(1);
#endif
			if(tabEtiquettes[indexMaxEti].pixelCount > 100)
			{


				if( centerX > thresholdHighX )
				{
					string command = SSTR("echo 0=+1 > /dev/servoblaster");
					system(command.c_str());
					//memset(pixReference,0,widthImage*3);
					RefOk = false;
					cout << "++++ "  <<endl;
				}
				else if(centerX < thresholdLowX)
				{
					string command = SSTR("echo 0=-1 > /dev/servoblaster");
					system(command.c_str());
					// memset(pixReference,0,widthImage*3);
					RefOk = false; // Will copy
					cout << "--- "  <<endl;
				}
			}


		}
	}

}
