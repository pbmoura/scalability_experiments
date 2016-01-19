#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
#file_name1 <- "./data/step2/load2_30.mon"
#file_name2 <- "./data/step2/elastic2_30.mon"

data1 <- read.table(file_name1)
data2 <- read.table(file_name2)

time_sec1 <- (data1$V1-data1[1,1])/1000
time_sec2 <- (data2$V1-data2[1,1])/1000

plot(time_sec1, data1$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4, data2$V4)))
lines(time_sec2, data2$V4, col="blue")
legend(x = 0, y = max(data1$V4), legend = c("USL", "single-step"), col=c("blue", "red"), lty=1)
