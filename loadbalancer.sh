#!/bin/bash
sar -o /var/scratch/$USER/sar/$4.lb 1 900 > /dev/null &
./loadbalancer $1 $2 $3 1>>results/$4.lb 2>>logs/$4.lb
#killall sar
