#!/usr/bin/env python
import sys, os, stat

params = map(int, sys.argv[1:7])
if (len(sys.argv) > 7):
  iter_to = int(sys.argv[7]) + 1
else:
  iter_to = 31
if (len(sys.argv) > 8):
  iter_from = int(sys.argv[8])
else:
  iter_from = 1
output_file = '%in_%i_%i_%in_%i_%i' % tuple(params)
directory = './experiments/'
if not os.path.exists(directory):
    os.makedirs(directory)
file_name = directory + output_file
file = open(file_name, 'w')
step_0 = params[0]
step_3 = params[3]

file.write('#!/bin/bash\n')
for i in range(iter_from,iter_to):
  params[0] = (i)*step_0
  params[3] = (i)*step_3
  format = tuple(params)+(output_file,)
  file.write('./run %i %i %i %i %i %i %s\n' % format )
file.close()
os.chmod(file_name, stat.S_IRWXU)
