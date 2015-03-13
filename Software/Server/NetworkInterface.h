#ifndef NetworkInterface_H
#define NetworkInterface_H

#define MAC_STRING_LENGTH 13

#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

struct Interface {
    char   name[IFNAMSIZ];
    char   macAddress[MAC_STRING_LENGTH];
    char   ipAddress[10];
    char   netMask[10];
    char   gateway[10];
    int    dhcpEnable;
};

class NetworkInterface
{
public:
    NetworkInterface();
    virtual ~NetworkInterface();
    Interface*  getInterface();
    
    int         convertNetmaskToCIDR(char *netmask);
    char*       convertCIDRToNetmask(int netmask);
    char*		convertByteToIP(uint32_t address);
    
    char*       getMac(char *iface);
    char*       getNamePrimaryInterface();
    int 		getGatewayIp(char *gatewayip);
    
    
    void        setDhcp(char *interfaceName );
    void        setStaticIp(char *interfaceName,char *address, int netmask, char *gateway );
    
    void        restartConfiguration() { system("ifup eth0; /etc/init.d/networking reload;");};
    
protected:
private:
    int         isDhcp(char *interfaceName);
    
    void        updateInterface(char *interfaceName, int dhcp, char *address,char *netmask, char *gateway);
    
    int 		readNlSock(int sockFd, char *bufPtr, size_t buf_size, int seqNum, int pId);
    int 		parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo);
    
    FILE*       openFile(char *interfaceName, const char* typ);
    
    
};

#endif // NetworkInterface_H
