#!/bin/bash

#Config
    RMLOG=rmlog.awk

#Functions

    usage(){

        echo 
        echo "--------------------------------------------------------------------------------------------------------"
	    echo "    Usage:"
        echo
        echo "    rmcastplot.bash <max_pckt_sent_by_source> <xyrange> <member1.log> <member2.log> [awk_script_dir] [tgif|png]"
        echo
        echo "    max_pckt_sent_by_source: amount of packets sent by the source"
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
            echo " found.">&2
        else
            echo " not found.">&2
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
            PACKETS_SENT_FROM_SOURCE=$1
            XYRANGE=$2
            LOG1=$3
            LOG2=$4

        if [ -z $5 ];then

            echo -n "awk script path not provided, trying to find it in local directory ... ">&2
            findawk

        else
            if [ "$5" = "tgif" ];then

                echo -n "awk script path not provided, trying to found it in local directory ... ">&2
                findawk

            else

                if [ "$5" = "png" ];then

                    echo -n "awk script path not provided, trying to found it in local directory ... ">&2
                    findawk

                else
                    echo -n "Awk script path provided, checking ... ">&2

                    if [ -e $5/$RMLOG ];then

                        echo "OK.">&2
                        AWKSCRIPT=$5/$RMLOG

                    else

                        echo "FAILED.">&2
                        exit 0

                    fi
                fi
            fi
        fi

        MEMBER1_NAME=`head -1 $LOG1 | awk '{if(NR==1)print $2}'`
        MEMBER1_IP=`head -2 $LOG1 | awk '{if(NR==2)print $2}'`
        MEMBER1_PID=`head -3 $LOG1 | awk '{if(NR==3)print $2}'`
        SENT_PACKETS1=`tail -1 $LOG1 | awk '{if($1 == "Sent_Packets:")print $2}'`

        echo        
        echo "Packets sent by the source= $PACKETS_SENT_FROM_SOURCE"
        echo
        echo "Member 1 Name= $MEMBER1_NAME"
        echo "Member 1 IP= $MEMBER1_IP"
        echo "Member 1 PID= $MEMBER1_PID"
        echo "Member 1 sent packets= $SENT_PACKETS1"
        echo

        MEMBER2_NAME=`head -1 $LOG2 | awk '{if(NR==1)print $2}'`
        MEMBER2_IP=`head -2 $LOG2 | awk '{if(NR==2)print $2}'`
        MEMBER2_PID=`head -3 $LOG2 | awk '{if(NR==3)print $2}'`
        SENT_PACKETS2=`tail -1 $LOG2 | awk '{if($1 == "Sent_Packets:")print $2}'`
        echo "Member 2 Name= $MEMBER2_NAME"
        echo "Member 2 IP= $MEMBER2_IP"
        echo "Member 2 PID= $MEMBER2_PID"
        echo "Member 2 sent packets= $SENT_PACKETS2"
        echo

    # Awk filtering

        awk -f $AWKSCRIPT maxpcktsource=$PACKETS_SENT_FROM_SOURCE maxpckts=$SENT_PACKETS1 f1=$LOG2 member_ip=$MEMBER1_IP member_pid=$MEMBER1_PID $LOG2
        awk -f $AWKSCRIPT maxpcktsource=$PACKETS_SENT_FROM_SOURCE maxpckts=$SENT_PACKETS2 f1=$LOG1 member_ip=$MEMBER2_IP member_pid=$MEMBER2_PID $LOG1

    #Preparing to plotting

        NAKSPLOT="$LOG1.$LOG2.naksplot"

        echo > $NAKSPLOT

        if [ "$5" = "png" ];then
            echo "set term png small color" >> $NAKSPLOT
            echo "set output \"$LOG1\_$LOG2.png\"" >> $NAKSPLOT 
        fi

        echo "plot $XYRANGE \"$LOG2.naksenviados\" title \"$MEMBER2_NAME:$MEMBER2_PID: Naks sent to multicast group\" lt 7" >> $NAKSPLOT
        if [ "$5" = "png" ];then
            echo "set output \"$LOG1\_$LOG2.png\"" >> $NAKSPLOT 
        fi
        echo "replot \"$LOG1.retenviadas\" title \"$MEMBER1_NAME:$MEMBER1_PID: Retrans. sent to multicast group\" with lines 8" >> $NAKSPLOT
        if [ "$5" = "png" ];then
            echo "set output \"$LOG1\_$LOG2.png\"" >> $NAKSPLOT 
        fi
        echo "replot \"$LOG2.retrecebidas\" title \"$MEMBER2_NAME:$MEMBER2_PID: Retrans. rcv from $MEMBER1_NAME:$MEMBER1_PID\" with lines" >> $NAKSPLOT

    #Plotting

        gnuplot -persist <  $NAKSPLOT
        
    #Removing temporary files:
    \rm $LOG1.* $LOG2.*

fi

