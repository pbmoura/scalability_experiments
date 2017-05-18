setwd("~/github/scalability_experiments/analysis")
source("./common.R")

file_name1 <- './data/step4/day63d.reqs'
reqs <- read.table(file_name1)
plot(reqs$V1, type='l', ylim=c(0,750), ylab='req/sec', xlab="time(sec)",col="gray")

ymax_rt <- 30000
ymax_p <- 20
experiment <- "30-pool-lim"

file_name <- paste0('./data/step4/estimated_start/', experiment, '/monitoring')
file_name1 <- paste0('./data/step4/estimated_start/',experiment, '/results/uslFIFA63d.play')
SLA <- 3000

data1 <- read.table(file_name1)
data <- read.table(file_name)

#sla_violation1 <- length(data1$V3[data1$V3>SLA])/length(data1$V3) * 100
time_sec <- (data$V1-data[1,1])/1000
rt_mean <- tapply(data1$V3, data1$V1 %/% 10000, mean)



#pdf(file=paste0('~/phd/submissions/journal/latex/figures/trace-', experiment, '.pdf'), width=8.25, height=5.75)
par(mar = c(4,4,1,4), lwd=2)
plot(reqs$V1, type='l', ylim=c(0,750), ylab=NA, xlab="time (sec)",col="gray", xlim=c(0,length(reqs$V1)), yaxt="n")
plot_over(data.frame(seq(from=5, by=10, length.out=length(rt_mean)), rt_mean), "response time (ms)", s=2, ylim=c(0,ymax_rt), type='l', xlim=c(0,length(reqs$V1)))
abline(h=SLA, lty=3)
#plot_over(data.frame(time_sec, data$V4), "pool size", 4, type="s", ylim=c(0, ymax_p), lty=2, xlim=c(0,length(reqs$V1)))
plot_over(rbind(data.frame(time_sec, data$V4), c(length(reqs$V1), 4)), "pool size", 4, type="s", ylim=c(0, ymax_p), lty=2, xlim=c(0,length(reqs$V1)))
legend("topleft", legend = c("request rate", "response time", "SLA", "pool size"), lty=c(1, 1, 3, 2), col=c("gray", "black", "black", "black"), bty='n')
#dev.off()

#pool
avg(time_sec, data$V4)
max(data$V4)
#queue
avg(time_sec, data$V3)
max(data$V3)
#cost
integral(time_sec, data$V4)
#response time
mean(data1$V3)
sd(data1$V3)
max(data1$V3)
sla_violation1
