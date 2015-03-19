#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/route.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>

#include "NetworkInterface.h"

#define BUFSIZE 8192 // For gateway
#define DEBUG 1

struct route_info
{
    struct in_addr dstAddr;
    struct in_addr srcAddr;
    struct in_addr gateWay;
    char ifName[IF_NAMESIZE];
};


NetworkInterface::NetworkInterface()
{
    //ctor
}

NetworkInterface::~NetworkInterface()
{
    //dtor
}

/*** Conversion functions ***/
char *NetworkInterface::convertCIDRToNetmask(int netmask_t){
    // Return char from CIDR
    char *cNetmask = (char *) malloc(sizeof(char) * 13);
    int i, netmask = netmask_t ;
    uint8_t mask ;
    
    strcpy(cNetmask,"");
    
    for( i = 0; i < 4; i++){
        if( netmask > 7){
            if( i != 3)
                sprintf(cNetmask,"%s255.",cNetmask);
            else
                sprintf(cNetmask,"%s255",cNetmask);
            netmask -=8;
        }
        else{
            mask = 0;
            while( (netmask--)>0 ){
                mask = mask>>1 | 0x80;
            }
            if( i != 3)
                sprintf(cNetmask,"%s%i.",cNetmask, mask );
            else
                sprintf(cNetmask,"%s%i",cNetmask, mask );
        }
    }
    return cNetmask;
}

int NetworkInterface::convertNetmaskToCIDR(char *netmask){
    // return CIDR
    if(strlen(netmask) != 8 )
        return 0;
    
    int i, count = 0;
    for(i= 0; i<8; i++){
        switch(netmask[i]){
            case 'f':
                count += 4;
                break;
            case 'e':
                return count +3;
            case 'c':
                return count +2;
            case '8':
                return count +1;
            default:
                return count;
        }
    }
    return count;
}

char* NetworkInterface::convertByteToIP(uint32_t address){
    char* ipAddress = (char *) malloc(sizeof(char) *30);
    
    sprintf(ipAddress,"%i.%i.%i.%i",address>>24,address>>16 & 0xFF, address>>8 & 0xFF, address%256);
    
    return ipAddress;
}

/*** Interfaces functions ***/
char* NetworkInterface::getNamePrimaryInterface(){
    int sock;
    struct ifconf ifconf;
    struct ifreq ifreq[10];
    int interfaces,i;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    // Point ifconf's ifc_buf to our array of interface ifreqs.
    ifconf.ifc_buf = (char *) ifreq;
    // Set ifconf's ifc_len to the length of our array of interface ifreqs.
    ifconf.ifc_len = sizeof ifreq;
    
    //  Populate ifconf.ifc_buf (ifreq) with a list of interface names and addresses.
    ioctl(sock, SIOCGIFCONF, &ifconf);
    
    interfaces = ifconf.ifc_len / sizeof(ifreq[0]);
    
    // Loop through the array of interfaces, printing each one's name and IP.
    for (i = 0; i < interfaces; i++) {
        if(memcmp(ifreq[i].ifr_name,"lo",2) != 0){
            close(sock);
            return ifreq[i].ifr_name;  // Return the first interface's name found
        }
    }
    close(sock);
    return NULL;
}

Interface* NetworkInterface::getInterface(){
    int sock;
    struct ifconf ifconf;
    struct ifreq ifreq[10];
    int interfaces;
    int i,j;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    // Point ifconf's ifc_buf to our array of interface ifreqs.
    ifconf.ifc_buf = (char *) ifreq;
    // Set ifconf's ifc_len to the length of our array of interface ifreqs.
    ifconf.ifc_len = sizeof ifreq;
    
    //  Populate ifconf.ifc_buf (ifreq) with a list of interface names and addresses.
    ioctl(sock, SIOCGIFCONF, &ifconf);
    
    interfaces = ifconf.ifc_len / sizeof(ifreq[0]);
    
    // Loop through the array of interfaces, printing each one's name and IP.
    for (i = 0; i < interfaces; i++) {
        if(memcmp(ifreq[i].ifr_name,"lo",2) != 0){
            
            // Initialise interface
            Interface *it = (Interface *)malloc(sizeof(Interface));
            
            // Name
            sprintf(it->name,"%s",ifreq[i].ifr_name);
            
            // DHCP
            it->dhcpEnable = isDhcp(it->name);
            
            // Ip address
            for (j = 0; j < 4; ++j)
                snprintf(it->ipAddress+j*2,INET_ADDRSTRLEN-j*2,"%02x",(unsigned char) ifreq[i].ifr_addr.sa_data[j+2]);
            
            // Mac address
            sprintf(it->macAddress,"%s",getMac(it->name));
            
            // Netmask address
            ifreq[i].ifr_addr.sa_family = AF_INET;
            strncpy(ifreq[i].ifr_name,it->name,IFNAMSIZ -1);
            ioctl(sock,SIOCGIFNETMASK,&ifreq[i]);
            for (j = 0; j < 4; ++j)
                snprintf(it->netMask+j*2,INET_ADDRSTRLEN-j*2,"%02x",(unsigned char) ifreq[i].ifr_addr.sa_data[j+2]);
            
            // Gateway
            getGatewayIp(it->gateway);
            
            close(sock);
            return it;  // Return the first interface's name found
        }
    }
    
    close(sock);
    return NULL;
}


