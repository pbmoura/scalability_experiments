#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
SLA <- args[3]
#file_name1 <- "./data/step2/load2_30.play"
#file_name2 <- "./data/step2/elastic2_30.play"
#SLA <- 9000
data1 <- read.table(file_name1)
data2 <- read.table(file_name2)

data <- rbind(data1$V3, data2$V3)

barplot(data, beside=TRUE, legend=c(paste0("single-step ",length(data1$V3[data1$V3>SLA])) , paste0("USL ",length(data2$V3[data2$V3>SLA])) ), xlab="requests", ylab="response time", border=NA, col=c("red", "blue"))
abline(h=SLA)

