#!/bin/bash
grep "queue size" $1 |cut -d ' ' -f 1,9
