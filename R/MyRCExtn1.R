# library(RCExtension)
# R function wrapers to call C Functions
.onLoad <- function(lib, package)
{
   library.dynam("RCExtension", package, lib )
}

####################################
# Flugbahn eines Geschosses in C
Flugbahn <- function(v0,t=1e-5,angle,Ziel_Schussebenen,m,k)
{
  if(Ziel_Schussebenen[1]<0){
    Ziel_Schussebenen[1] <- -Ziel_Schussebenen[1]
    m_result <- .Call( C_Flugbahn, as.numeric(v0), as.numeric(t), as.numeric(-angle),as.numeric(Ziel_Schussebenen), as.numeric(m),as.numeric(k))|>
      matrix(ncol = 5)
    colnames(m_result) <- c("v_x1","v_x2","s_x1","s_x2","t")
    m_result[,1] <- -m_result[,1]
    m_result[,3] <- -m_result[,3]
  }else{
    m_result <- .Call( C_Flugbahn, as.numeric(v0), as.numeric(t), as.numeric(angle),as.numeric(Ziel_Schussebenen), as.numeric(m),as.numeric(k))|>
      matrix(ncol = 5)
    colnames(m_result) <- c("v_x1","v_x2","s_x1","s_x2","t")
    m_result[,1] <- m_result[,1]
    m_result[,3] <- m_result[,3]
  }
  return(m_result)
}

Multiply <- function(x,y)
{
  .C( C_Multiply, x, y, numeric(1) ) [[3]]
}

foo <- function (x)
{
  .Call(C_foo, as.numeric(x), as.integer(length(x)))
}
####################################
# Pendelbewegung in C
pendulum_motion <- function ( t,  L,  delta_t,  THETA_0,  THETA_DOT_0,  mu, type = "theta")
{
  m_result <-.Call(C_pendulum_motion, as.numeric(t), as.numeric(L), as.numeric(delta_t),
                   as.numeric(THETA_0), as.numeric(THETA_DOT_0),  as.numeric(mu),
                   as.character(type))|>
    matrix(ncol = 3)
  colnames(m_result) <- c("t","angle","acceleration")
  return(m_result)
}