/*** Mac and DHCP functions ***/
char *NetworkInterface::getMac(char *iface){
    // Return char of byte
    char *ret = (char *)malloc(MAC_STRING_LENGTH);
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    
    strcpy(s.ifr_name, iface);
    if (fd >= 0 && ret && 0 == ioctl(fd, SIOCGIFHWADDR, &s)){
        strcpy( ret, s.ifr_addr.sa_data);
        // for (i = 0; i < 6; ++i) // For humain format
        //  snprintf(ret+i*2,MAC_STRING_LENGTH-i*2,"%02x",(unsigned char) s.ifr_addr.sa_data[i]);
    }
    else return NULL;
    
    return ret;
}

int NetworkInterface::isDhcp(char *interfaceName){
    // Return 1 if DHCP Or 0 if static
    FILE *readFile = openFile((char *) "/etc/network/interfaces","r"); // Load content of this file
    if(!readFile)
        return -1;
    
    char buffer[256]; // Create buffer for read line
    char *ref = (char *)malloc(sizeof(buffer));
    sprintf(ref,"iface %s ",interfaceName);
    
    while(fgets(buffer,sizeof(buffer),readFile)){ // For each line
        if(memcmp(buffer,"iface ",5) == 0){
            if(memcmp(buffer,ref,strlen(ref)) == 0 ){
                if( strstr(buffer,"dhcp") || strstr(buffer,"DHCP"))
                    return 1;
                else return 0;
            }
        }
    }
    return -1;
}


/*** Update configuration functions ***/
void NetworkInterface::setDhcp(char *interfaceName ){
    updateInterface(interfaceName,1,(char *)"",(char *)"",(char *) "");
}

void NetworkInterface::setStaticIp(char *interfaceName, char *address, int netmask, char *gateway ){
    updateInterface(interfaceName,0,address,convertCIDRToNetmask(netmask), gateway);
}

void NetworkInterface::updateInterface(char *interfaceName, int dhcp, char *address,char *netmask, char *gateway){
    FILE *readFile = openFile((char *) "/etc/network/interfaces","r"); // Load content of this file
    if(!readFile)
        return;
    
    int found = 0; // Flag
    FILE *newFile = openFile((char *) "interfaces.tmp","w"); // create a new empty file
    
    char buffer[256]; // Create buffer for read line
    char *ref = (char *)malloc(sizeof(buffer));
    sprintf(ref,"iface %s ",interfaceName);
    
    
    while(fgets(buffer,sizeof(buffer),readFile)){ // For each line
        int writeLine = 1;
        
        if(memcmp(buffer,"iface ",5) == 0){
            if(memcmp(buffer,ref,strlen(ref)) == 0) { // Interface found
                
                if( dhcp == 1 ){
                    // Write the DHCP configuration
                    fputs(ref           ,newFile);
                    fputs("inet dhcp"   ,newFile);
                    fputc('\n'          ,newFile);
                    fputc('\n'          ,newFile);
                }
                else {
                    // Write the static configuration
                    fputs(ref          ,newFile);
                    fputs("inet static",newFile);
                    fputc('\n'         ,newFile);
                    fputs("address "   ,newFile);
                    fputs(address      ,newFile);
                    fputc('\n'         ,newFile);
                    fputs("netmask "   ,newFile);
                    fputs(netmask      ,newFile);
                    fputc('\n'         ,newFile);
                    
                    if(memcmp(gateway,"0.0.0.0",7) != 0){
                        fputs("gateway "   ,newFile);
                        fputs(gateway      ,newFile);
                        fputc('\n'         ,newFile);
                    }
                    fputc('\n'         ,newFile);
                }
                found = 1;
                writeLine = 0;
            }
            else found = 0;
        }
        
        if(found == 1){ // Delete lines unused
            if(strstr(buffer,"address") || strstr(buffer,"netmask") || strstr(buffer,"gateway"))
                writeLine = 0;
        }
        
        if(writeLine == 1) { // Write read line to new file
            fputs(buffer,newFile);
        }
    }
    
    fclose (readFile);
    fclose (newFile);
    
    int result = rename("interfaces.tmp","/etc/network/interfaces");
#ifdef DEBUG
    if( result !=0 ){
        printf("Can't remplace /etc/network/interces (root acces needed !!)\n");
        fflush(stdout);
    }
#endif
    remove("interface.tmp");
}


