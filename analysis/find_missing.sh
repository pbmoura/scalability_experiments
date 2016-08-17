#!/bin/bash
j=0
#for i in $(grep LISTEN logs/uslFIFA63d.worker-node0* |cut -d ' ' -f 5  |sort -n)
while read i;
do
  if [ "$i" == "$j" ]; then
    j=$(($j+1))
  else
    for (( k=$j; k<$i; k++ ))
    do
      echo $1 $k $2
    done
    j=$(($i+1))
  fi
# echo $i
done
