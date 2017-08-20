#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

void R_init_mylib(DllInfo *info)
{
  // Register routines,
  // allocate resources.
}

void R_unload_mylib(DllInfo *info)
{
  // Release resources.
}

