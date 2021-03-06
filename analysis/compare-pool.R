#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
file_name3 <- args[3]

#file_name1 <- './data/step2/single5b_20.ad'
#file_name2 <- './data/step2/elastic5b_20.ad'
#file_name3 <- './data/step4/synthetic/usl5b.ad'


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

#file_name1 <- './data/step4/estimated_start/30-6-usl/monitoring'
#file_name2 <- './data/step4/estimated_start/30-pool-lim/monitoring'
#file_name3 <- './data/step4/estimated_start/30-6-pool-lim/monitoring'

#file_name1 <- './data/step4/estimated_start/10-single/monitoring'
#file_name2 <- './data/step4/estimated_start/10-usl/monitoring'
#file_name3 <- './data/step4/estimated_start/10-pool-lim/monitoring'

#file_name1 <- './data/step4/30-6-pool-lim/monitoring'
#file_name2 <- './data/step4/estimated_start/30-6-pool-lim/monitoring'

#file_name1 <- './data/step4/estimated_start/30-pool-lim/monitoring'
#file_name2 <- './data/step4/estimated_start/30-6-pool-lim/monitoring'


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





par(mar = c(4,4,1,4), lwd=2)
#plot(time_sec1, data1$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4)), xlim=c(0,max(time_sec1)))
#plot(time_sec2, data2$V4, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data2$V4)), xlim=c(0,max(time_sec2)))
#plot(time_sec2, data2$V3, type="l", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data2$V3)), xlim=c(0,max(time_sec2)))
#plot(time_sec1, data1$V4, type="s", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4, data2$V4)), xlim=c(0,max(time_sec1, time_sec2)), lty=1)
plot(time_sec1, data1$V4, type="s", col="red", xlab="time (sec.)", ylab="pool size", ylim=c(0,max(data1$V4, data2$V4, data3$V4)), xlim=c(0,min(max(time_sec1), max(time_sec2), max(time_sec3))), lty=2)
lines(time_sec2, data2$V4, col="blue", type="s", lty=3)
lines(time_sec3, data3$V4, col="green", type="s", lty=4)
#lines(time_sec1, data1$V4, col="blue")

plot_over(seq(0,by=60,length.out=7), c(1,4,1,3,5,1,1), type='s', xlim=c(0,min(max(time_sec1), max(time_sec2), max(time_sec3))), ylim=c(0,5), ylab='workload (requests/s)', lty=1)
plot_over(seq(0,by=60,length.out=8), c(1,4,4,4,4,1,1,1), type='s', xlim=c(0,min(max(time_sec1), max(time_sec2), max(time_sec3))), ylim=c(0,5), ylab='workload (requests/s)', lty=1)

#plot(seq(10,by=60,length.out=7), c(1,4,4,4,4,1,1), type='s', ylim=c(0,5), ylab='workload (requests/s)')

#plot_over(data3$V1, "workload", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0, max(data3$V1)), lty=2)
#plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1, time_sec2)), ylim=c(0,max(wd$intervals[wd$intervals != Inf])), lty=2)
#plot_over(wd, "workload", type="l", xlim=c(0,max(time_sec1)), ylim=c(0,max(wd$intervals[wd$intervals != Inf])))
#legend("topleft", legend = c("workload", "single-step", "USL"), col=c("black", "red", "blue"), lty=c(2,3,2), bty='n')

#legend("topleft", legend = c("USL-short history", "USL"), col=c("red", "blue"), lty=1, bty='n')
#legend("topleft", legend = c("USL-pool limit", "USL-avail"), col=c("red", "blue"), lty=1, bty='n')

#legend("topright", legend = c("single-step", "USL", "USL-limit", "workload"), col=c("red", "blue", "dark green", "black"), lty=c(2,3,4,1), bty='n')
legend("topleft", legend = c("single-step", "USL", "USL-limit", "workload"), col=c("red", "blue", "green", "black"), lty=c(2,3,4,1), bty='n')

#legend(y=5.2, x=250, legend = c("single-step", "USL", "USL-limit", "workload"), col=c("red", "blue", "dark green", "black"), lty=1, bty='n')


#legend("topright", legend = c("single-step", "USL", "USL-pool limit"), col=c("green", "blue", "red"), lty=1, bty='n')

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

#plot(seq(0,by=60,length.out=7), c(1,4,1,3,5,1,1), type='s', ylim=c(0,5), xlab='time', ylab='requests/s')

file_name <- './data/step4/estimated_start/10-pool-lim/monitoring'
data <- read.table(file_name)
time_sec <- (data$V1-data[1,1])/1000
pdf(file="~/phd/submissions/journal/latex/figures/pool-10-pool-lim.pdf")
plot(time_sec, data$V4, type="s", xlab="time (sec.)", ylab="pool size", ylim=c(0, 40))
dev.off()