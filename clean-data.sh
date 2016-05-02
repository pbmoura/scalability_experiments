#!/bin/bash
rm ~/scratch/results/$1*
rm ~/scratch/logs/$1*
if [[ -d /var/scratch/$USER/sar ]]; then
	rm /var/scratch/$USER/sar/$1*
fi

