#!/bin/bash
sar -o /var/scratch/$USER/sar/${10}.client 1 900 > /dev/null &
bin/client $1 $2 $3 $4 $5 $6 $7 $8 $9 1>>results/${10}.client 2>>logs/${10}.client
killall sar
