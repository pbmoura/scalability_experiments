#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./common.R")


file_name3 <- './data/step3/uslFIFA63c/handling'
file_name4 <- './data/step3/uslFIFA63c-old/handling'



data3 <- read.table(file_name3)
data4 <- read.table(file_name4)

reqs <- read.table('./data/step3/uslFIFA63c-old/day63c.reqs')
plot(reqs$V1, type='l')
wd <- workload(data3)
wp <- workload(data4)
lines(wd, col='blue', type='l')
lines(wp, col='red', type='l')
as.vector(wd[1:min(length(wd), length(wp))])-as.vector(wp[1:min(length(wd), length(wp))])
#plot(wd, type = 'l')
#plot(wp, type = 'l')