#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "rmstruct.e"
#include "rminternals.e"
#include "rmmsgpckt.e"

FILE *logfile;

void set(int *dest, int dat3, int dat4);
void showLogMsg(char type, PACKET_INFO *pckt_info);

struct timeval time_of_day;

int main()
{
	PACKET_INFO pckt_info;	 
    int data[1000];
    int cont,i;
    char line[180];
    char char_trash1[100], char_trash2[100], char_trash3[100], char_trash4[100];
    int dat1,dat2,dat3,dat4,dat5,dat6,dat7,dat8, trash1, trash2, trash3, trash4;
    char *aux;
	 
	int hour, min, sec, msec;
	 
    if (fgets(line,180,stdin)==NULL)
        exit(1);
        
    while (line!=NULL)
    {
    
        if (line[0] != '\t')
        {
		        sscanf(line,"%d:%d:%d.%d %s %s %s %s", &hour, &min, &sec, &msec,
                 char_trash1,char_trash2,char_trash3,char_trash4);
                
                aux = line;
                
                while ((*aux)!=':')          aux++;     
                aux++;                    
                while ((*aux)!=':')          aux++;       
                aux++;                    
                while ((*aux)!=':')          aux++;       
                aux++; 
                
                aux++;                
                                    
                if ( *aux == 'i' )
                {
                 puts(line);
                 fgets(line,180,stdin);                  
                 while (line[0]=='\t')
                    fgets(line,180,stdin); 
                 continue;
                }

			    time_of_day.tv_sec  = (hour*60+min)*60+sec;

			    time_of_day.tv_usec = msec;				

            if (fgets(line,180,stdin)==NULL)
            {
                break;            
            }

        }
        else
        {
            for (i=0; i<1; i++)
            {
                if (fgets(line,180,stdin)==NULL)
                {
                    break;
                }
            }
              
            
            memset(data,0,1000);
            
            cont = 0;
            sscanf(line,"%x %x %x %x %x %x %x %x",   &trash1,
                                                 &trash2,
                                                 &trash3,
                                                 &trash4,
                                                 &dat1, &dat2,
                                                 &dat3, &dat4);
				
			set(&(data[cont]), dat3, dat4);

            cont++;

            if (fgets(line,180,stdin)==NULL)
                break; 
                               
            puts(line);
                
            while (line[0]=='\t')
            {
                sscanf(line,"%x %x %x %x %x %x %x %x", &dat1,
                                                     &dat2,
                                                     &dat3,
                                                     &dat4,
                                                     &dat5, &dat6,
                                                     &dat7, &dat8);
					 
					 set(&(data[cont]), dat1, dat2);					cont ++;
					 set(&(data[cont]), dat3, dat4);					cont ++;
					 set(&(data[cont]), dat5, dat6);					cont ++;
					 set(&(data[cont]), dat7, dat8);					cont ++;					 

                if (fgets(line,180,stdin)==NULL)
                    break;                
            }
            
        //    msgPcktShowMessage((BYTE *)data);              

			msgPcktUnmountMessage(&pckt_info, (BYTE *)data);				
			showLogMsg('R', &pckt_info);
            
        }
    }
	 return 1;
}

void set(int *dest, int dat3, int dat4)
{
	int aux;
	aux = ((dat4 << 16) | dat3) ;
	memcpy(dest, &aux, 4);             
    swab(dest,dest,4);
}

void showLogMsg(char type, PACKET_INFO *pckt_info)
{
    int sn=0;
    char member_id[100];
    char pid[6];
    char last_sn[16];
    
//  gettimeofday(&time_of_day, 0);    

    if (pckt_info !=NULL)
    {
        switch (pckt_info->type)
        {
            case DATA_PACKET_TYPE:

                 sn = pckt_info->packet_data.data_packet.sn; 

                 strcpy(member_id,pckt_info->sender_id.ip);
                 strcat(member_id,"\t");
                 sprintf(pid,"%d",pckt_info->sender_id.pid);
                 strcat(member_id,pid);

                 break;

            case NAK_PACKET_TYPE:

                 sn = pckt_info->packet_data.nak_packet.sn; 

    //           sprintf(last_sn," -> %d",pckt_info->packet_data.nak_packet.last_sn);

                 strcpy(member_id,pckt_info->packet_data.nak_packet.requested_member_id.ip);
                 strcat(member_id,"\t");
                 sprintf(pid,"%d",pckt_info->packet_data.nak_packet.requested_member_id.pid);
                 strcat(member_id,pid);

                 break;

            case RETRANSM_PACKET_TYPE:

                 sn = pckt_info->packet_data.retransm_packet.data_packet.sn; 

                 strcpy(member_id,pckt_info->packet_data.retransm_packet.original_sender_id.ip);
                 strcat(member_id,"\t");
                 sprintf(pid,"%d",pckt_info->packet_data.retransm_packet.original_sender_id.pid);
                 strcat(member_id,pid);

                 break;                

    #ifdef REFRESH        

            case REFRESH_PACKET_TYPE:

                 sn = pckt_info->packet_data.refresh_packet.sn_of_last_msg_sent; 

                 strcpy(member_id,pckt_info->sender_id.ip);
                 strcat(member_id,"\t");
                 sprintf(pid,"%d",pckt_info->sender_id.pid);
                 strcat(member_id,pid);

                 break;
    #endif

            default:
                 fprintf(stderr,"Unknown packet type: %c\n",pckt_info->type);                     
                 
                 break;
        }

		  if((pckt_info->type <= 3)||(pckt_info->type == 8))
		  {
		  	fprintf(stderr,"%.0Lf     %c    %s %20s\t%3d%s\n", 
            ( (((long double)time_of_day.tv_sec)*1000000+(long double)time_of_day.tv_usec) ) ,
            type,
            MESSAGE_TYPE_NAME[(int)pckt_info->type],
            member_id, sn, 
            ((type == NAK_PACKET_TYPE) ? last_sn:""));
		  }
    }
    else
    {
       fprintf(logfile,"%.0Lf     %c\n", 
                ( (((long double)time_of_day.tv_sec)*1000000+(long double)time_of_day.tv_usec) ) ,
                type);
    }
        
            
}



