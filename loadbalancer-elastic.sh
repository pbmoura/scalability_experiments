#!/bin/bash
sar -o /var/scratch/$USER/sar/$6.lb 1 900 > /dev/null &
bin/elastic $1 $2 $3 $4 $5 1>>results/$6.lb 2>>logs/$6.lb
#killall sar
