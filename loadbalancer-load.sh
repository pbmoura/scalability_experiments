#!/bin/bash
sar -o /var/scratch/$USER/sar/$5.lb 1 900 > /dev/null &
bin/load $1 $2 $3 $4 1>>results/$5.lb 2>>logs/$5.lb
#killall sar
