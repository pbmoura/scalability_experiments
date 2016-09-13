#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
file_name3 <- args[3]

#file_name1 <- './data/step2/single3c_20.ad'
#file_name2 <- './data/step2/elastic3c_20.ad'
#file_name3 <- './data/step2/elastic3c_20.hand'

#file_name1 <- './data/step3/usl1000.mon'
#file_name3 <- './data/step3/usl600.hand'


file_name1 <- './data/step3/uslFIFA63d/arrival-departure'
file_name2 <- './data/step3/singleFIFA63d/arrival-departure'
file_name3 <- './data/step3/uslFIFA63c/handling'

file_name4 <- './data/step3/uslFIFA63c-old/handling'

#file_name1 <- './data/step4/10-pool-lim/monitoring'
#file_name2 <- './data/step4/10/monitoring'
#file_name3 <- './data/step4/10-2/monitoring'


data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)
data4 <- read.table(file_name4)

reqs <- read.table('./data/step3/uslFIFA63c-old/day63c.reqs')
plot(reqs$V1, type='l')
wd <- workload(data3)
wp <- workload(data4)
as.vector(wd[1:min(length(wd), length(wp))])-as.vector(wp[1:min(length(wd), length(wp))])
#plot(wd, type = 'l')
#plot(wp, type = 'l')
#lines(wd, col='blue', type='l')
#lines(wp, col='red', type='l')
time_sec1 <- (data1$V1-data1[1,1])/1000
time_sec2 <- (data2$V1-data2[1,1])/1000
time_sec3 <- (data3$V1-data3[1,1])/1000

par(mar = c(5,4,1,5))
#plot(time_sec1, data1$V3, type="l", col="red", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(data1$V3)), xlim=c(0,max(time_sec1)))
plot(time_sec1, data1$V3, type="l", col="red", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(data1$V3, data2$V3, data3$V3)), xlim=c(0,max(time_sec1, time_sec2)))
lines(time_sec2, data2$V3, col="blue")
lines(time_sec3, data3$V3, col="green")
plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0,max(wd$intervals[wd$intervals != Inf])), lty=2)
legend("topleft", legend = c("workload", "single-step", "USL"), col=c("black", "red", "blue"), lty=c(2, 3, 1), bty='n')
#legend("topleft", legend = c("USL", "single-step", "workload"), col=c("blue", "red", "black"), lty=1, bty='n')
legend("topright", legend = c("USL-pool limit", "USL-avail", "USL-short avail"), col=c("red", "blue", "green"), lty=1, bty='n')

avg(time_sec1, data1$V3)
avg(time_sec2, data2$V3)
avg(time_sec3, data3$V3)

max(data1$V3)
max(data2$V3)
max(data3$V3)


