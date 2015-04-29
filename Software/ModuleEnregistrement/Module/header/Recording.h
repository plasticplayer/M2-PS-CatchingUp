#ifndef __Recording__
#define __Recording__

#include <list>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>


typedef unsigned char BYTE ;
typedef struct RecordingFile { 
	char* fileName;
	int nameSize;
	// uint64_t size; ??
	uint64_t chks;
	uint64_t idFile;
	bool isInRecord;
	bool isInUpload;
	bool isUploaded;
}RecordingFile;

class Recording {
public:
   	Recording( uint64_t idRecording );
	bool startRecord();
	void stopRecord();
	unsigned long countUnsendingFiles();

	static bool loadRecordings();
	static bool exportRecordings();
protected:


private:
	static list<Recording *> _Recordings;
	
	list<RecordingFile *> _Files;
	uint64_t _IdRecording; 
	bool _isRecording;
	bool _isUploading;
	unsigned long _FileNotUploadForRecord;
	static unsigned long _FilesNotUpload;
};

#endif /* defined(__server__Tcp__) */
