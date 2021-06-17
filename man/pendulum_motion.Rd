\name{pendulum_motion}
\alias{pendulum_motion}
\title{Pedelbewegung theta(t)}
\usage{
pendulum_motion( t,  L,  delta_t,  THETA_0,  THETA_DOT_0,  mu)
}
\description{
Caluclating pendulum motion including  friction in repect to time
}

\arguments{
\item{t}{end time of calculation [s]}

\item{L}{Pendelum lenght [m]}

\item{delta_t}{calculation time inerval [s]}

\item{THETA_0}{start angel [rad]}

\item{THETA_DOT_0}{start angular velocity [m/s]}

\item{mu}{friction}
}

\value{
Returns matrix with theta and the angular acceleration \code{x}
}

\examples{
delta_t <- 1e-6
system.time(penulum_get_theta(50, 2,  delta_t,   pi/3,   0,  0.1)|>
              plot(type = "l",main = paste("calculation time intervall [s]",delta_t)))
}
