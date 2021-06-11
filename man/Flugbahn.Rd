\name{Flugbahn}
\alias{Flugbahn}
\title{Balistische Flugbahn berechnen}
\usage{
Flugbahn(v0,angle,Ziel_Schussebenen, m, k)
}
\description{
Berechnet die Flugbahn eines Geschosses.
}
\details{
Es wird die Balistische Flugbahn eines Geschosses berechnet mit der Berücksichtigung des Luftwiderstanes.
Das ist mit iterativen Ansatz gelösst. Die neu Geschwindigkeit und Fluglage wird pro Zeitinterval berechnet.
Um die genauigkeit

Aus dem cw-Wert, Dichte des Mediums und er Projektionsfläche des Geschosses kann der k-Wert errechnet werden um den Luftwiderstand zu berücksichtigen.

}

\arguments{
\item{v0}{Geschosss-Geschwindigkeit}

\item{t}{Zeit interval der Berechnung}

\item{angle}{Schusswinkel}

\item{Ziel_Schussebenen}{target vector}

\item{m}{Masse des Geschosses}

\item{k}{k-Wert = 0.5*cw*rho*A}
}

\examples{
library(RCExtension)
library(tidyverse)

# Ziel
Ziel_Schussebenen <- c(100, 20) # m
angle <- atan(Ziel_Schussebenen[2]/Ziel_Schussebenen[1]) #rad

# Zeitintervall
t <- 1e-5

# Geschoss
v0 = 55 # [m/s]
m <- 0.440 # [kg]
cw <- 0.18; # cw Wert des Geschosses (Nach Tabellen der Geschossform)
rho <- 1.293 # Dichte des Mediums (Luft) [kg/m^3]
A <- (0.22)^2*pi/4 #Projektions Flaeche in Flugrichtung [m^2]
k <- 0.5*cw*rho*A
#####################################################################################
# Initale Flugbahn
#####################################################################################
m_flugbahn <- Flugbahn(v0,t,angle,Ziel_Schussebenen,m,k)

m_flugbahn%>%
  as_tibble()%>%
  ggplot(aes(s_x1,s_x2))+
  geom_line()+
  geom_point(data = tibble(x = Ziel_Schussebenen[1],y = Ziel_Schussebenen[2]),aes(x,y), color = "red")+
  coord_fixed()+
  labs(title = "Flugbahn in C",
       subtitle = paste("v0:",v0,"\nt:",t))
}
