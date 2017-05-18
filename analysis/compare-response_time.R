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

#file_name1 <- "./data/step4/10-pool-lim/results/uslFIFA63d.play"
#file_name2 <- "./data/step4/10/results/uslFIFA63d.play"
#file_name1 <- "./data/step4/10-2/results/uslFIFA63d.play"

#file_name1 <- "./data/step4/10-pool-lim/results/uslFIFA63d.play"
#file_name2 <- "./data/step4/estimated_start/results/uslFIFA63d.play"
#file_name3 <- "./data/step4/10-2/results/uslFIFA63d.play"

#file_name1 <- "./data/step4/estimated_start/10-2-pool-lim/results/uslFIFA63d.play"
#file_name2 <- "./data/step4/estimated_start/10-pool-lim/results/uslFIFA63d.play"
#file_name3 <- "./data/step4/estimated_start/single-10/results/singleFIFA63d.play"

#file_name1 <- "./data/step4/estimated_start/30-pool-lim/results/uslFIFA63d.play"
#file_name2 <- "./data/step4/estimated_start/30-6-pool-lim/results/uslFIFA63d.play"
#file_name3 <- "./data/step4/estimated_start/30-single/results/singleFIFA63d.play"

#file_name1 <- "./data/step4/estimated_start/30-6-pool-lim/results/uslFIFA63d.play"
#file_name2 <- "./data/step4/estimated_start/30-single/results/singleFIFA63d.play"
#file_name3 <- "./data/step4/estimated_start/30-pool-lim/results/uslFIFA63d.play"

#file_name1 <- "./data/step4/30-pool-lim/results/uslFIFA63d.play"
#file_name2 <- "./data/step4/30-6-pool-lim/results/uslFIFA63d.play"

#file_name2 <- "./data/step4/single/10/2/results/singleFIFA63d.play"
#file_name3 <- "./data/step4/single/10/3/results/singleFIFA63d.play"

#file_name1 <- "./data/step4/synthetic/results/usl3b.play"
#file_name2 <- "./data/step4/synthetic/results/usl5b.play"

#file_name1 <- './data/step2/single3b_20.play'
#file_name2 <- './data/step2/elastic3b_20.play'
#file_name3 <- './data/step4/synthetic/results/usl3b.play'



#file_name1 <- './data/step2/single5b_20.play'
#file_name2 <- './data/step2/elastic5b_20.play'
#file_name3 <- './data/step4/synthetic/results/usl5b.play'

SLA <- 3000
SLA <- 13000
SLA <- 8000

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
lines(data3[order(data3$V1),3], col='dark green', type='l')
abline(h=SLA)
#legend("topright", legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" )), col=c("red", "blue"), lty=1, bty='n', title="% SLA violations")
legend("topright", legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" ), paste0("USL-limit ", sla_violation3, "%" )), col=c("red", "blue", "dark green"), lty=1, bty='n', title="% SLA violations")

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
points(time_sec3, data3[order(data3$V1),3], col='dark green', pch='.')
abline(h=SLA)
legend("topright", legend = c(paste0("USL-pool limit ", sla_violation1, "%"), paste0("USL-short ", sla_violation2, "%" ), paste0("single-step ", sla_violation3, "%" )), col=c("red", "blue", "black"), lty=1, bty='n', title="% SLA violations")
#legend("topright", legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" )), col=c("red", "blue"), lty=1, bty='n', title="% SLA violations")
legend("topright", legend = c(paste0("USL-pool limit ", sla_violation1, "%"), paste0("USL-avail ", sla_violation2, "%" )), col=c("red", "blue"), lty=1, bty='n', title="% SLA violations")


#lines - over time
time_sec1 <- (data1[order(data1$V1),1]-data1[1,1])/1000
time_sec2 <- (data2[order(data2$V1),1]-data2[1,1])/1000
time_sec3 <- (data3[order(data3$V1),1]-data3[1,1])/1000
#ymax = max(data1[order(data1$V1),3], data2[order(data2$V1),3], na.rm = TRUE)
ymax = max(data1[order(data1$V1),3], data2[order(data2$V1),3], data3[order(data3$V1),3], na.rm = TRUE)
plot(time_sec1, data1[order(data1$V1),3], ylim=c(0,ymax), col='red', xlab="time (sec)", ylab="response time (ms)", type='l', lty=2)
lines(time_sec2, data2[order(data2$V1),3], col='blue', lty=3)
lines(time_sec3, data3[order(data3$V1),3], col='green', lty=4)
abline(h=SLA, lty=6)
legend(y=ymax, x=250, legend = c(paste0("single-step ", sla_violation1, "%"), paste0("USL ", sla_violation2, "%" ), paste0("USL-limit ", sla_violation3, "%" )), col=c("red", "blue", "dark green"), lty=1, bty='n', title="% SLA violations")
legend(y=ymax, x=200, legend = c("single-step", "USL", "USL-limit", "SLA"), col=c("red", "blue", "dark green", "black"), lty=1, bty='n')
legend("topleft", legend = c("single-step", "USL", "USL-limit", "workload", "SLA"), col=c("red", "blue", "dark green", "black", "black"), lty=c(2,3,4,1,6), bty='n')

rt_mean1 <- tapply(data1[order(data1$V1),3], data1[order(data1$V1),1] %/% 10000, mean)
rt_mean2 <- tapply(data2[order(data2$V1),3], data2[order(data2$V1),1] %/% 10000, mean)

rt_mean1 <- tapply(data1[order(data1$V1),3], data1[order(data1$V1),1] %/% 10000, median)
rt_mean2 <- tapply(data2[order(data2$V1),3], data2[order(data2$V1),1] %/% 10000, median)


plot(seq(from=5, by=10, length.out=length(rt_mean1)), rt_mean1)
points(seq(from=5, by=10, length.out=length(rt_mean2)), rt_mean2, col="red")

ks.test(data1[order(data1$V1),3], data2[order(data2$V1),3], alternative="g")
wilcox.test(data1[order(data1$V1),3], data2[order(data2$V1),3], conf.int=TRUE)
wilcox.test(rt_mean1, rt_mean2, conf.int=TRUE)
hist(data1[order(data1$V1),3])
hist(data2[order(data2$V1),3])

cdf1 <- ecdf(data1[order(data1$V1),3])
cdf2 <- ecdf(data2[order(data2$V1),3])
plot(cdf1)
lines(cdf2, col="red")
lines(abs(cdf1(1:2107) - cdf2(1:2107)), col='green')
plot(abs(cdf1(1:2107) - cdf2(1:2107)), col='green')

x <- rnorm(1e5, 1, 2)
ks.test(x, "pnorm", 1, 2)

#
by = 10000
ymax=3000
plot(time_sec1, data1[order(data1$V1),3], ylim=c(0,ymax), col='red', xlab="time (sec)", ylab="response time (ms)", pch='.')
points(time_sec1[seq(0, length(time_sec1), by)], data1[order(data1$V1),3][seq(0, length(time_sec1), by)], col='red', pch=3)
points(time_sec2, data2[order(data2$V1),3], col='blue', pch='.')
points(time_sec2[seq(0, length(time_sec2), 100000)], data2[order(data2$V1),3][seq(0, length(time_sec2), 100000)], col='blue', pch=4)

idxs = sapply(seq(0, time_sec1[length(time_sec1)], 100), function(x) {which.min(abs(time_sec1-x))})


(1:100)[2]
seq(0, length(time_sec1), 100)

diff <- data1[order(data1$V1),3]-data2[order(data2$V1),3]
plot(diff, ylim=c(-200,200))


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

