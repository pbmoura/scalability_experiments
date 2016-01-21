


workload <- function(sample) {
  arrivals <- sample[,1]
  len <- length(arrivals)
  intervals <- c(arrivals[2:len] - arrivals[1:len-1], 0)
  time_sec <- (arrivals-arrivals[1])/1000
  s_frame <- data.frame(time_sec, 1000/intervals)
}

plot_over <- function(data, ylab, ...) {
  par(new = T)
  plot(data, axes=F, xlab=NA, ylab=NA, ...)
  axis(side = 4)
  mtext(side = 4, line = 3, ylab)
}