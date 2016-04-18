#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")
source("./USL.R")

args <- commandArgs(TRUE)
file_name <- args[1] 
#file_name <- "./data/step3/1n_5_0.2_1_20_800_300.client"
m <- read.table(file_name, head=FALSE, sep=" ")

by.run <-   by(m, m$V11, identity, simplify=FALSE)
times <- sapply(by.run, function(sample) {max(sample$V9+sample$V10) - min(sample$V10)})
requests <- sapply(by.run, function(sample) {length(sample$V1)})
throughput <- requests / (times/1000)

plot(throughput)

n <- which.max(throughput)
n<-20
throughput[n]
median(m$V9[1:(sum(1:n)*m[1,3])])





the_run <- data.frame(by.run[20])
the_run[order(the_run[,10]),]

