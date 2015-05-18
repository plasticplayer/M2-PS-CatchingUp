//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __MYSQL__
#define __MYSQL__

//#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include "Connection.h"

typedef unsigned char BYTE ;

using namespace std;

class Mysql  {
	public:
		Mysql( );
		bool connect(string host, string database, string user, string password );
		
		/***** Administration *****/
		
		/// Get
		static Result* getRooms();
		static Result* getCards();
		static Result* getUsers();
		static Result* getRecorders();
		static Result* getUsersRecorders();
	
		/// Create
		static uint64_t createRecorder ( uint64_t idRoom, string addressMac );
		static uint64_t createRoom ( string roomName, string description );
		static uint64_t createCard ( uint64_t cardNumber, uint64_t idUser );
		static bool generateNrfAddress ( uint64_t recorderId, uint64_t *rec, uint64_t *act );
		static uint64_t createUserWebSite ( string firstName, string lastName, string password, string email , string DateRegistration);
		static uint64_t createUserRecorder ( string firstName, string lastName, string password, string email , string dateBegin, string dateEnd );
		
		
		/// Update
		static bool updateCard ( uint64_t idCard , uint64_t idUser );
		static bool updateRecorder ( uint64_t idRecorder , uint64_t idRoom );
		static bool updateUserRecorderTable ( uint64_t Id, bool eDBegin, string dateBegin, bool eDEnd, string dateEnd );
		static bool updateRoom ( uint64_t roomId, bool enableName, string name, bool enableDescription, string description );
		static bool updateUserTable ( uint64_t Id, bool eFName, string fName, bool eLName, string lName, bool ePwd, string pwd, bool eMail, string email );
		
		
		/***** Recorders *****/
		static void stopRecording ( uint64_t idRecording );	
		static void RecordingTransferFinished ( uint64_t idRecording );	
		static uint64_t createRecording ( uint64_t idRecorder , uint64_t idUserRecorder);
		static uint64_t getIdRecorderFromMac( string mac );
		static uint64_t getIdUserRecorderFromTag( uint64_t idCard );
		static bool addFileToRecording ( uint64_t idRecording , string path, string type);
		
	protected:
		bool _IsInsertingRow;
	private:
		static uint64_t countUpdatedRows(); 
		Connection *_Connection;
		static uint64_t getIdFromTagNumber ( uint64_t cardNumber );
		static Mysql *_DataBase;
		//pthread_t _Listenner, _Sender;
};
#endif /* defined( __MYSQL__ ) */
