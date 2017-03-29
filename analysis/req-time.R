#!/usr/bin/Rscript
setwd("~/scalability_experiments/analysis")
source("./common.R")
args <- commandArgs(TRUE)
file_name3 <- args[1] 

#file_name3 <- '~/scratch//handling'



data3 <- read.table(file_name3)

wd <- workload(data3)
length(wd)
wd[1:100]
