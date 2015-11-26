#!/bin/bash
echo client params $1 $2 $3 $4 $5 $6 $7 $8
sar -o /var/scratch/$USER/sar/$8.client 1 900 > /dev/null &
bin/client $1 $2 $3 $4 $5 $6 $7 1>>results/$8.client 2>>logs/$8.client
killall sar
