#!/usr/bin/Rscript
#setwd("~/github/scalability_experiments/analysis")
source("./USL.R")

args <- commandArgs(TRUE)
file_name <- args[1] #database
sample <- args[2] #samples to use model estimation
file_name <- "./data/step3/10n_5_0.2_1n_10_40000_400.client"
sample <- 13
m <- read.table(file_name, head=FALSE, sep=" ")

by.run <-   by(m, m$V11, identity, simplify=FALSE)
times <- sapply(by.run, function(sample) {max(sample$V9+sample$V10) - min(sample$V10)})
requests <- sapply(by.run, function(sample) {length(sample$V1)})
throughput <- requests / times
x <- sapply(by.run, function(sample) {unique(sample$V5)})

#pdf(file=paste0(file_name, ".pdf"))
estimate <- usl.chart(sample, x, throughput, xlab = "workers")
lines(x, usl.formula(x, m[1,7]/1000000/m[1,4], m[1,8]/1000000/m[1,4]), lty=2)
usl.peak(m[1,7]/1000000/m[1,4], m[1,8]/1000000/m[1,4])
usl.peak(estimate[1], estimate[2])

#dev.off()
