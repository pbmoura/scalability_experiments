#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name1 <- args[1]    
file_name2 <- args[2]    
SLA <- args[3]

#file_name1 <- "./data/step2/single1_13.play"
#file_name2 <- "./data/step2/elastic1_13.play"

#SLA <- 8000
data1 <- read.table(file_name1)
data2 <- read.table(file_name2)

data <- rbind(data1$V3, data2$V3)

sla_violation1 <- round(length(data1$V3[data1$V3>SLA])/length(data1$V3) * 100)
sla_violation2 <- round(length(data2$V3[data2$V3>SLA])/length(data2$V3) * 100)

barplot(data, beside=TRUE, legend=c(paste0("single-step ", sla_violation1, "%") , paste0("USL ", sla_violation2, "%" )), xlab="requests", ylab="response time (ms)", border=NA, col=c("red", "blue"))
abline(h=SLA)

sum(data1$V3)/length(data1$V3)
sum(data2$V3)/length(data2$V3)

#barplot(data2$V3, beside=TRUE, , xlab="requests", ylab="response time (ms)", border=NA)
