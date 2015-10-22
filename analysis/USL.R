usl.prepare <- function (p,X) {
  C <- X/X[1]
  pC <- p/C
  
  x <- p-1
  y <- pC-1
  return(data.frame(x,y))
}

usl.peak <- function (contention, coherency) sqrt((1-contention)/coherency)

usl.inference <- function (p, X) {
  dataset <- usl.prepare(p, X)
  x <- dataset$x
  y <- dataset$y
  reg <- lm(y ~ 0 + x + I(x^2))
  a <- reg$coefficients[2]
  b <- reg$coefficients[1]
  
  sigma = b-a
  kappa = a
  peak = usl.peak(sigma,kappa)
  
  return(c(sigma, kappa, summary(reg)$r.squared, peak))
}



usl.plot <- function(n, estimated, measured, ...) {
  max = length(throughput)
  plot(n, measured, ylab = "capacity", ...)
  lines(estimated)
}

usl.formula <- function(xs, sigma, kappa) return(xs/(1+sigma*(xs-1)+xs*kappa*(xs-1)))

usl.chart <- function(limit, n, throughput, ...) {
  if (length(limit) == 1)
    ii <- c(1:limit)
  else
    ii <- limit
  infer <- usl.inference(n[ii], throughput[ii])
  estimated <- usl.formula((1:max(n)), infer[1], infer[2])
  capacity <- throughput/throughput[1]
  usl.plot(n, estimated, capacity, ...)
  print(usl.error(estimated, capacity, n))
  abline(v=limit)
  infer
}

usl.error <- function(estimated, measured, n) {
  abs(estimated[n] - measured[n])
}

usl.line <- function(limit, throughput, ...) {
  ii <- c(1:limit)
  infer <- usl.inference(n[ii], throughput[ii])
  lines(usl.formula((1:30), infer[1], infer[2]), ...)
  abline(v=limit, ...)
  infer
}