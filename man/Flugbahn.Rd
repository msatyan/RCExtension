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
library(slvwagner)
library(tidyverse)

# Ziel
Ziel_Schussebenen <- c(100, 20) # [m]
angel_schussebene <- atan(Ziel_Schussebenen[2]/Ziel_Schussebenen[1]) # [rad]

# Geschoss
v0 = 180 #m/s
m <- 0.440 # [kg]
g <- 9.81 # [m/s^2]
cw <- 0.45; # cw Wert des Geschosses (Nach Tabellen der Geschossform)
rho <- 1.293 # Dichte des Mediums (Luft) [kg/m^3]
A <- (0.220)^2*pi/4 #Projektions Fl?che in Flugrichtung [m^2]
k <- 0.5*cw*rho*A

#Treffergenauigkeit
target_hit_error <- 0.05

#Aufloesung der Flugbahnberechnung
t <-  1/(v0/target_hit_error) # sec
#im c code definiert
t <- 0.0001

# iterationen
iterationen <- floor(v0/slvwagner::math_betrag(Ziel_Schussebenen)/t*10)
iterationen <- 100000

#####################################################################################
c_C <- RCExtension::Flugbahn(v0,target_hit_error,atan(Ziel_Schussebenen[2]/Ziel_Schussebenen[1]),Ziel_Schussebenen,m,k)
c_length <- length(c_C)/4
m_s <- data.frame( s_x = c_C[(2*c_length+1):(3*c_length)],
                   s_y = c_C[(3*c_length+1):(4*c_length)])%>%
  as.matrix()
m_v <- data.frame(v_x = c_C[1:c_length],
                  v_y = c_C[(1*c_length+1):(2*c_length)])%>%
  as.matrix()

# Optimierungsalorithmus
angel_schussebene <- atan(Ziel_Schussebenen[2]/Ziel_Schussebenen[1]) #rad

c_wheight <- 1
cnt <- 1
c_correction <- 0.1

l_iteration <- list()
while(math_betrag(Ziel_Schussebenen-m_s[nrow(m_s),])>target_hit_error){
  # Distanz letzte berechnete Flugbahn
  last_dist <- slvwagner::math_betrag(Ziel_Schussebenen-m_s[nrow(m_s),])
  #Flugbahn
  c_C <- RCExtension::Flugbahn(v0,target_hit_error,angel_schussebene,Ziel_Schussebenen,m,k)
  c_length <- length(c_C)/4
  m_s <- data.frame( s_x = c_C[(2*c_length+1):(3*c_length)],
                     s_y = c_C[(3*c_length+1):(4*c_length)])%>%
    as.matrix()
  m_v <- data.frame(v_x = c_C[1:c_length],
                    v_y = c_C[(1*c_length+1):(2*c_length)])%>%
    as.matrix()

  # Schusswinkel anpassen
  if(Ziel_Schussebenen[1]>m_s[nrow(m_s),1]&Ziel_Schussebenen[2]>m_s[nrow(m_s)-1,2]){
    angel_schussebene <- angel_schussebene + (tan(c_correction)*c_wheight)
  }else if(Ziel_Schussebenen[1]<m_s[nrow(m_s),1]&Ziel_Schussebenen[2]<m_s[nrow(m_s)-1,2]){ #(Schuss über das Ziel)
    angel_schussebene <- angel_schussebene - (tan(1.2*c_correction)*c_wheight)
  }
  # Korrektur Gewicht anpassen um genauer zu werden
  if(last_dist < math_betrag(Ziel_Schussebenen-m_s[nrow(m_s),])){
    c_wheight <- c_wheight*0.5
  }
  #Speichern der Infos zu letzten Flugbahn berechnung
  l_iteration[[cnt]] <- tibble(Distantance_to_Target = math_betrag(Ziel_Schussebenen-m_s[nrow(m_s),]),
                               Winkel=angel_schussebene,
                               c_wheight =  c_wheight)
  # Ausgeben: Wenn sich die Distanz zum Ziel nicht mehr verkleinern laesst.
  if(sum(cnt == seq(1,1e3,1))>0) {
    writeLines(paste("Distantance to Target:",math_betrag(Ziel_Schussebenen-m_s[nrow(m_s),]),
                     "Winkel:",angel_schussebene,
                     "c_wheight:",c_wheight))
  }

  # Abbruch wenn die keinen aenderung mehr möglich ist mit dem verkleinern des Gewichts.
  if(cnt>6){
    if (sum(bind_rows(l_iteration)$c_wheight[(cnt-6):(cnt-1)]==c_wheight)>4){
      writeLines(paste("*******************",
                       "\nGenauerer Berechnung nicht mehr möglich: target_hit_error zu klein.",
                       "\n*******************"))
      break
      }
  }
  # Abbruch wenn das Ziel nicht getroffen werden kann weil der Winkel über 45Grad
  if(angel_schussebene>45/180*pi) {
    writeLines(paste("*******************",
                "\nWinkel > 45DEG",
                "\n*******************"))
    break
  }
  cnt <- cnt + 1
}

c_C <- RCExtension::Flugbahn(v0,target_hit_error,angel_schussebene,Ziel_Schussebenen,m,k)
c_length <- length(c_C)/4
df_c <- data.frame(v_x = c_C[1:c_length],
                   v_y = c_C[(1*c_length+1):(2*c_length)],
                   s_x = c_C[(2*c_length+1):(3*c_length)],
                   s_y = c_C[(3*c_length+1):(4*c_length)])

df <- bind_rows(l_iteration,.id = "cnt")%>%
  mutate(cnt = as.integer(cnt))

df%>%
  #filter(cnt >72)%>%
  ggplot(aes(cnt, Distantance_to_Target))+
  geom_point()+
  geom_line()+
  geom_hline(yintercept = 0)+
  geom_hline(yintercept = target_hit_error, color = "red")

df_c%>%
  slice(seq(1,nrow(df_c), round(1/(300/c_length))))%>%
  ggplot(aes(s_x,s_y))+
  geom_line()+
  coord_fixed()+
  geom_point(aes(x = Ziel_Schussebenen[1], y = Ziel_Schussebenen[2]), color = "red")+
  labs(title = "Flugbahn in C ",
       subtitle = paste("Aufloesung:",t, "s",
                        "\nTarget hit Error", target_hit_error))

}
