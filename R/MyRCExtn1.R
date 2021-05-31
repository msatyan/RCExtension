# Hello, world!
#
# This is an example function named 'hello'
# which prints 'Hello, world!'.
#
# You can learn more about package authoring with RStudio at:
#
#   http://r-pkgs.had.co.nz/
#
# Some useful keyboard shortcuts for package authoring:
#
#   Build and Reload Package:  'Ctrl + Shift + B'
#   Check Package:             'Ctrl + Shift + E'
#   Test Package:              'Ctrl + Shift + T'

# library(RCExtension)
#
.onLoad <- function(lib, package)
{
   library.dynam("RCExtension", package, lib )
}

####################################
# Multiply in c
####################################
Multiply <- function(x, y)
{
  # .C() automatically converts back and forth between R vectors and their C equivalents.
  # The output will be on the third parameter, extract that
  .Call( C_MultiplyVector, as.numeric(x), as.numeric(y))

}

SchussWinkel <- function(v0,target_hit_error,angle_Schussebenen,Ziel_Schussebenen,m,k)
{
  .Call( C_SchussWinkel, as.numeric(v0), as.numeric(target_hit_error), as.numeric(angle_Schussebenen),as.numeric(Ziel_Schussebenen), as.numeric(m),as.numeric(k))
}

