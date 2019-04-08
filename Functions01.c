#include<stdio.h>
#include<time.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<errno.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<linux/if_ether.h>
#include "GetIP.h"




void printWelcomeNote(){

	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	printf ("\nStarting PortScanner at : %s", asctime(timeinfo));


}


unsigned short CalculateCheckSum(unsigned short *ptr,int nbytes)
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



void printHowToScan(){
	printf("\nUsage:\n\t: ./akMap {Target Specification}  [Option]\n\n");

	printf("Target Specification:\n\tCan pass Hostname or IP address\n\tExample: www.iit.du.ac.bd, 192.254.188.83\n\n");

	printf("Techniques: \n\t-P : For ping to Specific Host\n\t-S : TCP SYN Scan\n\n");

	printf("PORT SPECIFICATION and SCAN ORDER :\n\t-p  <Port ranges> : Only scan Specific Ports\n\t\tExample : -p 10-120\n\n\t-O <Port number> : Only One specific Port\n\t\tExample : -O 80\n\n\t-F : Fast Scan fewer  ports than the Default Scan\n\t-D : Default port Scan From (1-50000)\n\t-A : Scan all the Live machines in the LAN\n\n");

}

int checkIfHostnameOrIpIsCorrect(char argv[], char ip_value[]){

    strcpy(ip_value,argv);
    if((int)ip_value[0]>=65){
		int FLAG ;
		FLAG = host_to_ip(argv,ip_value);

		if(FLAG){
			
			printf("FAILED to Resolve \"%s\"\nINVALID HOSTNAME is not ALLOWED!\n", argv);
			return 0;

		}
		else
            return 1;
	}

}

int isEqual(char addr[], char arg[]){

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

int isGivenTwoCorrectInput( char str[]){
    
	
		if(isEqual(str,"-h"))
			printHowToScan();
		
		else 
			printf("Type \"./akMap -h \"for Help\n");
			
		return 0;

}

int isGivenThreeCorrectInput(char argv2[]){
	if(!isEqual(argv2,"-P")){
		printf("Type \"./akMap -h \"for Help\n");
		return 0;
	}
	else 
		return 1;

}

int isGivenFourCorrectInput(char argv2[], char argv3[]){
	if(isEqual(argv2,"-S"))
		if(isEqual(argv3,"-A") || isEqual(argv3,"-D") || isEqual(argv3,"-F") ) 
			return 1;
		
	printf("Type \"./akMap -h \"for Help\n");
	return 0;
	
}

int isGivenFiveCorrectInput( char argv2[], char argv3[]){
	if(isEqual(argv2,"-S"))
		if(isEqual( argv3,"-O") || isEqual(argv3,"-p") ) 
			return 1;
		
	printf("Type \"./akMap -h \"for Help\n");
	return 0;

	
}

char * getServiceName(int portNo){
	if((portNo==20)|| (portNo==21))
		return "FTP";
	else if(portNo==22)
		return "SSH";
	else if(portNo==23)
		return "TELNET";
	else if(portNo==25)
		return "SMTP";
	else if(portNo==53)
		return "DNS";
	else if((portNo==67)|| (portNo==68))
		return "DHCP";
	else if(portNo==69)
		return "TFTP";
	else if(portNo==80)
		return "HTTP";
	else if(portNo==110)
		return "POP3";
	else if(portNo==123)
		return "NTP";
	else if((portNo==137) || (portNo== 138) ||( portNo == 139))
		return "NetBIOS";
	else if(portNo==143)
		return "IMAP";
	else if((portNo==161) ||(portNo== 162))
		return "SNMP";
	else if(portNo==179)
		return "BGP";
	else if(portNo==218)
		return "MPP";
	else if(portNo==220)
		return "IMAP3";
	else if(portNo==389)
		return "LDAP";
	else if(portNo==443)
		return "HTTPS";
	else if(portNo==444)
		return "SNPP";
	else if(portNo==465)
		return "SMTPs";
	else if(portNo==587)
		return "Submission";
	else if(portNo==636)
		return "LDAPS";
	else if((portNo==989 )||(portNo == 990))
		return "FTPs";
	else if(portNo==993)
		return "IMAPS";
	else if(portNo==995)
		return "POP3S";
	else if(portNo==2222)
		return "EtherNetIP-1";
	else if(portNo==3306)
		return "MySQL";
	else if(portNo==8080)
		return "HTTP-Proxy";
	else if(portNo==8443)
		return "HTTPS-alt";

	else 
		return "";

}


void initializeForFastScan(int arr[]){

	arr[0]=20;		arr[1]=21; 		arr[2]=22; 		arr[3]= 23; 	arr[4]=25; 		arr[5]=53;
	arr[6]=67; 		arr[7]=68; 		arr[8]=69; 		arr[9]= 80; 	arr[10]=110; 	arr[11]=123;
	arr[12]=137; 	arr[13]=138;	arr[14]=139; 	arr[15]= 143;	arr[16]=161; 	arr[17]=162;
	arr[18]=179; 	arr[19]=218; 	arr[20]=220; 	arr[21]= 389;	arr[22]=443; 	arr[23]=444;
	arr[24]=465; 	arr[25]=587; 	arr[26]=636; 	arr[27]= 989; 	arr[28]=990; 	arr[29]=993;
	arr[30]=995; 	arr[31]=2222; 	arr[32]=3306; 	arr[33]= 8080; 	arr[34]=8443; 

}

void initializeForDefaultScan(int arr[]){
	for(int i=0;i<50000;i++){
		arr[i]=i+1;
	}

}

void PrintFinalInfo(int count, int openAndFiltered[], char *state[], int closedCount, char ip_value[]){

	printf("Scan report for %s\n\n", ip_value);
	if(closedCount>0){
		if(closedCount==1){
			printf("%d Port is closed.\n",closedCount);

		}
		else
			printf("%d Ports are closed.\n",closedCount);
	}

	printf("\n PORT     STATE     SERVICE\n");

	for(int i=0 ; i<count ; i++){
		printf("%d    %s    %s\n",openAndFiltered[i],state[i],getServiceName(openAndFiltered[i]));
	}


} 
