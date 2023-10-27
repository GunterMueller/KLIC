#!/bin/bash

# AWK script
    AWKSCRIPT=rmlog2.awk

#Functions

    usage(){

        echo 
        echo "--------------------------------------------------------------------------------------------------------"
	    echo "    Usage:"
        echo
        echo "    autoplot.bash <max_packets_from_source> <xyrange> <NULL|loss_info_file_extension>"
        echo
        echo "    max_packets_from_source: amount of packets sent by the source"
        echo "    xyrange: [XMIN:XMAX][YMIN:YMAX] gnuplot style"
        echo "    NULL: write loss info to the default output file (logfile1_logfile2.out)"
        echo "    loss_info_file_extension: extension for the output file where the loss information must be written"
        echo "--------------------------------------------------------------------------------------------------------"
	    echo
    }
    
   if [ $# -ne 3 ];then

        usage
        exit 0

   else
       #Naming parameters
        MAX_PACKETS_FROM_SOURCE=$1
        XYRANGE=$2
        if [ $3 != "NULL" ];then
            LOSS_INFO_FILE=$3
            if [ -z $LOSS_INFO_DIR ];then
                LOSS_INFO_DIR=`pwd`
                echo "LOSS_INFO_DIR not found, using $LOSS_INFO_DIR"
            fi
                
            for FILE1 in `ls *.log`;do
                for FILE2 in `ls *.log`;do
                    if [ "$FILE1" != "$FILE2" ];then
                        echo "--------"
                        echo "$FILE1 -> $FILE2"
                        OUTPUT="${FILE1}_${FILE2}.out"
                        ./rmcastplot.bash $MAX_PACKETS_FROM_SOURCE $XYRANGE $FILE1 $FILE2 png> $OUTPUT
                        LOSS_OUTPUT="${LOSS_INFO_DIR}/${FILE1}_${LOSS_INFO_FILE}.loss"
                        echo "${LOSS_INFO_FILE}">> $LOSS_OUTPUT
                        awk -F "=" -f $AWKSCRIPT output_file=$LOSS_OUTPUT $OUTPUT 
                        echo "--------"
                    fi
                done
            done
        else
            for FILE1 in `ls *.log`;do
                for FILE2 in `ls *.log`;do
                    if [ "$FILE1" != "$FILE2" ];then
                        echo "--------"
                        echo "$FILE1 -> $FILE2"
                        OUTPUT="${FILE1}_${FILE2}.out"
                        ./rmcastplot.bash $MAX_PACKETS_FROM_SOURCE $XYRANGE $FILE1 $FILE2 png> $OUTPUT
                        awk -F "=" -f $AWKSCRIPT output_file=$OUTPUT $OUTPUT
                        echo "--------"
                    fi
                done
            done
        fi

   fi
   

