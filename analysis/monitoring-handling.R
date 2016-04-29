#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
file_name3 <- args[3]

#file_name1 <- './data/step2/single3c_20.mon'
#file_name2 <- './data/step2/elastic3c_20.mon'
#file_name3 <- './data/step2/elastic3c_20.hand'


file_name1 <- './data/step3/uslTEST2.mon'
file_name3 <- './data/step3/usl1000.hand'


data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)

wd <- workload(data3)
time_sec1 <- (data1$V1-data1[1,1])/1000
time_sec2 <- (data2$V1-data2[1,1])/1000

par(mar = c(5,5,2,5))
#plot(time_sec1, data1$V3, type="l", col="red", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(data1$V3)), xlim=c(0,max(time_sec1)))
plot(time_sec1, data1$V3, type="l", col="red", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(data1$V3, data2$V3)), xlim=c(0,max(time_sec1, time_sec2)))
lines(time_sec2, data2$V3, col="blue")
plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0,max(wd$intervals[wd$intervals != Inf])))
#plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1)), ylim=c(0,max(wd)))
#plot(wd, type = "l")
legend("topright", legend = c("USL", "single-step", "workload"), col=c("blue", "red", "black"), lty=1, bty='n')
#legend("topleft", legend = c("USL", "single-step", "workload"), col=c("blue", "red", "black"), lty=1, bty='n')

plot(time_sec1, data1$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size")

avg(time_sec1, data1$V3)
avg(time_sec2, data2$V3)

max(data1$V4)
max(data2$V4)

