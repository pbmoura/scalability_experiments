#!/bin/bash
sar -o /var/scratch/$USER/sar/$7.client 1 900 > /dev/null &
bin/client $1 $2 $3 $4 $5 $6 1>>results/$7.client 2>>logs/$7.client
killall sar
