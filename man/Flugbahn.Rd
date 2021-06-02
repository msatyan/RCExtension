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
# Maximale Flugdistanz

library(slvwagner)
library(tidyverse)


# # Ziel
Ziel_Schussebenen <- c(105, 0) #m
angel_schussebene <- atan(Ziel_Schussebenen[2]/Ziel_Schussebenen[1]) #rad
# Geschoss
v0 = 35
# [m/s]
m <- 0.440 # [kg]
g <- 9.81 # [m/s^2]
cw <- 0.45; # cw Wert des Geschosses (Nach Tabellen der Geschossform)
rho <- 1.293 # Dichte des Mediums (Luft) [kg/m^3]
A <- (0.22)^2*pi/4 #Projektions Flaeche in Flugrichtung [m^2]
k <- 0.5*cw*rho*A

#Treffergenauigkeit
target_hit_error <- 0.05 # [m]

#Aufloesung der Flugbahnberechnung
t <-  1/(v0/target_hit_error) # [sec]
#im c code definiert
t <- 0.0001 # [sec]


##################################################################################
# max throw distance
##################################################################################
l_throws <- list()
l_maxDist <- list()
#Verschieden Winkel
c_rad <- atan(seq(0.4,1,0.01))

for (i in 1:length(c_rad)) {
  l_throws[[i]] <- RCExtension::Flugbahn(50,target_hit_error,c_rad[i],c(105,0),m,k)%>%
    matrix(ncol = 5)
  colnames(l_throws[[i]]) <- c("v_x1","v_x2","s_x1","s_x2","t")
  l_throws[[i]] <- l_throws[[i]]%>%
    as_tibble()%>%filter(s_x2 >= 0)
  l_maxDist[[i]] <- l_throws[[i]]%>%
    slice(nrow(l_throws[[i]]))
}
names(l_throws) <- c_rad
df <- bind_rows(l_throws, .id = "Winkel")

names(l_maxDist) <- c_rad
df_max <- bind_rows(l_maxDist, .id = "Winkel")%>%
  mutate(Winkel = parse_double(Winkel)/pi*180)

s_df_max <- df_max%>%
  filter(s_x1 == max(s_x1))

df_max%>%
  ggplot(aes(Winkel, s_x1))+
  geom_line()+
  geom_hline(data = s_df_max,aes(yintercept = s_x1))+
  geom_vline(data = s_df_max,aes(xintercept = Winkel))

df_plot <- df%>%
  filter(t %in% seq(0,max(df$t), 0.1),
         Winkel %in% atan(seq(0.4,1,0.05)))%>%
  mutate(Winkel = parse_double(Winkel),
         Winkel = round(Winkel/pi*180,4),
         Winkel = factor(Winkel))
df_plot%>%
  ggplot(aes(s_x1,s_x2, color = Winkel))+
  geom_line()+
  coord_fixed()+
  geom_vline(data = s_df_max,aes(xintercept = s_x1))+
  geom_hline(aes(yintercept = max(s_x2)),linetype = 2)+
  geom_hline(aes(yintercept = 0))+
  labs(title = "Flugbahn in C ",
       subtitle = paste("Aufloesung:",t, "s",
                        "\nTarget hit Error", target_hit_error))
