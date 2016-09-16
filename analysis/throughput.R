file_name1 <- './data/step4/10-pool-lim/departures'
file_name2 <- './data/step4/10/departures'
file_name3 <- './data/step4/10-2/departures'

mav <- function(x,n=5){filter(x,rep(1/n,n), sides=2)}

data1 <- read.table(file_name1)
data2 <- read.table(file_name2)
data3 <- read.table(file_name3)

wd1 <- mav(workload(data1))
wd2 <- mav(workload(data2))
wd3 <- mav(workload(data3))

plot(wd1, type='l', ylim=c(0,650), ylab='req/sec', xlab="time(sec)", col='red')
lines(wd2, col='blue', type='l')
lines(wd3, col='green', type='l')
legend("topright", legend = c("USL-pool limit", "USL-avail", "USL-short avail"), col=c("red", "blue", "green"), lty=1, bty='n')

