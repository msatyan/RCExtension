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
\item{t}{time range , calculation from 0 to t }

\item{L}{Pendelum lenght}

\item{delta_t}{calculation time inerval}

\item{THETA_0}{start angel [rad]}

\item{THETA_DOT_0}{start angular velocity [rad/delta_t]}

\item{mu}{friction}
}

\value{
Returns matrix with theta and the angular velocity \code{x}
}

\examples{
delta_t <- 1e-6
system.time(pendulum_motion(50, 2,  delta_t,   pi/3,   0,  0.1)|>
              plot(type = "l",main = paste("time interval [s]",delta_t)))
}
