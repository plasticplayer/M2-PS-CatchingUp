#include "../header/Config.h"
#include "../header/logger.h"
#include "../header/Recording.h"

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>

using namespace std;

unsigned long Recording::_FilesNotUpload = 0;
list<Recording *> Recording::_Recordings;


Recording::Recording( uint64_t idRecording ){
	_IdRecording = idRecording;
	_isRecording = false;
	_isUploading = false;
	_FileNotUploadForRecord = 0;
	_Recordings.push_back(this);
}

bool Recording::startRecord(){
	_isRecording = true;
	return true;
}

void Recording::stopRecord(){
	_isRecording = false;
}

unsigned long Recording::countUnsendingFiles(){
	return _FileNotUploadForRecord;
}

/// Load records from configurationFile
bool Recording::loadRecordings(){
	return false;
}
/// Export records from configurationFile
bool Recording::exportRecordings(){
	return false;
}
