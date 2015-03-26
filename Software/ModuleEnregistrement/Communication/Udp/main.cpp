#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "Udp.h"

#define BUFFSIZE 255

void Die(char *mess) { perror(mess); exit(1); }


using namespace std;


#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



int main(int argc, char *argv[]) {
	
	Udp *udp = new Udp( 1902 );

	InfoTCP *info = udp->getInfoSrv( 1902 );
	
	if ( info != NULL ){
		cout << "Info SRV: " << info->ipAddress << ":" << info->port << endl;
	}
	else cout << "Nothing :(" << endl;

}
