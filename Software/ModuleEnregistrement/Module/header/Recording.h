#ifndef __Recording__
#define __Recording__

#include <list>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>

typedef unsigned char BYTE ;
class RecordingFile {
public:
	string fileName;
	string path;
	string idRecording;
	// uint64_t size; ??
	uint64_t chks;
	uint64_t idFile;
	bool isInRecord;
	bool isInUpload;
	bool isUploaded;
};

class Recording {
public:
   	Recording( uint64_t idRecording );
	bool startRecord();
	void stopRecord();
	void addFile ( RecordingFile *f );
	static RecordingFile* getNextFile();
	static bool loadRecordings();
	static uint64_t _FilesNotUpload;
    string _folderRecording;
protected:


private:
	static list<Recording *> _Recordings;
	static void loadRecordingFolder( string folder, string fold );

	list<RecordingFile *> _Files;
	uint64_t _IdRecording;
	bool _isRecording;
	bool _isFinished;
	bool _isUploading;
};

#endif /* defined(__server__Tcp__) */
