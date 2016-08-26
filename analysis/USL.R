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
  #max = length(throughput)
  plot(n, measured, ylab = "capacity", ...)
  lines(estimated)
}

usl.formula <- function(xs, sigma, kappa) return(xs/(1+sigma*(xs-1)+xs*kappa*(xs-1)))

usl.inverted <- function(xs, sigma, kappa) return( (-sigma + kappa +1/xs - sqrt(  (sigma - kappa - 1/xs)^2 - 4*kappa*(1-sigma) ))/(2*kappa) )
usl.invertedp <- function(xs, sigma, kappa) return( (-sigma + kappa +1/xs + sqrt(  (sigma - kappa - 1/xs)^2 - 4*kappa*(1-sigma) ))/(2*kappa) )
#usl.inverted <- function(xs, sigma, kappa) return(-sqrt( (-1/kappa) - (sigma/-kappa) + ((1+xs*(kappa-sigma))^2/(4*(-xs*kappa)^2)) ) - (1 - xs*(sigma + kappa) )/(-2*xs*kappa))
#usl.invertedp <- function(xs, sigma, kappa) return(sqrt( (-1/kappa) - (sigma/-kappa) + ((1+xs*(kappa-sigma))^2/(4*(-xs*kappa)^2)) ) - (1 - xs*(sigma + kappa) )/(-2*xs*kappa))


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

usl.line <- function(limit, n, throughput, ...) {
  ii <- c(1:limit)
  infer <- usl.inference(n[ii], throughput[ii])
  lines(usl.formula((1:30), infer[1], infer[2]), ...)
  abline(v=limit, ...)
  infer
}

max.n.func <- function(a, b, t1) {
  function(sla) {
    ( t1*a - t1*b - sla - (sla + t1*a + t1*b) - sqrt(2*sla*t1*b - 2*sla*t1*a - 2*t1^2*a*b - 4*t1^2*b*(1-a)) )/2*t1*b
  }
}

max.n.func <- function(a, b, t) {
  function(sla) {
    (sqrt(t^2 * (a^2 + 2*a*b + (b-4)*b) + 2*sla*t*(b-a) + sla^2) - a*t +b*t + sla)/(2*b*t)
  }
}

service.time.func <- function(a, b, t1) {
  function(n) {
    t1*(a + (1-a)/n +b*(n-1))
  }
}
  