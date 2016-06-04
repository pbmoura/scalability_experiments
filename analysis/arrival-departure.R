#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name <- args[1]    
file_name_h <- args[2] 

#file_name <- "./data/step3/uslFIFA2.ad"
#file_name_h <- "./data/step3/uslFIFA2.hand"


data <- read.table(file_name)
data2 <- read.table(file_name_h)

wl <- workload(data2) 
time_sec <- (data$V1-data[1,1])/1000
times <- tapply(time_sec, data$V2,c)
quantities <- tapply(data$V3, data$V2,c)
xrange <- range(time_sec)

#pdf(file=paste0(file_name, ".pdf"))
par(mar = c(4,4,2,4))
plot(time_sec, data$V3, xlim = xrange, type="l", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(quantities$arrival)))
plot_over(data.frame(time_sec, data$V4), "pool size/workload", type = "l", xlim = xrange, lty=2, ylim = c(0,max(data$V4, 1000/max(wl$intervals))))
plot_over(wl, "", type = "l", lty = 3, xlim = xrange, ylim = c(0,max(data$V4, 1000/max(wl$intervals))))
#plot(wl, type = "l", lty = 3)
legend("topleft", legend=c("queue", "pool", "workload"), lty=c(1,2,3), bty='n')
#dev.off()

ad <- by(data, data$V2, identity)
