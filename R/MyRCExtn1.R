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
.onLoad <- function(lib, pkg) 
{
   library.dynam("RCExtension", pkg, lib )
}

hello <- function() 
{
  print("Hello, world!")
}


Increment <- function(x) 
{
  # .Call( "Increment" )
  # .Call( getNativeSymbolInfo("Increment") )
  .Call( C_Increment, x )
}


Multiply <- function(x, y) 
{
  # .C() automatically converts back and forth between R vectors and their C equivalents.
  # The output will be on the third parameter, extract that
  .C( C_Multiply, x, y, numeric(1) ) [[3]]
}


Divide <- function(x, y) 
{
  # .C() automatically converts back and forth between R vectors and their C equivalents.
  # The output will be on the third parameter, extract that
  .C( C_Divide,  x, y, numeric(1) ) [[3]]
}


MyPi <- function() 
{
  .Call(C_MyPi)
}


Add <- function(x, y) 
{
  .Call(C_Add, x, y)
}


Subtract <- function(x, y) 
{
  .Call(C_Subtract, x, y)
}


