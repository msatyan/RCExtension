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
.onLoad <- function(lib, pkg) {
   library.dynam("RCExtension", pkg, lib )
}

hello <- function() {
  print("Hello, world!")
}


Increment <- function(a) {
  #.Call( "Increment," )
  # .Call( getNativeSymbolInfo("Increment,") )
  .Call( C_Increment, a )
}


# Multiply <- function(a, b) {
#   .C(C_Multiply, as.double(a), as.double(b))
# }

# Divide <- function(a, b) {
#   .C(C_Divide, a, b)
# }


MyPi <- function() {
  .Call(C_MyPi)
}

Add <- function(a, b) {
  .Call(C_Add, a, b)
}

Subtract <- function(a, b) {
  .Call(C_Subtract, a, b)
}


