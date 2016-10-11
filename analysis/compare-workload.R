#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

file_name1 <- './data/step4/day63d.reqs'

file_name3 <- './data/step3/singleFIFA63d/handling'
file_name3 <- './data/step3/uslFIFA63d/1/handling'
file_name3 <- './data/step4/10-2-pool-lim/handling'
file_name3 <- './data/step4/10-pool-lim/handling'
file_name3 <- './data/step4/30-6-pool-lim/handling'
file_name3 <- './data/step4/estimated_start/10-pool-lim/handling2'
file_name3 <- './data/step4/estimated_start/10-2-avail/handling'
file_name3 <- './data/step4/estimated_start/30-usl/handling'
file_name3 <- './data/step4/estimated_start/single-10/handling'
file_name3 <- './data/step4/single/10/2/handling'

file_name4 <- './data/step3/uslFIFA53c/player'

file_name3 <- './data/step3/uslFIFA63c.reqs'
file_name3 <- './data/step3/test.player'
file_name4 <- './data/step3/uslFIFA63c.hand'


#file_name1 <- './data/step4/10-pool-lim/departures'
#file_name2 <- './data/step4/10/departures'
#file_name3 <- './data/step4/10-2/departures'


reqs <- read.table(file_name1)
plot(reqs$V1, type='l', ylim=c(0,750), ylab='req/sec', xlab="time(sec)")
#plot_over(reqs$V1, type='l', ylim=c(0,750), ylab='req/sec')


data3 <- read.table(file_name3)
wd <- workload(data3)
lines(wd, col='blue', type='l')
diff <- as.vector(wd[1:min(length(wd), length(reqs$V1))])-as.vector(reqs$V1[1:min(length(wd), length(reqs$V1))])
lines(abs(diff), col='green', type='l')

data4 <- read.table(file_name4)

#data4$V1 = data4$V1/1000
wp <- workload(data4)
lines(wp, col='red', type='l')
as.vector(wd[1:min(length(wd), length(wp))])-as.vector(wp[1:min(length(wd), length(wp))])
diff <- as.vector(wp[1:min(length(wp), length(reqs$V1))])-as.vector(reqs$V1[1:min(length(wp), length(reqs$V1))])
  max(abs(diff[-length(diff)]))
#plot(wd, type = 'l')
#plot(wp, type = 'l')

wd.frame <- as.data.frame(wd)
for(i in 1:length(reqs$V1)) {
  if(length(wd.frame[wd.frame$Var1==i,2]) > 0) 
    diff[i] = reqs$V1[i] - wd.frame[wd.frame$Var1==i,2]
  else
    diff[i] = reqs$V1[i]
}


cumulative <- reqs$V1
for (ii in 2:length(cumulative)) {
  cumulative[ii] = cumulative[ii] + cumulative[ii-1]
}
plot(cumulative)
abline(h=3815082)
abline(h=3815260)
abline(v=which(cumulative >= 3815793)[1])
lines(reqs$V1, col='blue')


cumulative[8991]

( m <- matrix(1:12, 3, 4) )
div.3 <- m %% 3 == 0
which(div.3)
which(div.3, arr.ind = TRUE)
rownames(m) <- paste("Case", 1:3, sep = "_")
which(m %% 5 == 0, arr.ind = TRUE)