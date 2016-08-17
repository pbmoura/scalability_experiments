service_time = 0.200
contention = 0.00006
coherency = 0.00003
threads = 12

num_reqs = 600

num_peers = trunc(reqs/(threads*(1/service_time)) +1)
plot(num_peers$V1, type='l', ylim=c(0,12))


#task_time = service_time + (num_peers-1)*contention + (num_peers-1)*num_peers*coherency
#num_peers = num_reqs/(threads*(1/task_time))
