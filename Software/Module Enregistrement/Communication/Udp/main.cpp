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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

int getMacaa(char *iface, char *dest){
	// Copie l'adresse mac de l'interface iface dans dest
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    strcpy(s.ifr_name, iface);
    if (fd >= 0 && 0 == ioctl(fd, SIOCGIFHWADDR, &s)){
        strcpy( dest, s.ifr_addr.sa_data);
        return 1;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    char mac[6];
	memset((void*)mac,sizeof(mac),0);
    //mac[0] = (char) GET_MAC_ADDR;
	if( getMacaa((char *)"eth0",mac ) == 0 ){
        cout << "Interface not found" << endl;
        return NULL;
	}

    for ( int i = 0; i < sizeof(mac); i++ ){
        printf("%02x ",mac[i]&0xFF);
    }
    cout << endl;
    Udp *udp = new Udp();

    InfoTCP *info = udp->getInfoSrv( 1902 );
    if ( info != NULL ){
        cout << info->ipAddress << ":" << info->port << endl;
    }
    else cout << "Nothing :(" << endl;

}
