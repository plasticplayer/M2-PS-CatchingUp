//
//  Recorder.h
//  server
//
//  Created by maxime max on 05/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __h__configAppli__
#define __h__configAppli__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


typedef unsigned char BYTE ;
using namespace std;

class ConfigAppli{
	public:
		ConfigAppli ( );
		static bool run( );
		static void getRecorders();
	private:

	protected:
};

#endif
