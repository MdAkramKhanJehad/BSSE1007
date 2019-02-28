#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    if(argc<2){
        fprintf(stderr,"port is not provided.\n");
        exit(1);
    }

    int sockfd, newsockfd, portNo,n;
    char msg[250];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cliLen;

    sockfd = socket(AF_INET, SOCK_STREAM,0);

    if(sockfd < 0)
        error("Socket is not opened\n");
    
    bzero((char *)&serv_addr, sizeof(serv_addr));

    portNo=atoi(argv[1]);

    serv_addr.sin_family= AF_INET ;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port = htons(portNo);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
        error("binding failed");
    
    listen(sockfd, 5);
    cliLen = sizeof(cli_addr);

    newsockfd= accept(sockfd, (struct sockaddr *)&cli_addr , &cliLen);
    
    if(newsockfd<0)
        error("accept error");

    while(1){
        bzero(msg, 250);
        n = read(newsockfd, msg , 250);
        
        if(n<0) error("error on reading\n");
        
        printf("client: %s\n", msg);
        bzero(msg, 250);
        fgets(msg, 250, stdin);

        n= write(newsockfd,msg, strlen(msg));
        if(n<0) error("error on writting\n");

        int i=strncmp("bye", msg, 3);
        if(i==0) break;

    }

    close(newsockfd);
    close(sockfd);

    return 0;
}
