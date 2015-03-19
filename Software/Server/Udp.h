//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __server__Udp__
#define __server__Udp__

#include <stdio.h>

typedef unsigned char BYTE ;


class Udp  {
public:
    Udp( int port );
    void listenner();
protected:
private:
    int _Port ;
};

#endif /* defined(__server__Udp__) */
