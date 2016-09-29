#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
file_name3 <- args[3]

#file_name1 <- './data/step2/single3b_20.ad'
#file_name2 <- './data/step2/elastic3b_20.ad'
#file_name3 <- './data/step4/synthetic/usl3b.ad'

#file_name1 <- './data/step3/singleFIFA63d/arrival-departure'
#file_name1 <- './data/step3/uslFIFA63d/30-5/arrival-departure'
#file_name2 <- './data/step3/uslFIFA63d/arrival-departure'
#file_name3 <- './data/step3/day63d.reqs'

#file_name1 <- './data/step3/singleFIFA63d/monitoring'
#file_name2 <- './data/step3/uslFIFA63d/monitoring'
#file_name3 <- './data/step3/uslFIFA63d/10-2/monitoring'
#file_name3 <- './data/step3/day63d.reqs'

#file_name1 <- './data/step4/10-2/monitoring'
#file_name2 <- './data/step4/10/monitoring'

#file_name1 <- './data/step4/10-pool-lim/monitoring'
#file_name2 <- './data/step4/10/monitoring'
#file_name3 <- './data/step4/single/10/monitoring'

#file_name1 <- './data/step4/estimated_start/single-10/monitoring'
#file_name2 <- './data/step4/estimated_start/10-usl/monitoring'
#file_name3 <- './data/step4/estimated_start/10-2-usl/monitoring'

#file_name1 <- './data/step4/estimated_start/10-2-avail/monitoring'
#file_name2 <- './data/step4/estimated_start/10-2-pool-lim/monitoring'


#file_name1 <- './data/step4/30-pool-lim/monitoring'
#file_name2 <- './data/step4/30-6-pool-lim/monitoring'


data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)

# wd <- workload(data3)
#plot(wd,type='l', col='red')
#plot(pl, type='l')
#lines(pl)
time_sec1 <- (data1$V1-data1[1,1])/1000
time_sec2 <- (data2$V1-data2[1,1])/1000
time_sec3 <- (data3$V1-data3[1,1])/1000





par(mar = c(5,4,1,5))
#plot(time_sec1, data1$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4)), xlim=c(0,max(time_sec1)))
#plot(time_sec2, data2$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data2$V4)), xlim=c(0,max(time_sec2)))
#plot(time_sec2, data2$V3, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data2$V3)), xlim=c(0,max(time_sec2)))
plot(time_sec1, data1$V4, type="s", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4, data2$V4, data3$V4)), xlim=c(0,max(time_sec1, time_sec2)), lty=1)
lines(time_sec2, data2$V4, col="blue", type="s")
lines(time_sec3, data3$V4, col="dark green", type="s")
#lines(time_sec1, data1$V4, col="blue")
plot_over(data3$V1, "workload", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0, max(data3$V1)), lty=2)
#plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0,max(wd$intervals[wd$intervals != Inf])), lty=2)
#plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1)), ylim=c(0,max(wd$intervals[wd$intervals != Inf])))
legend("topleft", legend = c("workload", "single-step", "USL"), col=c("black", "red", "blue"), lty=c(2,3,2), bty='n')

legend("topleft", legend = c("USL-short history", "USL"), col=c("red", "blue"), lty=1, bty='n')
legend("topleft", legend = c("USL-pool limit", "USL-avail"), col=c("red", "blue"), lty=1, bty='n')

legend("topright", legend = c("single-step", "USL", "USL-short history"), col=c("red", "blue", "green"), lty=1, bty='n')
legend("topright", legend = c("single-step", "USL", "USL-pool limit"), col=c("green", "blue", "red"), lty=1, bty='n')

#legend(x = 0, y = 5, legend = c("USL", "single-step", "workload"), col=c("blue", "red", "black"), lty=1, bty = 'n')

avg(time_sec1, data1$V4)
avg(time_sec2, data2$V4)
avg(time_sec3, data3$V4)

integral(time_sec1, data1$V4)
integral(time_sec2, data2$V4)
integral(time_sec3, data3$V4)

max(data1$V4)
max(data2$V4)
max(data3$V4)
