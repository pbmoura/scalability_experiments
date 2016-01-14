#!/usr/bin/Rscript

#setwd("~/github/scalability_experiments/analysis")
source("./USL.R")

args <- commandArgs(TRUE)
file_name <- args[1] 
#file_name <- "./data/1n_5_5_1_4_300_10.client"
m <- read.table(file_name, head=FALSE, sep=" ")

by.run <-   by(m, m$V11, identity, simplify=FALSE)
times <- sapply(by.run, function(sample) {max(sample$V9+sample$V10) - min(sample$V10)})
requests <- sapply(by.run, function(sample) {length(sample$V1)})
throughput <- requests / times

plot(throughput*1000)

n <- which.max(throughput)
throughput[n]
median(m$V9[1:(sum(1:n)*m[1,3])])
