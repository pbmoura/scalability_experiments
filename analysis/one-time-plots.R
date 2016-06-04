setwd("~/github/scalability_experiments/analysis/")
source("./USL.R")

# SLA violations
data <- rbind(c(0, 5, 17, 66, 72, 70, 80, 83, 85, 85, 88), c(11, 2, 1, 4, 6, 13, 13, 22, 66, 27, 71))
barplot(data, beside=TRUE, xlab="intervals", ylab="SLA violations (%)", border=NA, col=c("red", "blue"))
legend("topleft", legend=c("single-step" , "USL"), col=c("red", "blue"), pch=15)


# USL examples
plot(usl.formula((1:30), 0.2, 0), type='l', col='red', xlab = "# parallel units (n)", ylab="Capacity (C)")
lines(usl.formula((1:30), 0.2, 0.003), type='l', col='blue', lty=2)
lines(usl.formula((1:50), 0, 0), type='l', lty=4)
legend(x = 20, y=2, legend = c("a=0, b=0", "a=0.2, b=0", "a=0.2, b=0.003"), col=c("black","red","blue"), lty = c(4, 1, 2), bty = "n")
abline(h=usl.formula(trunc(usl.peak(0.2, 0.003)), 0.2, 0.003), lty=3)
segments(trunc(usl.peak(0.2, 0.003)), 0, trunc(usl.peak(0.2, 0.003)), usl.formula(trunc(usl.peak(0.2, 0.003)), 0.2, 0.003), lty=3)
points(trunc(usl.peak(0.2, 0.003)), usl.formula(trunc(usl.peak(0.2, 0.003)), 0.2, 0.003), pch=4)



abline(h=usl.formula(usl.peak(0.2, 0.003), 0.2, 0.003), lty=3)
segments(usl.peak(0.2, 0.003), 0, usl.peak(0.2, 0.003), usl.formula(usl.peak(0.2, 0.003), 0.2, 0.003), lty=3)
points(usl.peak(0.2, 0.003), usl.formula(usl.peak(0.2, 0.003), 0.2, 0.003), pch=4)

# inverted USL example
plot(seq(1,4,0.00001), usl.inverted(seq(1,4,0.00001), 0.2, 0.003), type='l', col='red', xlab = "Capacity (C)", ylab="# parallel units (n)", ylim=c(0,30))
lines(seq(1,4,0.00001), usl.invertedp(seq(1,4,0.00001), 0.2, 0.003), type='l', col='blue', lty=2)
abline(h=usl.peak(0.2, 0.003))


# time to stabilization indexes identified by exploring data1 and data2
time_sec2[119]
abline(v=60.012)

time_sec2[851]
abline(v=154.134-60.012)

time_sec1[117]
abline(v=59.512)

time_sec1[1640]
abline(v=253.267-59.512)
