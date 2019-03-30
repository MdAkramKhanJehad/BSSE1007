#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<errno.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<linux/if_ether.h>
#include<arpa/inet.h>
#include "GetIPFromHostName.h"

struct pseudo_header
{
	unsigned int source_address;
	unsigned int dest_address;
	unsigned char placeholder;
	unsigned char protocol;
	unsigned short int tcp_length;
};


unsigned short csum(unsigned short *ptr,int nbytes)
{
	unsigned long int sum;
	unsigned short oddbyte;
	unsigned short int answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((unsigned char*)&oddbyte)=*(unsigned char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;

	return(answer);
}

int equals(char addr[], char arg[]){

	int i = 0, flag = 0;
    while(addr[i] != '\0' && arg[i] != '\0')
    {

        if(addr[i] != arg[i])
        {
            flag = 1;
            break;
        }
        i++;
    }


    if(flag == 0 && addr[i] == '\0' && arg[i] == '\0')
        return 1;
    else
        return 0;


}


int main (int size, char *argv[])
{

	if(size<3){
		printf("give hostname and port\n");
		return 0;

	}
	
	char ip_value[16];
	int FLAG ;
	FLAG = host_to_ip(argv[1],ip_value);

	if(FLAG){
		printf("Wrong hostname\n");
		return 0;

	}

	int s = socket (AF_INET, SOCK_RAW, IPPROTO_TCP);

	if(s == -1)
	{

		perror("Failed to create socket");
		exit(1);
	}


	char datagram[4096] , source_ip[32] , *data , *pseudogram;


	memset(datagram, 0, 4096);


	struct iphdr *iph = (struct iphdr *) datagram;


	struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct iphdr));
	struct sockaddr_in sin;
	struct sockaddr_in source;
	struct sockaddr_in dest;
	struct pseudo_header psh;


	data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
	strcpy(data , "");



	strcpy(source_ip , "192.168.31.136");
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argv[2]));
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


	iph->check = (csum ((unsigned short *) datagram, iph->tot_len));


	tcph->source = htons (15111);
	tcph->dest = htons (atoi(argv[2]));
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

	int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
	pseudogram = malloc(psize);

	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr) + strlen(data));

	tcph->check = csum( (unsigned short*) pseudogram , psize);



	int one = 1;
	const int *val = &one;

	if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		perror("Error setting IP_HDRINCL");
		exit(0);
	}


	if (sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
	{
		perror("sendto failed");

	}

	else
	{
		printf ("Packet Send. Length : %d \n" , iph->tot_len);
	}


	struct sockaddr saddr;
	int saddr_len = sizeof (saddr);
	int i=0,flag=1;
	while(i<5){

		memset (datagram, 0, 4096);
		int rcv =recv(s,datagram,4096,0);
		if(rcv<0)
		{
			printf("error in reading recv function\n");
			return -1;
		}


		struct iphdr *ip = (struct iphdr *)(datagram );


		memset(&source, 0, sizeof(source));
		source.sin_addr.s_addr = ip->saddr;
		memset(&dest, 0, sizeof(dest));
		dest.sin_addr.s_addr = ip->daddr;


		int iphdrlen = ip->ihl*4;
		

		struct tcphdr *tcp=(struct tcphdr*)(datagram + iphdrlen);


		if((unsigned int)ip->protocol==6){


			if(equals(inet_ntoa(source.sin_addr),ip_value)){
				flag =0;

				printf("\nTCP header Flags:\n\tSYN: %d\n",tcp->syn);
				printf("\tRST: %d\n",tcp->rst);
				printf("\tPSH: %d\n",tcp->psh);
				printf("\tACK: %d\n",tcp->ack);
				printf("\tURG: %d\n",tcp->urg);
				if(tcp->rst) printf("This port is closed!\n");

				break;
			}

			if(tcp->rst) printf("This port is closed!\n");

		}

		i++;
	}

	if(flag) printf("This is filtered port\n");


	return 0;
}
