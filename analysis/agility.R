#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
file_name3 <- args[3]

#file_name1 <- './data/step2/single3b_20.mon'
#file_name2 <- './data/step2/elastic3b_20.mon'
#file_name3 <- './data/step2/elastic3b_1.mon'


data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)

time_sec1 <- (data1$V1-data1[1,1])/1000
time_sec2 <- (data2$V1-data2[1,1])/1000
time_sec3 <- (data3$V1-data3[1,1])/1000


plot(time_sec1,data1$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4, data2$V4)), xlim=c(0,max(time_sec1, time_sec2)))
lines(time_sec2, data2$V4, col="blue")
lines(time_sec3, data3$V4)

legend("topleft", legend = c("USL", "single-step", "baseline"), col=c("blue", "red", "black"), lty=1)


SAMPLING_INTERVAL <- 20
delta1 <- abs(data1$V4 - data3$V4[((seq(1,length(time_sec1)) -1) *SAMPLING_INTERVAL) +1])
agility1 <- sum(delta1)

delta2 <- abs(data2$V4 - data3$V4[((seq(1,length(time_sec2)) -1) *SAMPLING_INTERVAL) +1])
agility2 <- sum(delta2)
