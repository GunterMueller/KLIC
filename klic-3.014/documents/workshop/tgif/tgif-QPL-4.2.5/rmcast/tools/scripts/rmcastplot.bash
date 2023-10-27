#!/bin/bash

#Config
    RMLOG=rmlog.awk

#Functions

    usage(){

        echo 
        echo "--------------------------------------------------------------------------------------------------------"
	    echo "    Usage:"
        echo
        echo "    rmcastplot.bash <xyrange> <member1.log> <member2.log> [awk_script_dir] [tgif|png]"
        echo
        echo "    xyrange: [XMIN:XMAX][YMIN:YMAX] gnuplot style"
        echo "    member1.log: full path to member log"
        echo "    member2_log: full path to member log"
        echo "    awk_script_dir: optional parameter. Full path to directory where $RMLOG script is found"
        echo "    tgif or png: optional parameter. Changes gnuplot output to generate Tgif files or PNG files"
        echo "--------------------------------------------------------------------------------------------------------"
	    echo
    }

    findawk(){

        if [ -e $RMLOG ];then
            AWKSCRIPT=$RMLOG
            echo " found."
        else
            echo " not found."
            usage
            exit 0
        fi
    }


# Main

    if [ $# -eq 0 ];then

        usage
        exit 0

    else

        #Naming parameters
            XYRANGE=$1
            LOG1=$2
            LOG2=$3

        if [ -z $4 ];then

            echo -n "awk script path not provided, trying to find it in local directory ... "
            findawk

        else
            if [ "$4" = "tgif" ];then

                echo -n "awk script path not provided, trying to found it in local directory ... "
                findawk

            else

                if [ "$4" = "png" ];then

                    echo -n "awk script path not provided, trying to found it in local directory ... "
                    findawk

                else
                    echo -n "Awk script path provided, checking ... "

                    if [ -e $4/$RMLOG ];then

                        echo "OK."
                        AWKSCRIPT=$4/$RMLOG

                    else

                        echo "FAILED."
                        exit 0

                    fi
                fi
            fi
        fi

        echo
        MEMBER1_NAME=`head -1 $LOG1 | awk '{if(NR==1)print $2}'`
        MEMBER1_IP=`head -2 $LOG1 | awk '{if(NR==2)print $2}'`
        MEMBER1_PID=`head -3 $LOG1 | awk '{if(NR==3)print $2}'`
        SENT_PACKETS1=`tail -1 $LOG1 | awk '{if($1 == "Sent_Packets:")print $2}'`
        echo "Member 1 Name:  $MEMBER1_NAME"
        echo "Member 1 IP:  $MEMBER1_IP"
        echo "Member 1 PID: $MEMBER1_PID"
        echo "Member 1 sent packets: $SENT_PACKETS1"
        echo

        MEMBER2_NAME=`head -1 $LOG2 | awk '{if(NR==1)print $2}'`
        MEMBER2_IP=`head -2 $LOG2 | awk '{if(NR==2)print $2}'`
        MEMBER2_PID=`head -3 $LOG2 | awk '{if(NR==3)print $2}'`
        SENT_PACKETS2=`tail -1 $LOG2 | awk '{if($1 == "Sent_Packets:")print $2}'`
        echo "Member 2 Name:  $MEMBER2_NAME"
        echo "Member 2 IP:  $MEMBER2_IP"
        echo "Member 2 PID: $MEMBER2_PID"
        echo "Member 2 sent packets: $SENT_PACKETS2"
        echo

    # Awk filtering

        awk -f $AWKSCRIPT maxsndpackets=$SENT_PACKETS1 f1=$LOG2 member_ip=$MEMBER1_IP member_pid=$MEMBER1_PID $LOG2
        awk -f $AWKSCRIPT maxsndpackets=$SENT_PACKETS2 f1=$LOG1 member_ip=$MEMBER2_IP member_pid=$MEMBER2_PID $LOG1

    #Preparing to plotting

        DTPLOT="$LOG1.$LOG2.dtplot"
        NAKSPLOT="$LOG1.$LOG2.naksplot"
        RETPLOT="$LOG1.$LOG2.retplot"

        echo > $DTPLOT
        echo > $NAKSPLOT
        echo > $RETPLOT

        if [ "$4" = "tgif" ];then
            echo "set term tgif portrait [1,1] dashed \"Helvetica\" 14 " >> $DTPLOT
            echo "set output \"$LOG1.dtplot.obj\"" >> $DTPLOT
        fi
        if [ "$4" = "png" ];then
            echo "set term png small color" >> $DTPLOT
            echo "set output \"$LOG1.dtplot.png\"" >> $DTPLOT 
        fi
        echo "plot $XYRANGE \"$LOG1.dtenviados\" title \"$MEMBER1_NAME:$MEMBER1_PID: Data sent to multicast group\" lt 7" >> $DTPLOT 

        if [ "$4" = "png" ];then
            echo "set output \"$LOG1.dtplot.png\"" >> $DTPLOT 
        fi
        echo "replot \"$LOG2.dtrecebidos\" title \"$MEMBER2_NAME:$MEMBER2_PID: Data rcv from $MEMBER1_NAME:$MEMBER1_PID\" with lines 8" >> $DTPLOT

        if [ "$4" = "png" ];then
            echo "set output \"$LOG1.dtplot.png\"" >> $DTPLOT 
        fi

        echo "replot \"$LOG2.dtperdidos\" title \"$MEMBER2_NAME:$MEMBER2_PID: Data lost from $MEMBER1_NAME:$MEMBER1_PID\" with lines 3" >> $DTPLOT

        if [ "$4" = "tgif" ];then
            echo "set term tgif portrait [1,1] dashed \"Helvetica\" 14 " >> $NAKSPLOT
            echo "set output \"$LOG1.naksplot.obj\"" >> $NAKSPLOT
        fi
        if [ "$4" = "png" ];then
            echo "set term png small color" >> $NAKSPLOT
            echo "set output \"$LOG1.naksplot.png\"" >> $NAKSPLOT 
        fi

        echo "plot $XYRANGE \"$LOG2.naksenviados\" title \"$MEMBER2_NAME:$MEMBER2_PID: Naks sent to multicast group\" with lines 8" >> $NAKSPLOT
        if [ "$4" = "png" ];then
            echo "set output \"$LOG1.naksplot.png\"" >> $NAKSPLOT 
        fi
        echo "replot \"$LOG1.naksrecebidos\" title \"$MEMBER1_NAME:$MEMBER1_PID: Naks rcv from $MEMBER2_NAME:$MEMBER2_PID\" with lines 3" >> $NAKSPLOT
        if [ "$4" = "png" ];then
            echo "set output \"$LOG1.naksplot.png\"" >> $NAKSPLOT 
        fi
        echo "replot \"$LOG1.retenviadas\" title \"$MEMBER1_NAME:$MEMBER1_PID: Retransmission sent to multicast group\" lt 7" >> $NAKSPLOT
        if [ "$4" = "png" ];then
            echo "set output \"$LOG1.naksplot.png\"" >> $NAKSPLOT 
        fi
        echo "replot \"$LOG2.retrecebidas\" title \"$MEMBER2_NAME:$MEMBER2_PID: Retransmission rcv from $MEMBER1_NAME:$MEMBER1_PID\" with lines" >> $NAKSPLOT

        if [ "$4" = "tgif" ];then
           echo "set term tgif portrait [1,1] dashed \"Helvetica\" 14 " >> $RETPLOT
           echo "set output \"$LOG1.retplot.obj\"" >> $RETPLOT
        fi
        if [ "$4" = "png" ];then
            echo "set term png small color" >> $RETPLOT
            echo "set output \"$LOG1.retplot.png\"" >> $RETPLOT 
        fi

        echo "plot $XYRANGE \"$LOG1.retenviadas\" title \"$MEMBER1_NAME:$MEMBER1_PID: Retransmission sent to multicast group\" lt 7" >> $RETPLOT 
        if [ "$4" = "png" ];then
            echo "set output \"$LOG1.retplot.png\"" >> $RETPLOT 
        fi
        echo "replot \"$LOG2.retrecebidas\" title \"$MEMBER2_NAME:$MEMBER2_PID: Retransmission rcv from $MEMBER1_NAME:$MEMBER1_PID\" with lines 8" >> $RETPLOT
        if [ "$4" = "png" ];then
            echo "set output \"$LOG1.retplot.png\"" >> $RETPLOT 
        fi
        echo "replot \"$LOG2.retperdidas\" title \"$MEMBER2_NAME:$MEMBER2_PID: Retransmission lost from $MEMBER1_NAME:$MEMBER1_PID\" with lines 3" >> $RETPLOT

    #Plotting

        gnuplot -persist <  $DTPLOT  
        gnuplot -persist <  $NAKSPLOT
        gnuplot -persist <  $RETPLOT  
fi