/*** Gateway functions ***/
int NetworkInterface::getGatewayIp(char *gatewayip){
    int found_gatewayip = 0;
    
    struct nlmsghdr *nlMsg;
    struct rtmsg *rtMsg;
    struct route_info route_info;
    char msgBuf[BUFSIZE]; // pretty large buffer
    
    int sock, len, msgSeq = 0, j;
    
    /* Create Socket */
    if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
        return(-1);
    }
    
    /* Initialize the buffer */
    memset(msgBuf, 0, sizeof(msgBuf));
    for( j = 0; j <4; j++) gatewayip[j] = 0x00;
    
    /* point the header and the msg structure pointers into the buffer */
    nlMsg = (struct nlmsghdr *)msgBuf;
    rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);
    
    /* Fill in the nlmsg header*/
    nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
    nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
    
    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
    nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.
    
    /* Send the request */
    if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0)
    {
        return -1;
    }
    
    /* Read the response */
    if((len = readNlSock(sock, msgBuf, sizeof(msgBuf), msgSeq, getpid())) < 0)
    {
        return -1;
    }
    
    /* Parse and print the response */
    for(; NLMSG_OK(nlMsg,len); nlMsg = NLMSG_NEXT(nlMsg,len))
    {
        memset(&route_info, 0, sizeof(route_info));
        if ( parseRoutes(nlMsg, &route_info) < 0 )
            continue;  // don't check route_info if it has not been set up
        
        // Check if default gateway
        if (strstr((char *)inet_ntoa(route_info.dstAddr), "0.0.0.0"))
        {
            //for (j = 0; j < 4; ++j) // For human display
            //      snprintf(gatewayip+j*2,INET_ADDRSTRLEN-j*2,"%02x",(unsigned char) (route_info.gateWay.s_addr>>8*j)%256);
            gatewayip[0] = route_info.gateWay.s_addr     & 0xFF;
            gatewayip[1] = route_info.gateWay.s_addr>>8  & 0xFF;
            gatewayip[2] = route_info.gateWay.s_addr>>16 & 0xFF;
            gatewayip[3] = route_info.gateWay.s_addr>>24;
            found_gatewayip = 1;
            break;
        }
    }
    
    close(sock);
    
    return found_gatewayip;
}

int NetworkInterface::readNlSock(int sockFd, char *bufPtr, size_t buf_size, int seqNum, int pId){
    struct nlmsghdr *nlHdr;
    int readLen = 0, msgLen = 0;
    
    do
    {
        /* Recieve response from the kernel */
        if((readLen = recv(sockFd, bufPtr, buf_size - msgLen, 0)) < 0)
        {
            perror("SOCK READ: ");
            return -1;
        }
        
        nlHdr = (struct nlmsghdr *)bufPtr;
        
        /* Check if the header is valid */
        if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
        {
            perror("Error in recieved packet");
            return -1;
        }
        
        /* Check if the its the last message */
        if(nlHdr->nlmsg_type == NLMSG_DONE)
        {
            break;
        }
        else
        {
            /* Else move the pointer to buffer appropriately */
            bufPtr += readLen;
            msgLen += readLen;
        }
        
        /* Check if its a multi part message */
        if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
        {
            /* return if its not */
            break;
        }
    }
    while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
    
    return msgLen;
}

int NetworkInterface::parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo){
    /* parse the route info returned */
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
    int rtLen;
    
    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
    
    /* If the route is not for AF_INET or does not belong to main routing table then return. */
    if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
        return -1;
    
    /* get the rtattr field */
    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);
    
    for(; RTA_OK(rtAttr,rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen))
    {
        switch(rtAttr->rta_type)
        {
            case RTA_OIF:
                if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
                break;
                
            case RTA_GATEWAY:
                memcpy(&rtInfo->gateWay, RTA_DATA(rtAttr), sizeof(rtInfo->gateWay));
                break;
                
            case RTA_PREFSRC:
                memcpy(&rtInfo->srcAddr, RTA_DATA(rtAttr), sizeof(rtInfo->srcAddr));
                break;
                
            case RTA_DST:
                memcpy(&rtInfo->dstAddr, RTA_DATA(rtAttr), sizeof(rtInfo->dstAddr));
                break;
        }
    }
    
    return 0;
}


/*** Others functions ***/
FILE* NetworkInterface::openFile(char *interfaceName, const char *typ){
    // return content of file
    if(!interfaceName)
        return NULL;
    
    FILE *pInterface = fopen(interfaceName,typ); // Open file in reading
    if(!pInterface){
#ifdef DEBUG
        printf("openFile: File not found %s\n",interfaceName);
#endif
        return NULL;
    }
    
    return pInterface;
}
