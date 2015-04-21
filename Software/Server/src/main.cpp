//
//  main.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include <iostream>
#include "../header/Udp.h"
#include "../header/define.h"
#include "../header/Tcp_Socket_Server.h"

using namespace std;

int main(int argc, const char * argv[]) {
	// insert code here...
	cout << "Start Server" << endl;

	Tcp_Socket_Server* tcpSocketSrv = new Tcp_Socket_Server( TCP_PORT );
	tcpSocketSrv->start();

	Udp* udp = new Udp( 1902 );
	udp->listenner();


	return 0;
}
