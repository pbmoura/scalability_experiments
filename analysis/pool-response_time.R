setwd("~/github/scalability_experiments/analysis")
source("./common.R")

ymax_rt <- 250000
ymax_p <- 20
experiment <- "30-usl"

file_name <- paste0('./data/step4/estimated_start/', experiment, '/monitoring')
file_name1 <- paste0('./data/step4/estimated_start/',experiment, '/results/uslFIFA63d.play')
SLA <- 3000

data1 <- read.table(file_name1)
data <- read.table(file_name)

sla_violation1 <- length(data1$V3[data1$V3>SLA])/length(data1$V3) * 100
time_sec <- (data$V1-data[1,1])/1000
rt_mean <- tapply(data1$V3, data1$V1 %/% 10000, mean)



pdf(file=paste0('~/phd/submissions/journal/latex/figures/trace-', experiment, '.pdf'), width=8.25, height=5.75)
par(mar = c(4,4,1,4))
plot(seq(from=5, by=10, length.out=length(rt_mean)), rt_mean, ylim=c(0,ymax_rt), xlab="time (sec)", ylab="response time (ms)", type='l')
abline(h=SLA, lty=3)
plot_over(data.frame(time_sec, data$V4), "pool size", type="s", ylim=c(0, ymax_p), lty=2)
legend("topleft", legend = c("response time", "SLA", "pool size"), lty=c(1, 3, 2), bty='n')
dev.off()

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
