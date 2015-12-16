#!/usr/bin/Rscript

setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name <- args[1]    #"./data/cohe10/arrival-departure7d.dat"
data <- read.table(file_name)

time_sec <- (data$V1-data[1,1])/1000

times <- tapply(time_sec, data$V2,c)
quantities <- tapply(data$V3, data$V2,c)

pdf(file=paste0(file_name, ".pdf"))
plot(times$arrival, quantities$arrival, xlim = range(time_sec), pch=3, xlab="time (sec.)", ylab="queue/pool size", ylim=c(0,max(quantities$arrival)))
points(times$departure, quantities$departure, pch=4)
lines(time_sec, data$V4)
#legend(x = 75, y = 14, legend = c("arrival", "departure", "#workers"), pch = c(3, 4, NA), lty = c(NA, NA, 1))
dev.off()


