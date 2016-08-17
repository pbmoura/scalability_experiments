#!/bin/bash
while read p;
do
  #echo '->'$p
  grep -e "${p}" $1
done
