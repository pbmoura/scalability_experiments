#!/bin/bash
grep -v rate $1 | grep -P "arrival|departure" 
