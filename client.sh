#!/bin/bash
#sar -o /var/scratch/$USER/sar/$9.client 1 900 > /dev/null &
bin/client $1 $2 $3 $4 $5 $6 $7 $8 $1 1>>results/$9.client 2>>logs/$9.client
#killall sar
