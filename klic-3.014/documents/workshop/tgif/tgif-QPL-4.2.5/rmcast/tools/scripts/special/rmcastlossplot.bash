#!/bin/bash

for i in `ls *.loss`;do
    awk -f lossplot.awk $i
done

for i in `ls *.plot`;do
     echo "set term png small color">temp
     echo "set output \"${i}.png\"">>temp
     echo "plot \"$i\" with lines">>temp
     gnuplot -persist < temp
done
\rm temp *.plot

