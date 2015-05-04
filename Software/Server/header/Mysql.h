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
		
		// Administration
		static uint64_t createUserRecorder ( string firstName, string lastName, string password, string email , string dateBegin, string dateEnd );
		static uint64_t createRecorder ( uint64_t idRoom, string addressMac );
		static uint64_t createRoom ( string roomName, string description );
		static uint64_t createCard ( uint64_t cardNumber, uint64_t idUser );
		static bool generateNrfAddress ( uint64_t recorderId, uint64_t *rec, uint64_t *act );
		// 
		static void stopRecording ( uint64_t idRecording );	
		static uint64_t createRecording ( uint64_t idRecorder , uint64_t idUserRecorder);
		static uint64_t getIdRecorderFromMac( string mac );
		static uint64_t getIdUserRecorderFromTag( uint64_t idCard );
	
	protected:
		bool _IsInsertingRow;
	private:
		Connection *_Connection;
		static uint64_t getIdFromTagNumber ( uint64_t cardNumber );
		static Mysql *_DataBase;
		//pthread_t _Listenner, _Sender;
};
#endif /* defined( __MYSQL__ ) */
