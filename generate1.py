#!/usr/bin/env python
import sys, os, stat

#params = map(int, sys.argv[1:8])
params = sys.argv[1:8]
if (len(sys.argv) > 8):
  iter_to = int(sys.argv[8]) + 1
else:
  iter_to = 11
if (len(sys.argv) > 9):
  iter_from = int(sys.argv[9])
else:
  iter_from = 1
output_file = '%sn_%s_%s_%s_%s_%s_%s' % tuple(params)
directory = './experiments/'
if not os.path.exists(directory):
    os.makedirs(directory)
file_name = directory + output_file
file = open(file_name, 'w')
step_0 = int(params[0])

file.write('#!/bin/bash\n')
for i in range(iter_from,iter_to):
  params[0] = (i)*step_0
  format = tuple(params)+(i, output_file,)
  file.write('./run %i %s %s %s %s %s %s %i %s\n' % format )
file.close()
os.chmod(file_name, stat.S_IRWXU)
