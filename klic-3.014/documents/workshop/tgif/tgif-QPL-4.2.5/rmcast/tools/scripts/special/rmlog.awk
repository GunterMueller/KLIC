BEGIN{ csnddt=0; 
       crcvdt=0; 
       csndnak=0; 
       crcvnak=0; 
       csndrefresh=0; 
       crcvrefresh=0; 
       csndret=0; 
       crcvret=0;
       csimloss=0;
       cldt=0;
       clnak=0;
       clref=0;
       clret=0;
       cpackets=0;
       control=1;
     }
      
     {
	if (control>7)
	{
	    if($2=="S"){
               if ($3=="DT") {
        	  csnddt++;
        	  sdt[$6]++;
               }   
               else if ($3=="NK"){

        	   if ( NF >= 11 ){

                       base_sn=$8;
                       window_size=$9;
                       hmask=$10;
                       lmask=$11;

                       i=0;

                       if(lmask<0){
                	   lmask=(-1)*lmask;
                	   snak[base_sn + (window_size/2)]++;
                	}

                       while( int(lmask/2) > 0 ){

                	   if( (lmask%2)==1 )
                               snak[base_sn + i + 1]++;

                	   lmask=int(lmask/2);
                	   i++;
                       }
                       if( (lmask%2)==1 )
                               snak[base_sn + i + 1]++;
                       i=0;

                       if(hmask<0){
                	   hmask=(-1)*hmask;
                	   snak[base_sn + window_size]++;
                       }

                       while( int(hmask/2) > 0 ){

                	   if( (hmask%2)==1 )
                               snak[(int(window_size/2)) + base_sn + i + 1]++;

                	   hmask=int(hmask/2);
                	   i++;
                       }
                       if( (hmask%2)==1 )
                	   snak[(int(window_size/2)) + base_sn + i + 1]++;
                    }
                    else{
                	snak[$8]++;
                    }

        	   csndnak++;
               }
               else if ($3=="RF") csndrefresh++;
               else if ($3=="RT"){
                	sret[$8]++;
                	csndret++;
               }

	    }
	    else if($2=="R"){
               
	       if (($3=="DT") && ($4==member_ip) && ($5==member_pid)){
        	   crcvdt++;
        	   rdt[$6]++;
               }    
               else if (($3=="NK") && ($4==member_ip) && ($5==member_pid)){

        	   if ( NF >= 11 ){

                       base_sn=$8;
                       window_size=$9;
                       hmask=$10;
                       lmask=$11;

                       i=0;

                       if(lmask<0){
                	  lmask=(-1)*lmask;
                	  rnak[base_sn + (window_size/2)]++;
                       }

                       while( int(lmask/2) > 0 ){

                	   if( (lmask%2)==1 )
                               rnak[base_sn + i + 1]++;

                	   lmask=int(lmask/2);
                	   i++;
                       }
                       if( (lmask%2)==1 )
                               rnak[base_sn + i + 1]++;
                       i=0;

                       if(hmask<0){
                	  hmask=(-1)*hmask;
                	  rnak[base_sn + window_size]++;
                       }

                       while( int(hmask/2) > 0 ){

                	   if( (hmask%2)==1 )
                               rnak[(int(window_size/2)) + base_sn + i + 1]++;

                	   hmask=int(hmask/2);
                	   i++;
                       }
                       if( (hmask%2)==1 )
                	   rnak[(int(window_size/2)) + base_sn + i + 1]++;
        	   }
        	   else{
                       rnak[$8]++;
        	   }

        	  crcvnak++;
              }
              else if (($3=="RF") && ($4==member_ip) && ($5==member_pid)) crcvrefresh++;
              else if (($3=="RT") && ($4==member_ip) && ($5==member_pid)){
                       rret[$8]++;
                       crcvret++;
        	   }
	    }
	    else if($2=="L") { csimloss++;
        	if (($3=="DT") && ($4==member_ip) && ($5==member_pid)){ 
        	    ldt[$6]++;
        	    cldt++;             
        	}    
        	else if (($3=="NK") && ($4==member_ip) && ($5==member_pid)){
                	 lnak[$8]++;
                	 clnak++;
        	     }
        	else if (($3=="RF") && ($4==member_ip) && ($5==member_pid)){
                	 lref[$6]++;
                	 clref++;
                       }
        	else if (($3=="RT") && ($4==member_ip) && ($5==member_pid)){
                	 lret[$8]++;
                	 clret++;
        	     }
	    }        

	    cpackets++;
	}
	control++;
     }
  END{
          printf("--------------------------------------------------\n");
          printf("  Data related to\n  %s -> %s:%d\n",f1,member_ip,member_pid);
          printf("--------------------------------------------------\n");
          printf("Data sent=    %d\n",csnddt); 
          printf("Data received from %s:%d=   %d\n",member_ip,member_pid,crcvdt);
          printf("NACKs sent=     %d\n",csndnak);
          printf("NACKs received from %s:%d=    %d\n",member_ip,member_pid,crcvnak);
          printf("Refresh sent=  %d\n",csndrefresh);
          printf("Refresh received from %s:%d= %d\n",member_ip,member_pid,crcvrefresh); 
          printf("Retrans sent=  %d\n",csndret); 
          printf("Retrans received from %s:%d= %d\n",member_ip,member_pid,crcvret);
          printf("Total simulated loss=   %d\n",csimloss);
          printf("Data lost by simulation from %s:%d=    %d\n",member_ip,member_pid,cldt);
          printf("NACKs lost by simulation from %s:%d=    %d\n",member_ip,member_pid,clnak);
          printf("Refresh lost by simulation from %s:%d=    %d\n",member_ip,member_pid,clref);
          printf("Retrans lost by simulation from %s:%d=    %d\n",member_ip,member_pid,clret);
          printf("Packets identified=  %d\n",cpackets);
          
          
          naksenviados=sprintf("%s.naksenviados",f1);
          naksrecebidos=sprintf("%s.naksrecebidos",f1);
          retenviadas=sprintf("%s.retenviadas",f1);
          retrecebidas=sprintf("%s.retrecebidas",f1); 
          dtperdidos=sprintf("%s.dtperdidos",f1);
          nakperdidos=sprintf("%s.nakperdidos",f1);
          retperdidos=sprintf("%s.retperdidas",f1);
          refperdidos=sprintf("%s.refperdidos",f1);
          dtenviados=sprintf("%s.dtenviados",f1);
          dtrecebidos=sprintf("%s.dtrecebidos",f1);
                            
          print " "> naksenviados;
          print " "> retenviadas;
          print " "> naksrecebidos;
          print " "> retrecebidas;
          print " "> dtperdidos;
          print " "> nakperdidos;
          print " "> retperdidos;
          print " "> refperdidos;
          print " "> dtenviados;
          print " "> dtrecebidos;
                 
          
          for( j = 0; j < maxpcktsource; j++ )
          {
              printf("%d %d\n",j,snak[j]) >> naksenviados;
              printf("%d %d\n",j,sret[j]) >> retenviadas;
              printf("%d %d\n",j,rnak[j]) >> naksrecebidos;
              printf("%d %d\n",j,rret[j]) >> retrecebidas;
              printf("%d %d\n",j,ldt[j]) >> dtperdidos;
              printf("%d %d\n",j,lnak[j]) >> nakperdidos;
              printf("%d %d\n",j,lret[j]) >> retperdidos;
              printf("%d %d\n",j,lref[j]) >> refperdidos;
              printf("%d %d\n",j,sdt[j]) >> dtenviados;
              printf("%d %d\n",j,rdt[j]) >> dtrecebidos;

         }
         
         printf("--------------------------------------------------\n\n");
     }

