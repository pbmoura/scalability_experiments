#!/bin/bash
grep "queue size" $1 |cut -d ' ' -f 9|sort -n |tail -n 50
