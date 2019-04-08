#include<stdio.h>
#include<string.h>
#include <time.h>
#include <unistd.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<errno.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<linux/if_ether.h>
#include<arpa/inet.h>

#include "GetIP.h"
#include "Functions01.h"


int 	n1,n2,flg=1,closedPortCounter=0,total ,counter=0;
char 	ip_value[16];



struct pseudo_header
{
	unsigned int source_address;
	unsigned int dest_address;
	unsigned char placeholder;
	unsigned char protocol;
	unsigned short int tcp_length;
};

void parseIntFromInput(char arr[]){

    int count, sign=0, offset, f=0,i;
    
   
    for( i=0;i<strlen(arr);i++)
        if((int)arr[i]>=64 && (int)arr[i]<=122)
        {
            flg=0;
        }
  
    if (arr[0] == '-') {  
        sign = -1;
    }
    
    if (sign == -1) 
        offset = 1;
    else 
        offset = 0;
    
    for (count = offset; (count<strlen(arr)); count++) {
        
        if(arr[count]=='-') break;
        n1 = n1 * 10 + (arr[count] - 48);

    }
    
    if (sign == -1) 
        n1 = -n1;

    if(arr[count]=='-'){
      
        count++;
        for (;  count<strlen(arr); count++) {
            n2 = n2 * 10 + (arr[count] - 48);
        }


    }

}


void sendAndReceivePacket(char argv[], int portNo, char *portState[], int openNfilteredPort[]){


	struct 	sockaddr saddr;
	int 	i=0, flag=1, one = 1 , saddr_len = sizeof (saddr),SOCK , psize;
	const int  *val = &one;
	char	datagram[4096] , source_ip[32] , *data , *pseudogram;
	struct 	sockaddr_in sin,source, dest;
	struct 	pseudo_header psh;



	if(!checkIfHostnameOrIpIsCorrect(argv,ip_value))
        exit(1);

	//printf("Scan Report for %s\n",ip_value);

	SOCK = socket (AF_INET, SOCK_RAW, IPPROTO_TCP);

	if(SOCK == -1)
	{		
		perror("FAILED to create SOCKET!");
		exit(1);
	}

	
	memset(datagram, 0, 4096);

	struct iphdr *iph = (struct iphdr *) datagram;
	struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct iphdr));


	data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
	strcpy(data , "");
	

	strcpy(source_ip , "192.168.31.136");
	sin.sin_family = AF_INET;
	sin.sin_port = htons(portNo);
	sin.sin_addr.s_addr = inet_addr(ip_value);


	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr) + strlen(data);
	iph->id = htonl (54321);
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_TCP;
	iph->check = 0;
	iph->saddr = inet_addr ( source_ip );
	iph->daddr = sin.sin_addr.s_addr;


	iph->check = (CalculateCheckSum((unsigned short *) datagram, iph->tot_len));


	tcph->source = htons (15111);
	tcph->dest = htons (portNo);
	tcph->seq = 110;
	tcph->ack_seq = 110;
	tcph->doff = 5;
	tcph->fin=0;
	tcph->syn=1;
	tcph->rst=0;
	tcph->psh=0;
	tcph->ack=0;
	tcph->urg=0;
	tcph->window = htons (5840);
	tcph->check = 0;
	tcph->urg_ptr = 0;


	psh.source_address = inet_addr( source_ip );
	psh.dest_address = sin.sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(data) );

	psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
	pseudogram = malloc(psize);

	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr) + strlen(data));

	tcph->check = CalculateCheckSum( (unsigned short*) pseudogram , psize);

	

	if (setsockopt (SOCK, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		perror("Error setting IP_HDRINCL");
		exit(0);
	}


	if (sendto (SOCK, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
	{
		perror("sendto failed");
		exit(0);

	}


	while(i<5){

		

		memset (datagram, 0, 4096);
		int rcv =recv(SOCK,datagram,4096,0);
		if(rcv<0)
		{
			printf("error in reading recv function\n");
			exit(1);
		}

		struct iphdr *ip = (struct iphdr *)(datagram );


		memset(&source, 0, sizeof(source));
		source.sin_addr.s_addr = ip->saddr;
		memset(&dest, 0, sizeof(dest));
		dest.sin_addr.s_addr = ip->daddr;

		int iphdrlen = ip->ihl*4;

		struct tcphdr *tcp=(struct tcphdr*)(datagram + iphdrlen);


		if((unsigned int)ip->protocol==6){
				
				if(isEqual(inet_ntoa(source.sin_addr),ip_value)){
					
					flag =0;
					

					if(tcp->syn && tcp->ack){
						if(portNo==519)
							printf("519 ok\n");

						portState[counter] = "open";
						openNfilteredPort[counter] = portNo;
						counter++;
					
					}

					
					if(tcp->rst && tcp->ack) closedPortCounter++;
					
					break;
				
				}

		}
		
		i++;
	
	}

	if(flag){
		
		portState[counter]="filtered";
		openNfilteredPort[counter] = portNo;
		counter++;

	}
	
	

}

int main (int size, char *argv[]){


	int i, total, arr[50000];
		
	
	if(size>2)	printWelcomeNote();

	if(size==1){
		printHowToScan();
		return 0;
	}
	else if(size==2){
        
		if(!isGivenTwoCorrectInput(argv[1]))
			return 0;
	}
	else if(size==3){
		if(!isGivenThreeCorrectInput(argv[2]))
			return 0;
		//send ICPM packet();

	}
	else if(size==4){
		if(!isGivenFourCorrectInput(argv[2], argv[3]))
			return 0;

		if(isEqual(argv[3],"-A")){
			//Do the subnetting for ip;
		}
		else if(isEqual(argv[3],"-F")){

			total=35;
			initializeForFastScan(arr);

		}
		else if(isEqual(argv[3],"-D")){
		
			total = 50000;
			initializeForDefaultScan(arr);

		}
		else{
			printf("Type \"./akMap -h \"for Help\n");
			return 0;
		}

	}
	else if(size==5){
		
		if(!isGivenFiveCorrectInput(argv[2],argv[3]))
			return 0;
		
		parseIntFromInput(argv[4]);

		if(isEqual(argv[3],"-p")){
			
			if(!flg){
				
				printf("Type \"./akMap -h \"for Help\n");
				return 0;

			}
			else{	
				
				if(n2<n1){
					printf("\nYour port range %d-%d is backwards. Did you mean %d-%d?\nQUITTING!\n\n",n1,n2,n2,n1);
					return 0;
				
				}			
				else{
				
					total =n2-n1+1;
					
					for(int x=0;x<total;x++,n1++)
						arr[x]=n1;

				}
			
			}

		}
		else {

		
			total=1;
			arr[0]=n1;
			
		}


	}
	else{

		printf("Type \"./akMap -h \"for Help\n");
		return 0;

	}

	

	char *portState[total];
	int openAndFilteredPorts[total];

	
	for(i = 0; i<total ; i++)
		sendAndReceivePacket(argv[1], arr[i], portState, openAndFilteredPorts);

	PrintFinalInfo(counter, openAndFilteredPorts, portState, closedPortCounter, ip_value);

	return 0;
}


