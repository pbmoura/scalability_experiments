#!/bin/bash
sar -o /var/scratch/$USER/sar/$7.lb 1 900 > /dev/null &
bin/elastic $1 $2 $3 $4 $5 $6 1>>results/$7.lb 2>>logs/$7.lb
#killall sar
