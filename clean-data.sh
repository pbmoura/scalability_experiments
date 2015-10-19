#!/bin/bash
rm results/$1*
rm logs/$1*
if [[ -d /var/scratch/$USER/sar ]]; then
	rm /var/scratch/$USER/sar/$1*
fi

