source("./USL.R")
service_time_for <- function(s1, a, b) {function(n) {s1 + s1 * a * (n-1) + s1 * b * n * (n-1) } }
arrival_rate <- function(load, st) {load/st}
estimate_for <- function(a, b, x1) {
  function(x) {
    max_w <- round(usl.peak(a, b))
    c <- x/x1
    if (c >= usl.formula(max_w, a, b)) {
      return(max_w)
    } else {
      delta <- (a - b - 1/c)^2 - 4 * b * (1-a)
      return( (b - a + 1/c - sqrt(delta)) / (2*b) )
    }
  } 
} 
usl.inverted <- function(xs, sigma, kappa) return( (-sigma + kappa +1/xs - sqrt(  (sigma - kappa - 1/xs)^2 - 4*kappa*(1-sigma) ))/(2*kappa) )
args <- commandArgs(TRUE)
file_name <- args[1]    
#file_name <- "./data/step2/elastic2_30.mon"
data <- read.table(file_name)


time_sec <- (data$V1-data[1,1])/1000


service_time <- service_time_for(5.002, 0.0398, 0.0031)
estimate <- estimate_for(0.0398, 0.0031, 0.769)

data$V5 <- service_time(data$V4)
data$V6 <- arrival_rate(data$V3, data$V5)

data$V7 <- sapply(data$V6, estimate)


plot(time_sec, data$V5, ylim = c(0, max(data$V5, data$V6, data$V7, na.rm = TRUE)), type='l')
lines(time_sec, data$V6, lty=2)
lines(time_sec, round(data$V7), lty=3)
legend(x=0, y=max(data$V5, data$V6, data$V7, na.rm = TRUE), legend = c("service time", "arrival rate", "num. of workers"), lty=c(1,2,3))





