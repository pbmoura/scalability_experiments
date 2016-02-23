#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
file_name3 <- args[3]

#file_name1 <- './data/step2/elastic3b_20.ad'
#file_name2 <- './data/step2/elastic3b_1.ad'
#file_name3 <- './data/step2/elastic3b_20.hand'


data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)

wd <- workload(data3)
time_sec1 <- (data1$V1-data1[1,1])/1000
time_sec2 <- (data2$V1-data2[1,1])/1000

par(mar = c(5,5,2,5))
plot(time_sec1, data1$V3, type="l", col="red", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(data1$V3, data2$V3)), xlim=c(0,max(time_sec1, time_sec2)))
lines(time_sec2, data2$V3, col="blue")
plot_over(wd, "workload/pool size", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0,max(wd$intervals[wd$intervals != Inf],data1$V4, data2$V4)))
plot_over(data.frame(time_sec1, data1$V4), "workload/pool size", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0,max(wd$intervals[wd$intervals != Inf],data1$V4, data2$V4)), lty = 2, col = 'red')
plot_over(data.frame(time_sec2, data2$V4), "workload/pool size", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0,max(wd$intervals[wd$intervals != Inf],data1$V4, data2$V4)), lty = 2, col = 'blue')
legend("topleft", legend = c("USL queue", "single-step queue", "USL pool", "single-step pool", "workload"), col=c("blue", "red", "blue", "red", "black"), lty=c(1,1,2,2,1), bty = 'n')

avg(time_sec1, data1$V3)
avg(time_sec2, data2$V3)

avg(time_sec1, data1$V4)
avg(time_sec2, data2$V4)

integral(time_sec1, data1$V4)
integral(time_sec2, data2$V4)
