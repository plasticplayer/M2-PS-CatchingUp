#include "Config.h"
#include "logger.h"
#include "Recording.h"
#include "define.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include "hashlibpp.h"
#include "hl_md5wrapper.h"

using namespace std;

uint64_t Recording::_FilesNotUpload = 0;
list<Recording *> Recording::_Recordings;


Recording::Recording( uint64_t idRecording ){
	LOGGER_DEBUG( "Create Recording " << idRecording ) ;
	_folderRecording = SSTR(CurrentApplicationConfig.Data_path  << "/" << idRecording).c_str();
	_IdRecording = idRecording;
	_isRecording = false;
	_isUploading = false;
	_isFinished  = false;
	_Recordings.push_back(this);
}
void Recording::makeDirectory()
{
	DIR *rep = opendir(_folderRecording.c_str());
	if(rep == NULL)
	{
		LOGGER_VERB("Must create directory : " << _folderRecording);
		mkdir(_folderRecording.c_str(), 0777);
	}
}
bool Recording::startRecord(){
	if ( _isFinished )
		return false;
	_isRecording = true;
	return true;
}

void Recording::stopRecord(){
	if ( ! _isRecording )
		return;
	_isRecording = false;
	_isFinished = true;
}

/// Load records from configurationFile
bool Recording::loadRecordings(){
	DIR *rep = opendir(CurrentApplicationConfig.Data_path.c_str());
	if ( rep == NULL ){
	 	// TODO : CREATE DIR
		LOGGER_WARN ( "Dir unfound : It will be create " << CurrentApplicationConfig.Data_path.c_str() );
		return  false;
	}
	else{
		struct stat st;
		struct dirent *ent;
		while ( ( ent = readdir(rep)) != NULL ){
			if(ent->d_name[0] == '.')
				continue;
			string dirName = SSTR(CurrentApplicationConfig.Data_path << "/" << ent->d_name);
			LOGGER_VERB("Listing Dir : "<< dirName);
			if ( stat( dirName.c_str(),&st) == 0 ){
				if ( (st.st_mode & S_IFDIR) != 0 )
				{
					printf("Folder : %s\n",ent->d_name);
					loadRecordingFolder(dirName, ent->d_name );
				}
			}

		}
	}
	closedir(rep);
	return false;
}

RecordingFile* Recording::getNextFile(){
	Recording* r = NULL;
	RecordingFile *f = NULL;

	for ( std::list<Recording *>::iterator it=_Recordings.begin() ; it != _Recordings.end() ; ++it){
		r = *it ;
		for ( std::list<RecordingFile*>::iterator itt=r->_Files.begin() ; itt != r->_Files.end() ; ++itt ) {
			f = *itt;
			if ( f->isInRecord )
				continue;
			if ( !f->isUploaded ) {
				return f ;
			}
		}
	}
	return NULL;
}
bool Recording::isRecording(){
	return _isRecording;
}
void Recording::loadRecordingFolder( string folder , string fol ){
	uint64_t idRecording = strtoull(fol.c_str(), NULL, 0);
	LOGGER_INFO( fol << "::" << idRecording );

	if ( idRecording == 0 )
		return;

	Recording *r = NULL;

	DIR* rep = opendir(folder.c_str() );
	if ( rep == NULL ) {
		LOGGER_WARN("Open res failed: " << folder );
		return;
	}
	struct dirent *ent;

	while (( ent = readdir(rep)) != NULL ){
		if ( ent->d_name[0] == '.' )
			continue;
		if ( r == NULL ){
			r = new Recording( idRecording );
			r->_isFinished = true;
		}
		RecordingFile *f = new RecordingFile();
		f->idRecording = SSTR( idRecording ) ;
		f->isInRecord = false;
		f->fileName = ent->d_name;
		f->path = SSTR ( folder << "/" << ent->d_name );
		r->addFile(f);
	}
	closedir(rep);
}


/// Export records from configurationFile
void Recording::addFile(RecordingFile *f ){
	if ( f == NULL )
		return;
	f->recording = this;
	f->idRecording = SSTR(_IdRecording);
	_Files.push_back( f );
	LOGGER_VERB( "Add File " << f->fileName );
	_FilesNotUpload++;
}

bool RecordingFile::generateChksum(){
	hashwrapper *myWrapper = new md5wrapper();
        try
        {
                myWrapper->test();
        }
        catch(hlException &e)
        {
                cout << "Error 1 " << endl;
                return false;
        }

        try
        {
                chks = myWrapper->getHashFromFile( path );
        }
        catch(hlException &e)
        {
                cout << "Error 2 " << endl;
                return false;
        }
	return true;
}

void RecordingFile::uploadOk(){
	Recording::_FilesNotUpload--;
	recording->_Files.remove(this);
	remove( (char*) path.c_str() );
}
