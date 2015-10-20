This is a simple system to emulate a service receiving a workload and distributing it to some instances.
It is composed by the following executables:
- client: creates a set of processes to emulate simultanous users;
- loadbalancer: receives messages from client and ditribute to workers doing round robin
- worker: emulate task execution

You can use make to compile all the executables. The binaries will at bin/ folder.

Each of those executables receives some parameters.
The script generate.py creates a bash script, in experiments folder, to automate experiments execution.
The generated script, when executed, runs the system, distributed in DAS-4 nodes, a sequence of times,
varying two of executables' parameters (the number of client processes and the number of worker instances).
generate.py receives the following parameters:

$1) number of client processes (per step)
$2) the number of messages each client process will send to the load balancer
$3) the duration of each task in a worker
$4) number of nodes running a worker (per step)
$5) the duration of each synchronization between two workers
$6) (optional) final step. default=30
$7) (optional) initial step. default=1

then, it produces a file with one line per step to execute in the experiment.
In the first line, the number of processes is equals to $1 * $7 and number of worker instances equals to $$4 * $7.
In each subsequent line, they are incremented by $1 and $4 respectively.

During the execution, time measurements are saved in results folder.
Other messages are saved in logs folder.

