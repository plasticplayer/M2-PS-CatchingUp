#ifndef __Recording__
#define __Recording__

#include <list>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>

class Recording;

typedef unsigned char BYTE ;
class RecordingFile {
public:
	string fileName;
	string path;
	string idRecording;
	Recording *recording;
	string chks;
	uint64_t idFile;
	bool isInRecord;
	bool isInUpload;
	bool isUploaded;
	bool generateChksum();
	void uploadOk();
};

class Recording {
public:
   	Recording( uint64_t idRecording );
	bool startRecord();
	void stopRecord();
	bool isRecording();
	void addFile ( RecordingFile *f );
	static RecordingFile* getNextFile();
	static bool loadRecordings();
	static uint64_t _FilesNotUpload;
	void makeDirectory();
	string _folderRecording;	
	list<RecordingFile *> _Files;	
	static list<Recording *> _Recordings;
	uint64_t _IdRecording;
protected:


private:
	static void loadRecordingFolder( string folder, string fold );
	bool _isRecording;
	bool _isFinished;
	bool _isUploading;
};

#endif /* defined(__server__Tcp__) */
