source("./USL.R")

x1 <- 98.91197
contention <- 0.08457
coherency <- 0.002

file_name1 <- './data/step3/day63d.reqs'
reqs <- read.table(file_name1)

pool_var <- usl.inverted(reqs/x1, contention, coherency)
plot(pool_var$V1, type='l', ylim=c(0,23), ylab='pool size', xlab="time(sec)")

head(pool_var, 25)
