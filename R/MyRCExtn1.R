# library(RCExtension)
# R function wrapers to call C Functions
.onLoad <- function(lib, package)
{
   library.dynam("RCExtension", package, lib )
}

####################################
# Flugbahn in c

Flugbahn <- function(v0,t,angle_Schussebenen,Ziel_Schussebenen,m,k)
{
  .Call( C_Flugbahn, as.numeric(v0), as.numeric(t), as.numeric(angle_Schussebenen),as.numeric(Ziel_Schussebenen), as.numeric(m),as.numeric(k))
}

Multiply <- function(x,y)
{
  .C( C_Multiply, x, y, numeric(1) ) [[3]]
}
