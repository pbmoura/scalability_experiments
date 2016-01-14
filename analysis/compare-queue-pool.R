#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
file_name3 <- args[3]

data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)

time_sec1 <- (data1$V1-data1[1,1])/1000
time_sec2 <- (data2$V1-data2[1,1])/1000
time_sec3 <- (data3[order(data3$V1),1]-min(data3$V1))/1000

plot(time_sec1, data1$V3, type="l", col="red", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(data1$V3, data2$V3)), xlim=c(0,max(time_sec1, time_sec2)))
lines(time_sec2, data2$V3, col="blue")
par(new = T)
plot(time_sec3, 1000/data3[order(data3$V1),2], type="l", axes=F, xlab=NA, ylab=NA, ylim=c(0,max(1000/data3[,2])))
axis(side = 4)
mtext(side = 4, line = 3, 'Requests per minute')
legend("topleft", legend = c("USL", "single-step", "workload"), col=c("blue", "red", "black"), lty=1)



#legend(x = 0, y = max(data1$V3), legend = c("USL counting arrivals", "USL with Little's Law"), col=c("blue", "red"), lty=1)

plot(time_sec1, data1$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4, data2$V4)))
lines(time_sec2, data2$V4, col="blue")
legend("topleft", legend = c("USL", "single-step", "workload"), col=c("blue", "red", "black"), lty=1)

par(new = T)
plot(time_sec, 1000/data3[order(data3$V1),2], type="l", axes=F, xlab=NA, ylab=NA, ylim=c(0,max(1000/data3[,2])))
axis(side = 4)
mtext(side = 4, line = 3, 'Requests per minute')
