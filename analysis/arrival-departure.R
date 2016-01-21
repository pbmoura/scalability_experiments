#!/usr/bin/Rscript
source("./common.R")
#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name <- args[1]    
file_name2 <- args[2] 

#file_name <- "./data/step2/elastic3c_20.ad"
#file_name2 <- "./data/step2/elastic3c_20.hand"


data <- read.table(file_name)
data2 <- read.table(file_name2)

wl <- workload(data2)
time_sec <- (data$V1-data[1,1])/1000
times <- tapply(time_sec, data$V2,c)
quantities <- tapply(data$V3, data$V2,c)
xrange <- range(time_sec)

pdf(file=paste0(file_name, ".pdf"))
par(mar = c(5,5,2,5))
plot(time_sec, data$V3, xlim = xrange, type="l", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(quantities$arrival)))
plot_over(data.frame(time_sec, data$V4), "pool size/workload", type = "l", xlim = xrange, lty=2, ylim = c(0,max(data$V4, 1000/max(wl_frame$intervals))))
plot_over(wl, "", type = "l", lty = 3, xlim = xrange, ylim = c(0,max(data$V4, 1000/max(wl_frame$intervals))))
legend("topleft", legend=c("queue", "pool", "workload"), lty=c(1,2,3))
dev.off()

