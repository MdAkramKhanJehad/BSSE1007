#include<stdio.h>

struct f{
    unsigned int magicNumber;
    unsigned short int version_major;
    unsigned short int version_minor;
    int  time;
    unsigned int sigfigs;
    unsigned int snaplen;
    unsigned int network;


};


int main(void){
    FILE *file;
    
    char c, arr[8];
    int x=0, i=0,z=0,p=-1;
    struct f block;
    
    file= fopen("iit2.pcap","rb");

    unsigned char c4;
    unsigned int uint, pktlen=0,var;
    unsigned short int usint,u2,u3,ihl,protocol,u4, u5,syn=2, ack=16,test1;
    int sint;
	

    if(file!=NULL){


        fread(&uint , sizeof(unsigned int), 1, file);
        block.magicNumber=uint;

        fread(&usint , sizeof(unsigned short int), 1, file);
        block.version_major= usint;

        fread(&usint , sizeof(unsigned short int), 1, file);
        block.version_minor = usint;

        fread(&sint , sizeof(int), 1, file);
        block.time= sint;

        fread(&uint , sizeof(unsigned int), 1, file);
        block.sigfigs= uint;

        fread(&uint , sizeof(unsigned int), 1, file);
        block.snaplen = uint;

        fread(&uint , sizeof(unsigned int), 1, file);
        block.network=uint;

        printf("magic number: %x\nversion major: %x\nversion minor : %x\ntime :   %x\nsigfig: %x\nhighest length : %d \nnetwork type: %d", block.magicNumber, block.version_major, block.version_minor, block.time,
               block.sigfigs, block.snaplen, block.network);

        unsigned int l=0;
        while(!feof(file)){
          
            p++;
            printf("\nPacket #%d",(p+1));
            i=0;
            while(i<8){

                i++;
                c = fgetc(file);

            }
            z=24;
            i=0;
            
            //counting the packet  data size
            fread(&pktlen , sizeof(unsigned int), 1, file);
            printf("\n\npacket length: %d",pktlen);
            
            
            i=0;
            while(i<16){

                i++;
                c = fgetc(file);

            }
            //finding ethernet type

            c = fgetc(file);
            c4 = c;
            u4 = (int)c4;
            u4=u4<<8;
            
            c = fgetc(file);
            c4 = c;
            u5 = (int)c4;

            u4=u4|u5;
            printf("\nEthernet type: %d",u4);
            
            if(u4!=2048){
                printf("\n\nThis is not ipv4 packet!\n");
                pktlen= pktlen-14;

                i=0;
                while(i<pktlen){

                    i++;
                    c = fgetc(file);

                }

                continue;

            }

            //finding header length
            c = fgetc(file);
            c4 = c;
            ihl = (int)c4;

            ihl=ihl<<12;
            ihl=ihl>>12;
            ihl=ihl*4;
            //scale of 4
            printf("\nIH length : %d\n",ihl);
            ihl=ihl-20;
            //we have to skip u4 byte after destination ip

            i=0;
            while(i<8){

                i++;
                c = fgetc(file);

            }

            c = fgetc(file);
            c4 = c;
            protocol = (int)c4;
            printf("protocol   : %d\n", protocol);
            i=0;
            while(i<2){

                i++;
                c = fgetc(file);
            }


            printf("Source ip     : ");
            z=0;
            while(z<4){
                c = fgetc(file);
                unsigned char c3=c;
                if(z<=2) printf("%d.",(int)c3);
                else printf("%d",(int)c3);
            z++;
            }

            z=0;
            printf("\ndestination ip: ");

            while(z<4){
                c = fgetc(file);
                unsigned char c3=c;
                if(z<=2) printf("%d.",(int)c3);
                else printf("%d\n",(int)c3);
                z++;
            }

            int i=0;
            //skipping the option part of the ip header
            while(i<ihl){

                i++;
                c = fgetc(file);

            }

            if(protocol==6){

                printf("\nThis is TCP packet. \n");
                c = fgetc(file);
                c4 = c;
                u2 = (int)c4;

                c = fgetc(file);
                c4 = c;
                u3 = (int)c4;
                u2= u2 << 8;
                u3 = u3 | u2;
                printf("\nsource port: %d\n", u3);

                c = fgetc(file);
                c4 = c;
                u2 = (int)c4;

                c = fgetc(file);
                c4 = c;
                u3 = (int)c4;
                u2= u2 << 8;
                u3 = u3 | u2;
                printf("destination port: %d\n",u3);

                i=0;
                while(i<9){

                    i++;
                    c = fgetc(file);
                }

                c4 = fgetc(file);
               
		     	for(int bit=7,count=0;bit>=0;bit--, count++)
	           	{
           			int k=((int)c4)>>bit;
	           		
	           		if(k&1) arr[count]='1';
	           		else arr[count]='0';  		
	           	}
	           /*
                URG—Indicates that some urgent data has been placed.
				ACK—Indicates that acknowledgement number is valid.
				PSH—Indicates that data should be passed to the application as soon as possible.
				RST—Resets the connection.
				SYN—Synchronizes sequence numbers to initiate a connection.
				FIN—Means that the sender of the flag has finished sending data.
               */
               	printf("flags:\n\tURG: %c\n",arr[2]);
               	printf("\tACK: %c\n",arr[3]);
               	printf("\tPSH: %c\n",arr[4]);
               	printf("\tRST: %c\n",arr[5]);
               	printf("\tSYN: %c\n",arr[6]);
               	printf("\tFIN: %c\n",arr[7]);


                pktlen = pktlen-38-ihl-10;      

                i=0;
                while(i<pktlen){

                i++;
                c = fgetc(file);

                }
        
            }
            else if(protocol==17){

                printf("\nThis is UDP packet!\nNothnig will be printed.\n\n");
                pktlen = pktlen-34;                
                i=0;
                while(i<pktlen){

                i++;
                c = fgetc(file);

                }

            }
            else {
                printf("\nNeither tcp nor udp packet.\n\n");
                pktlen = pktlen-34;                
                i=0;
                while(i<pktlen){

                i++;
                c = fgetc(file);

                }

            }
            
            
        }
            
        fclose(file);
    }
    
    else 
        printf("hoy nai");
  

    return 0;
}
