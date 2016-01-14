#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name <- args[1]    
file_name2 <- args[2] 
#file_name <- "./data/step2/elastic2_30.ad"
#file_name2 <- "./data/step2/elastic2_30.play"

data <- read.table(file_name)
data2 <- read.table(file_name2)

time_sec <- (data$V1-data[1,1])/1000
time_sec2 <- (data2[order(data2$V1),1]-min(data2$V1))/1000

times <- tapply(time_sec, data$V2,c)
quantities <- tapply(data$V3, data$V2,c)

pdf(file=paste0(file_name, ".pdf"))
plot(time_sec, data$V3, xlim = range(time_sec), type="l", xlab="time (sec.)", ylab="queue size", ylim=c(0,max(quantities$arrival)))
par(new = T)
plot(time_sec2, 1000/data2[order(data2$V1),2], type="l", lty=2, axes=F, xlab=NA, ylab=NA, ylim=c(0,max(1000/data2[,2])))
axis(side = 4)
mtext(side = 4, line = 3, 'pool size')
legend("topleft", legend=c("queue", "pool"), lty=c(1,2))
dev.off()

#plot(times$arrival, quantities$arrival, xlim = range(time_sec), pch=3, xlab="time (sec.)", ylab="queue/pool size", ylim=c(0,max(quantities$arrival)))
#points(times$departure, quantities$departure, pch=4)
#lines(time_sec, data$V4)
#legend(x = 75, y = 14, legend = c("arrival", "departure", "#workers"), pch = c(3, 4, NA), lty = c(NA, NA, 1))