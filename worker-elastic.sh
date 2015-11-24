#!/bin/bash
sar -o /var/scratch/$USER/sar/$4.worker-`hostname` 1 900 > /dev/null &
echo $1 $2 $3 `date` >> logs/$4.worker-`hostname`
echo "#" $1 $2 $3 `date` >> results/$4.worker-`hostname`
bin/worker $1 $2 $3 1>>results/$4.worker-`hostname` 2>>logs/$4.worker-`hostname`
#killall sar
