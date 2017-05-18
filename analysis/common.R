



workload <- function(sample) {
  arrivals <- sample[,1]
  
  residual = arrivals[1] %% 1000
  arrivals = arrivals - residual
  
  table(trunc((arrivals-arrivals[1])/1000))
}


plot_over <- function(data, ylab, s=4, line_num=0, ...) {
  par(new = TRUE)
  plot(data, axes=F, xlab=NA, ylab=NA, ...)
  axis(side = s, line = line_num)
  mtext(side = s, line = line_num+3, ylab)
}

integral <- function(x, y) {
  library(zoo)
  sum(diff(x)*rollmean(y,2))
}

avg <- function(x, y) {
  integral(x, y)/max(x)
}

moving_avg <- function(x,n=5){filter(x,rep(1/n,n), sides=2)}