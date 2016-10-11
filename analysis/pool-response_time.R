setwd("~/github/scalability_experiments/analysis")
source("./common.R")


experiment <- "10-single"

file_name <- paste0('./data/step4/estimated_start/', experiment, '/monitoring')
file_name1 <- paste0('./data/step4/estimated_start/',experiment, '/results/singleFIFA63d.play')
SLA <- 3000

data1 <- read.table(file_name1)
data <- read.table(file_name)

sla_violation1 <- round(length(data1$V3[data1$V3>SLA])/length(data1$V3) * 100)
time_sec <- (data$V1-data[1,1])/1000
rt_mean <- tapply(data1$V3, data1$V1 %/% 1000, mean)



pdf(file=paste0('~/phd/submissions/journal/latex/figures/trace-', experiment, '.pdf'), width=8.25, height=5.75)
par(mar = c(4,4,1,4))
plot(rt_mean, ylim=c(0,50000), xlab="time (sec)", ylab="response time (ms)", pch='.')
abline(h=SLA, lty=3)

plot_over(data.frame(time_sec, data$V4), "pool size", type="s", ylim=c(0, 40), lty=2)

legend("topleft", legend = c("response time", "SLA", "pool size"), lty=c(1, 3, 2), bty='n')

dev.off()
