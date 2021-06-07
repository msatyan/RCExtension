\name{Flugbahn}
\alias{Flugbahn}
\title{Balistische Flugbahn berechnen}
\usage{
Flugbahn()
}
\description{
Berechnet die Flugbahn eines Geschosses.
}


\examples{
m <- RCExtension::Flugbahn(50,0.001,45/180*pi,c(100,-20),0.444,0.004423604)
m <- as.matrix(m,ncol = 5)
colnames(m2) <- c("v_x1","v_x2","s_x1","s_x2","t")
head(m)
tail(m)
}
