#!/bin/bash
sar -o /var/scratch/$USER/sar/$3.pm 1 900 > /dev/null &
bin/pool $1 $2 1>>~/scratch/results/$3.pm 2>>~/scratch/logs/$3.pm
#killall sar
