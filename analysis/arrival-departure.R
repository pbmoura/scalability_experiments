#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name <- args[1]    
file_name_h <- args[2] 

#file_name <- "./data/step3/uslFIFA2.ad"
#file_name_h <- "./data/step3/uslFIFA2.hand"
file_name <- './data/step3/uslFIFA63d/1/monitoring'
file_name <- './data/step4/estimated_start/monitoring'
file_name <- './data/step4/single/10/2/monitoring'
file_name <- './data/step3/singleFIFA63d/arrival-departure'
file_name <- './data/step4/synthetic/usl1_15.ad'

file_name <- './data/step4/estimated_start/10-2-pool-lim/monitoring'
data <- read.table(file_name)

time_sec <- (data$V1-data[1,1])/1000
times <- tapply(time_sec, data$V2,c)
quantities <- tapply(data$V3, data$V2,c)
xrange <- range(time_sec)

#pdf(file=paste0(file_name, ".pdf"))
#par(mar = c(4,4,2,4))
#plot(time_sec, data$V3, pch='.')
plot(time_sec, data$V3, xlim = xrange, type="l", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(data$V3)))
#plot(time_sec, data$V4, xlim = xrange, type="l", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data$V4)))
plot_over(data.frame(time_sec, data$V4), "pool size/workload", type = "s", xlim = xrange, lty=2, ylim = c(0,max(data$V4)))
#plot_over(data.frame(time_sec, data$V4), "pool size", type = "l", xlim = xrange, lty=2, ylim = c(0,max(data$V4)))
#plot(wl, type = "l", lty = 3)
legend("topleft", legend=c("queue", "pool"), lty=c(1,2), bty='n')
#dev.off()
data2 <- read.table(file_name_h)
wl <- workload(data2) 
plot_over(wl, "", type = "l", lty = 3, xlim = xrange, ylim = c(0,max(data$V4, 1000/max(wl$intervals))))
legend("topleft", legend=c("queue", "pool", "workload"), lty=c(1,2,3), bty='n')

ad <- by(data, data$V2, identity)

#pool
avg(time_sec, data$V4)
max(data$V4)
#queue
avg(time_sec, data$V3)
max(data$V3)
#cost
integral(time_sec, data$V4)
