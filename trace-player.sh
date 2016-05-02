#!/bin/bash
sar -o /var/scratch/$USER/sar/$3.play 1 900 > /dev/null &
bin/play $1 $2 1>>~/scratch/results/$3.play 2>>~/scratch/logs/$3.play
#killall sar
