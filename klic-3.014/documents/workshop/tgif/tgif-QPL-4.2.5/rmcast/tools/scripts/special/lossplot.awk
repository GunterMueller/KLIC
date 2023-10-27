BEGIN{
}
{
    if( NR ==  1 ){ DATE=$1;  }
    
    if( NR ==  4 ){ HOST1=$1;       }
    if( NR == 10 ){ PCKT_LOSS1=$4;  }
    if( NR == 15 ){ HOST2=$1;       }
    if( NR == 21 ){ PCKT_LOSS2=$4;  }    
    if( NR == 26 ){ HOST3=$1;       }
    if( NR == 32 ){ PCKT_LOSS3=$4;  }    
}
END{
    printf("%d %f\n",DATE,PCKT_LOSS1)>>(HOST1".plot");
    printf("%d %f\n",DATE,PCKT_LOSS2)>>(HOST2".plot");
    printf("%d %f\n",DATE,PCKT_LOSS3)>>(HOST3".plot");
}

