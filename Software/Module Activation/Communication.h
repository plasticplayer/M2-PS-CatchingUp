//
//  Communication.h
//  ProtocolCommunication
//
//  Created by maxime max on 28/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __ProtocolCommunication__Communication__
#define __ProtocolCommunication__Communication__

//typedef unsigned char BYTE ;
#include "Arduino.h"
#define BYTE byte

#define BUFFER_SIZE 40
//#include <stdio.h>

typedef void (*FuncType)( unsigned char data[], int size);

class Communication {

public:

  int encodeData( BYTE* dataIn, BYTE** dataOut, int sizeDataIn );
  void setFunction( void (*FuncType)( BYTE data[], int size) , int ida);


  Communication( BYTE start, BYTE stope, BYTE escape );

  void recieveData( BYTE* data, int size );

protected:
  bool _Escaped , _StartDetected;
  FuncType _PtrFunctions[256];
  BYTE _DecodeDatas[60];
  int _PosDecodeData;
  char _Start, _Stop, _Escape;
};


#endif /* defined(__ProtocolCommunication__Communication__) */

