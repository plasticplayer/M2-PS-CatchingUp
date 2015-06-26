#include <iostream> // for standard I/O
#include <string>   // for strings
#include <list>
#include <dirent.h>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace std;
using namespace cv;

VideoWriter outputVideo; // For writing the video

//int width = 1920; // Declare width here
//int height = 1080; // Declare height here
//Size S = Size(width, height); // Declare Size structure

// Open up the video for writing
const string filename = "toto.avi"; // Declare name of file here

// Declare FourCC code
int fourcc = CV_FOURCC('M','J','P','G');

// Declare FPS here
int fps = 25;

DIR *dpdf;
struct dirent *epdf;


class itemRecording
{
	public:
		typedef enum _TypeFile
		{
			VIDEO_TRACKING, 
			PHOTO_BOARD, 
			VIDEO_BOARD
		}TypeFile;
		typedef enum _StatusFile
		{
			TRANSFER,
			WAITINGTREATMENT, 
			TREATMENT, 
			TREATED
		}StatusFile;

		int idFile;
		int idRecording;
		TypeFile type;
		string fileName;
		StatusFile status;

		itemRecording(int _idFile,int _idRecording,TypeFile _type,string _fileName,StatusFile _status)
			:idFile(_idFile),idRecording(_idRecording),type(_type),fileName(_fileName),status(_status)
		{
		}
};
list<itemRecording> listItems;

bool sortFileNames (const itemRecording& first, const itemRecording& second)
{
	string fnFirst  = first.fileName;
	string fnSecond = second.fileName;

	size_t posSlash1 = fnFirst.find_last_of("/");
	if(posSlash1 != string::npos)
		fnFirst = fnFirst.substr(posSlash1);

	size_t posSlash2 = fnSecond.find_last_of("/");
	if(posSlash2 != string::npos)
		fnSecond = fnSecond.substr(posSlash2);

	size_t posPoint1 = fnFirst.find_last_of(".");
	if(posPoint1 != string::npos)
		fnFirst = fnFirst.substr(0,posPoint1);

	size_t posPoint2 = fnSecond.find_last_of(".");
	if(posPoint2 != string::npos)
		fnSecond = fnSecond.substr(0,posPoint2);

	unsigned int num1 = 0;
	unsigned int num2 = 0;
	for(unsigned int i = 0 ; i < fnFirst.length(); i++)
	{
		char charValue = fnFirst.at(i);
		if(charValue >= '0' && charValue <= '9')
		{
			num1 += (charValue - '0');
			num1 *= 10;
		}
	}

	for(unsigned int i = 0 ; i < fnSecond.length(); i++)
	{
		char charValue = fnSecond.at(i);
		if(charValue >= '0' && charValue <= '9')
		{
			num2 += (charValue - '0');
			num2 *= 10;
		}
	}
	return ( num1 < num2 );
}




int main()
{
	listItems.push_back(itemRecording(122, 24, itemRecording::PHOTO_BOARD, "/home/CatchingUp/Media/20/snap8.jpeg",itemRecording::WAITINGTREATMENT));
	listItems.push_back(itemRecording(125, 24, itemRecording::PHOTO_BOARD, "/home/CatchingUp/Media/20/snap4.jpeg",itemRecording::WAITINGTREATMENT));
	listItems.push_back(itemRecording(124, 24, itemRecording::PHOTO_BOARD, "/home/CatchingUp/Media/20/snap5.jpeg",itemRecording::WAITINGTREATMENT));
	listItems.push_back(itemRecording(127, 24, itemRecording::PHOTO_BOARD, "/home/CatchingUp/Media/20/snap3.jpeg",itemRecording::WAITINGTREATMENT));
	listItems.push_back(itemRecording(128, 24, itemRecording::PHOTO_BOARD, "/home/CatchingUp/Media/20/snap2.jpeg",itemRecording::WAITINGTREATMENT));
	listItems.push_back(itemRecording(129, 24, itemRecording::PHOTO_BOARD, "/home/CatchingUp/Media/20/snap1.jpeg",itemRecording::WAITINGTREATMENT));

	listItems.sort(sortFileNames);


	Mat image;
	for (list<itemRecording>::iterator it = listItems.begin(); it != listItems.end(); it++)
	{
		std::cout << (*it).fileName << endl;
		image = imread((*it).fileName);
		if(!outputVideo.isOpened())
			outputVideo.open(filename, fourcc, fps, image.size());
		for(int im = 0;im < fps ; im++)
			outputVideo << image;
	}
	//	outputVideo << image;
	return 0;

}
