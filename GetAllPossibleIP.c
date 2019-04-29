#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <string.h> 
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


unsigned int bitshift(char addr[]){
    unsigned int final=0;
    int n1, n2, n3, n4;
    sscanf(addr, "%d.%d.%d.%d", &n1, &n2, &n3, &n4);
    
    n1 = n1<<24;
    n2 = n2<<16;
    n3 = n3<<8;
    final = final | n1;
    final = final | n2;
    final = final | n3;
    final = final | n4;

    return final;

}

void convertIpAddress(unsigned int ip, unsigned char IPaddr[15]) {
   unsigned char bytes[4];
   unsigned char temp[5];

   int i;
   for(i=0; i<4; i++) {
      bytes[i] = (ip >> i*8) & 0xFF;
   }

   sprintf(IPaddr,"%d",bytes[3]);
   strcat(IPaddr,".");
   sprintf(temp,"%d",bytes[2]);
   strcat(IPaddr,temp);
   strcat(IPaddr,".");
   sprintf(temp,"%d",bytes[1]);
   strcat(IPaddr,temp);
   strcat(IPaddr,".");
   sprintf(temp,"%d",bytes[0]);
   strcat(IPaddr,temp);
   
  // printf("hahah = %s\n", IPaddr);
   
   
}


void initializeIPArray(){
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa, *sa2;
    unsigned char *addr, *addr2,ip[16], netmask[16] , *possiIP;
    int flag=0;
    unsigned int ipaddr, net, temp;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family==AF_INET) {
            addr = inet_ntoa(((struct sockaddr_in *) ifa->ifa_addr)->sin_addr);

            if(!(strcmp(ifa->ifa_name,"lo")==0)){  
                break;
            }
        }
    }
    freeifaddrs(ifap);           
    strcpy(ip,addr);
     
    getifaddrs (&ifap);  
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family==AF_INET) {
            addr2 = inet_ntoa(((struct sockaddr_in *) ifa->ifa_netmask)->sin_addr);

            if(!(strcmp(ifa->ifa_name,"lo")==0)) 
                break;
            
        }
    }
    freeifaddrs(ifap);

    strcpy(netmask,addr2);
    
    
    ipaddr = bitshift(ip);
    net = bitshift(netmask);
    int size = (~net)-1;
    unsigned char allPossibleIp[size][15], tempIP[15];
    
    for(int i=1 ; i<(~net) ;i++){
        strcpy(tempIP, "\0");
        temp = (ipaddr & net) +i;
        convertIpAddress(temp,tempIP);
        strcpy(allPossibleIp[i-1],tempIP); 

    }

    for(int i=1 ; i<(~net) ;i++){
       
        printf("%d = %s\n",i, allPossibleIp[i-1]); 

    }


}


int main(void){
    
    initializeIPArray();
  
    return 0;
}