BEGIN{ 
     } 
     {
        if( NR ==  4 ){ SOURCE_NAME=$2;  }
        if( NR ==  6 ){ SOURCE_PID=$2;   }
        if( NR ==  9 ){ RECEIVER_NAME=$2;}
        if( NR == 11 ){ RECEIVER_PID=$2; }
        if( NR == 19 ){ DATA_RECV=$2;    }
        if( NR == 38 ){ DATA_SENT=$2;    }
        if( NR == 40 ){ NACK_SENT=$2;    }
        if( NR == 21 ){ NACK_RECV=$2;    }
        if( NR == 42 ){ REF_SENT=$2;     }
        if( NR == 23 ){ REF_RECV=$2;     }
        if( NR == 44 ){ RET_SENT=$2;     }
        if( NR == 25 ){ RET_RECV=$2;     }
     }
  END{
        printf("\n--------------------------------------------------\n") >> output_file;
        printf("%s:%d losses from %s:%d\n",RECEIVER_NAME,RECEIVER_PID,SOURCE_NAME,SOURCE_PID) >> output_file;
        printf("--------------------------------------------------\n") >> output_file;
        
        if( (DATA_SENT > 0) && (DATA_RECV < DATA_SENT) )
            printf( "Data loss= %.4f\n", 1 - ( DATA_RECV/DATA_SENT ) ) >> output_file;
        else
        {
            printf( "Data loss= 0\n") >> output_file;
            DATA_RECV=DATA_SENT;
        }
            
        if( (NACK_SENT > 0) && (NACK_RECV < NACK_SENT) )
            printf( "NACK loss= %.4f\n", 1 - ( NACK_RECV/NACK_SENT ) ) >> output_file;
        else
        {
            printf( "NACK loss= 0\n" ) >> output_file;
            NACK_RECV=NACK_SENT;
        }
        
        if( (REF_SENT > 0) && (REF_RECV < REF_SENT) )
            printf( "Refresh loss= %.4f\n", 1 - ( REF_RECV/REF_SENT ) ) >> output_file;
        else
        {
            printf( "Refresh loss= 0\n" ) >> output_file;
            REF_RECV=REF_SENT;
        }
            
        if( (RET_SENT > 0) && (RET_RECV < RET_SENT) )
            printf( "Retransmission loss= %.4f\n", 1 - ( RET_RECV/RET_SENT ) ) >> output_file;
        else
        {
            printf( "Retransmission loss= 0\n" ) >> output_file;
            RET_RECV=RET_SENT;
        }
        
        packets_sent_sum=DATA_SENT + NACK_SENT + REF_SENT + RET_SENT;
        if( packets_sent_sum > 0 )
        {
            packets_recv_sum=DATA_RECV + NACK_RECV + REF_RECV + RET_RECV;
            printf("Total packet loss= %.4f\n", 1 - (packets_recv_sum/packets_sent_sum) ) >> output_file;
        }
            
        printf("--------------------------------------------------\n") >> output_file;
     }

