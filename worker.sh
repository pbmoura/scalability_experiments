#!/bin/bash
sar -o /var/scratch/$USER/sar/$6.worker-`hostname` 1 900 > /dev/null &
echo $1 $2 $3 $4 `date` >> logs/$6.worker-`hostname`
echo "#" $1 $2 $3 $4 `date` >> results/$6.worker-`hostname`
bin/worker $1 $2 $3 $4 $5 1>>results/$6.worker-`hostname` 2>>logs/$6.worker-`hostname`
#killall sar
