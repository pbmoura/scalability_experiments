#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")

file_name1 <- './data/step3/uslFIFA63c-old/day63c.reqs'
file_name1 <- './data/step3/uslFIFA63d/day63d.reqs'

file_name3 <- './data/step3/uslFIFA63d/handling'
file_name4 <- './data/step3/uslFIFA63d/player'

file_name3 <- './data/step3/uslFIFA63c.reqs'
file_name3 <- './data/step3/test.player'
file_name4 <- './data/step3/uslFIFA63c.hand'



data3 <- read.table(file_name3)
data4 <- read.table(file_name4)

reqs <- read.table(file_name1)
plot(reqs$V1, type='l', ylim=c(0,750))
#data4$V1 = data4$V1/1000
wd <- workload(data3)
wp <- workload(data4)
lines(wd, col='blue', type='l')
lines(wp, col='red', type='l')
as.vector(wd[1:min(length(wd), length(wp))])-as.vector(wp[1:min(length(wd), length(wp))])
diff <- as.vector(wd[1:min(length(wd), length(reqs$V1))])-as.vector(reqs$V1[1:min(length(wd), length(reqs$V1))])
diff <- as.vector(wp[1:min(length(wp), length(reqs$V1))])-as.vector(reqs$V1[1:min(length(wp), length(reqs$V1))])
lines(abs(diff), col='red', type='l')
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