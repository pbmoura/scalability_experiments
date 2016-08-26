#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
SLA <- args[3]

#file_name1 <- "./data/step2/single3b_20.play"
#file_name2 <- "./data/step2/elastic3b_20.play"

#file_name1 <- "./data/step3/singleFIFA63d/30/results/singleFIFA63d.play"
#file_name2 <- "./data/step3/uslFIFA63d/30/results/uslFIFA63d.play"

#file_name1 <- "./data/step3/singleFIFA63d/results/singleFIFA63d.play"
#file_name2 <- "./data/step3/uslFIFA63d/results/uslFIFA63d.play"
#file_name3 <- "./data/step3/uslFIFA63d/10-2/results/uslFIFA63d.play"

#file_name1 <- "./data/step4/10-2/results/uslFIFA63d.play"
#file_name2 <- "./data/step4/10/results/uslFIFA63d.play"

#file_name2 <- "./data/step4/single/10/2/results/singleFIFA63d.play"
#file_name3 <- "./data/step4/single/10/3/results/singleFIFA63d.play"
SLA <- 3000

data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)

#data <- rbind(data1[order(data1$V1),3], data2[order(data2$V1),3])

sla_violation1 <- round(length(data1$V3[data1$V3>SLA])/length(data1$V3) * 100)
sla_violation2 <- round(length(data2$V3[data2$V3>SLA])/length(data2$V3) * 100)
sla_violation3 <- round(length(data3$V3[data3$V3>SLA])/length(data3$V3) * 100)

#barplot
#barplot(data1$V3)
barplot(data, beside=TRUE, legend.text=c(paste0("single-step ", sla_violation1, "%") , paste0("USL ", sla_violation2, "%" )), args.legend=list(title="% SLA violations",bty='n'), xlab="requests", ylab="response time (ms)", border=NA, col=c("red", "blue"))
abline(h=SLA)

#lineplot
ymax = max(data1[order(data1$V1),3], data2[order(data2$V1),3], data3[order(data3$V1),3])
plot(data1[order(data1$V1),3], ylim=c(0,ymax), col='red', type='l', xlab="#request", ylab="response time (ms)")
lines(data2[order(data2$V1),3], col='blue', type='l')
lines(data3[order(data3$V1),3], col='green', type='l')
abline(h=SLA)
#legend("topright", legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" )), col=c("red", "blue"), lty=1, bty='n', title="% SLA violations")
legend("topright", legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" ), paste0("USL-short history", sla_violation3, "%" )), col=c("red", "blue", "green"), lty=1, bty='n', title="% SLA violations")

#wl <- data.frame(1:length(data1$V2) * 3, 1000000/data1[order(data1$V1),2])
#plot_over(wl, "workload", type="l", ylim=c(0,max(1000000/data1[order(data1$V1),2])))

#points - over time
time_sec1 <- (data1[order(data1$V1),1]-data1[1,1])/1000
time_sec2 <- (data2[order(data2$V1),1]-data2[1,1])/1000
time_sec3 <- (data3[order(data3$V1),1]-data3[1,1])/1000
#ymax = max(data1[order(data1$V1),3], data2[order(data2$V1),3], na.rm = TRUE)
ymax = max(data1[order(data1$V1),3], data2[order(data2$V1),3], data3[order(data3$V1),3], na.rm = TRUE)
plot(time_sec1, data1[order(data1$V1),3], ylim=c(0,ymax), col='red', xlab="time (sec)", ylab="response time (ms)", pch='.')
points(time_sec2, data2[order(data2$V1),3], col='blue', pch='.')
points(time_sec3, data3[order(data3$V1),3], col='green', pch='.')
abline(h=SLA)
legend("topright", legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" ), paste0("USL-short history", sla_violation3, "%" )), col=c("red", "blue", "green"), lty=1, bty='n', title="% SLA violations")
#legend("topright", legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" )), col=c("red", "blue"), lty=1, bty='n', title="% SLA violations")
legend("topright", legend = c(paste0("USL-short history ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" )), col=c("red", "blue"), lty=1, bty='n', title="% SLA violations")


plot(time_sec1, data1[order(data1$V1),3], xlab="time (sec)", ylab="response time (ms)", pch='.')

mean(data1$V3)
mean(data2$V3)
mean(data3$V3)

sd(data1$V3)
sd(data2$V3)
sd(data3$V3)

max(data1$V3)
max(data2$V3)
max(data3$V3)


#barplot(data2[order(data2$V1),3], beside=TRUE, , xlab="requests", ylab="response time (ms)", border=NA)

