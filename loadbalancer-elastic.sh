#!/bin/bash
sar -o /var/scratch/$USER/sar/$8.lb 1 900 > /dev/null &
bin/elastic $1 $2 $3 $4 $5 $6 $7 1>>results/$8.lb 2>>logs/$8.lb
#killall sar
