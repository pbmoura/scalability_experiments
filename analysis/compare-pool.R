#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
file_name3 <- args[3]

#file_name1 <- './data/step2/single5b_20.ad'
#file_name2 <- './data/step2/elastic5b_20.ad'
#file_name3 <- './data/step2/elastic5b_20.hand'

file_name1 <- './data/step3/uslFIFA63b/arrival-departure'
file_name2 <- './data/step3/uslFIFA63d/arrival-departure'
file_name3 <- './data/step3/uslFIFA2b/handling'
file_name4 <- './data/step3/uslFIFA2b/player'




data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)
data4 <- read.table(file_name4)

wd <- workload(data3)
pl <- workload(data4)
#plot(wd,type='l', col='red')
#plot(pl, type='l')
#lines(pl)
time_sec1 <- (data1$V1-data1[1,1])/1000
time_sec2 <- (data2$V1-data2[1,1])/1000


par(mar = c(5,4,1,5))
#plot(time_sec1, data1$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4)), xlim=c(0,max(time_sec1)))
#plot(time_sec2, data2$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data2$V4)), xlim=c(0,max(time_sec2)))
plot(time_sec1, data1$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4, data2$V4)), xlim=c(0,max(time_sec1, time_sec2)), lty=3)
lines(time_sec2, data2$V4, col="blue")
plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0,max(wd$intervals[wd$intervals != Inf])), lty=2)
#plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1)), ylim=c(0,max(wd$intervals[wd$intervals != Inf])))
legend("topright", legend = c("workload", "single-step", "USL"), col=c("black", "red", "blue"), lty=c(2,3,1), bty='n')

legend(x = 0, y = 5, legend = c("USL", "single-step", "workload"), col=c("blue", "red", "black"), lty=1, bty = 'n')

avg(time_sec1, data1$V4)
avg(time_sec2, data2$V4)

integral(time_sec1, data1$V4)
integral(time_sec2, data2$V4)
