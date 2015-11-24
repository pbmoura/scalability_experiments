#!/bin/bash
sar -o /var/scratch/$USER/sar/$4.play 1 900 > /dev/null &
bin/play $1 $2 $3 1>>results/$4.play 2>>logs/$4.play
#killall sar
