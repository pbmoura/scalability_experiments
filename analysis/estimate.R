#!/usr/bin/Rscript
source("./USL.R")

#setwd("~/github/scalability_experiments/analysis")

args <- commandArgs(TRUE)
file_name <- args[1] #
#file_name <- "./data/1n_5_5000_1n_250_10.client"
sample <- args[2]
#sample <- 8
m <- read.table(file_name, head=FALSE, sep=" ")

by.run <-   by(m, m$V1, identity, simplify=FALSE)
times <- sapply(by.run, function(sample) {max(sample$V9+sample$V10) - min(sample$V10)})
requests <- sapply(by.run, function(sample) {length(sample$V1)})
throughput <- requests / times
x <- sapply(by.run, function(sample) {unique(sample$V6)}) 

pdf(file=paste0(file_name, ".pdf"))
usl.chart(sample, x, throughput, xlab = "workers")
#usl.chart(15, x, throughput, xlab = "workers")
lines(x, usl.formula(x, m[1,7]/m[1,5], m[1,8]/m[1,5]), lty=2)
usl.peak(m[1,7]/m[1,5], m[1,8]/m[1,5])
dev.off()
