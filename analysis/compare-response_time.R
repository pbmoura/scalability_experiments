#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
SLA <- args[3]

#file_name1 <- "./data/step2/single3b_20.play"
#file_name2 <- "./data/step2/elastic3b_20.play"

file_name1 <- "./data/step3/uslTEST2.play"

#SLA <- 13000
data1 <- read.table(file_name1)
data2 <- read.table(file_name2)

data <- rbind(data1[order(data1$V1),3], data2[order(data2$V1),3])

sla_violation1 <- round(length(data1$V3[data1$V3>SLA])/length(data1$V3) * 100)
sla_violation2 <- round(length(data2$V3[data2$V3>SLA])/length(data2$V3) * 100)

#barplot
#barplot(data1$V3)
barplot(data, beside=TRUE, legend.text=c(paste0("single-step ", sla_violation1, "%") , paste0("USL ", sla_violation2, "%" )), args.legend=list(title="% SLA violations",bty='n'), xlab="requests", ylab="response time (ms)", border=NA, col=c("red", "blue"))
abline(h=SLA)

#lineplot
ymax = max(data1[order(data1$V1),3], data2[order(data2$V1),3])
plot(data1[order(data1$V1),3], ylim=c(0,ymax), col='red', lty=3, type='l', xlab="#request", ylab="response time (ms)")
lines(data2[order(data2$V1),3], col='blue', type='l')
abline(h=SLA)
legend("topright", legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" )), col=c("red", "blue"), lty=c(3,1), bty='n', title="% SLA violations")

#wl <- data.frame(1:length(data1$V2) * 3, 1000000/data1[order(data1$V1),2])
#plot_over(wl, "workload", type="l", ylim=c(0,max(1000000/data1[order(data1$V1),2])))


mean(data1$V3)
mean(data2$V3)

sd(data1$V3)
sd(data2$V3)

max(data1$V3)
max(data2$V3)


#barplot(data2[order(data2$V1),3], beside=TRUE, , xlab="requests", ylab="response time (ms)", border=NA)
