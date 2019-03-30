#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h> 
#include <arpa/inet.h>

int hostname_resolver(char * hostname , char* ip) 
{  struct hostent *host;     
   struct in_addr **list_of_address;     
   int i;     
   if ( (host = gethostbyname( hostname ) ) == NULL)     
   {
        herror("gethostbyname");         
        return 1;
    }

   list_of_address = (struct in_addr **) host->h_addr_list;
    
    for(i = 0; list_of_address[i] != NULL; i++)
    {  
        strcpy(ip ,inet_ntoa(*list_of_address[i]) );
        return 0;
    }

    return 1;
}

int main(int argc , char *argv[]){
    
    if(argc <2)     
    {
        printf("Please provide a hostname");         
        exit(1);
    }     
    
    char *hostname = argv[1];     
    char ip[16];     
    int flag = hostname_resolver(hostname , ip);     
    
    if(!flag)  
        printf("%s resolved to %s\n" , hostname , ip);     
    
    return 0;
} 

