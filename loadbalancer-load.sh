#!/bin/bash
sar -o /var/scratch/$USER/sar/$5.lb 1 900 > /dev/null &
bin/load $1 $2 $3 $4 3 1>>~/scratch/results/$5.lb 2>>~/scratch/logs/$5.lb
#killall sar
