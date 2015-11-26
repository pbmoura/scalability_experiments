#!/bin/bash
sar -o /var/scratch/$USER/sar/$5.worker-`hostname` 1 900 > /dev/null &
echo $1 $2 $3 $4`date` >> logs/$5.worker-`hostname`
echo "#" $1 $2 $3 $4 `date` >> results/$5.worker-`hostname`
bin/worker $1 $2 $3 $4 1>>results/$5.worker-`hostname` 2>>logs/$5.worker-`hostname`
#killall sar
