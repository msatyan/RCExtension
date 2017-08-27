#include <stdio.h>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


SEXP Add(SEXP x, SEXP y);
SEXP Subtract(SEXP x, SEXP y);
SEXP Increment(SEXP x);
SEXP MyPi();

/////////// .C interface style ////////////////
// void MyFunc1(double *x, int *n, char **names, int *status);
// static R_NativePrimitiveArgType argMyFunc1[] = {  REALSXP, INTSXP, STRSXP, LGLSXP };

// The functions should return void in order to deal with .C, 
// so we have to use function parameter to return value to the caller. 
void Multiply( double *x, double *y, double *result );
void   Divide( double *x, double *y, double *result );

static R_NativePrimitiveArgType argMultiply[] = {  REALSXP, REALSXP, REALSXP };
static R_NativePrimitiveArgType argDivide[] = {  REALSXP, REALSXP, REALSXP };

static const R_CMethodDef cMethods[] = 
{ 
	// C functions extended by using .C interface 
   {"Multiply",   (DL_FUNC) &Multiply, 3, argMultiply},
   {"Divide",     (DL_FUNC) &Divide,   3, argDivide},
   {NULL, NULL, 0, NULL}
};

R_CallMethodDef callMethods[] = 
{
	// C functions extended by using .Call interface 
	{ "Add",        (DL_FUNC)&Add,       2 },
	{ "Subtract",   (DL_FUNC)&Subtract,  2 },
	{ "MyPi",       (DL_FUNC)&MyPi,      0 },
	{ "Increment",  (DL_FUNC)&Increment, 1 },
	{ NULL, NULL, 0 }
};

void R_init_RCExtension(DllInfo *dllInfo)
{
	R_registerRoutines(dllInfo, cMethods, callMethods, NULL, NULL);
}


void R_unload_RCExtension(DllInfo *info)
{
  // Release resources.
}
////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

////// Pure C functions to be used with .C interface /////
// The functions should return void in order to deal with .C, 
// so we have to use function parameter to return value to the caller. 
void Multiply(double *x, double *y, double *result)
{
	*result = *x * *y;
}

void Divide(double *x, double *y, double *result)
{
	*result = 0;

	if( *y != 0 )
		*result = *x / *y;
}


//////// Function to be used with .Call interface //////////// 
SEXP Add(SEXP x, SEXP y)
{
	SEXP result = PROTECT(allocVector(REALSXP, 1));
	REAL(result)[0] = asReal(x) + asReal(y);
	UNPROTECT(1);
  
	return result;
}

SEXP Subtract(SEXP x, SEXP y)
{
	SEXP result = PROTECT(allocVector(REALSXP, 1));
	REAL(result)[0] = asReal(x) - asReal(y);
	UNPROTECT(1);
  
	return result;
}


SEXP Increment(SEXP x)
{
	SEXP result = PROTECT(allocVector(REALSXP, 1));
	REAL(result)[0] = asReal(x) + 1;
	UNPROTECT(1);
  
	return result;
}

SEXP MyPi()
{
	SEXP result = PROTECT(allocVector(REALSXP, 1));
	REAL(result)[0] = 3.14159265359;
	UNPROTECT(1);
  
	return result;
}
 




/*
c:/Rtools/mingw_64/bin/gcc  -I"C:/Dev/R/R-3.4.1/include" -DNDEBUG     -I"d:/Compiler/gcc-4.9.3/local330/include"     -O2 -Wall  -std=gnu99 -mtune=core2 -c MyRCExtn1.c -o MyRCExtn1.o

c:/Rtools/mingw_64/bin/gcc -shared -s -static-libgcc -o MyRCExtn1.dll tmp.def MyRCExtn1.o -Ld:/Compiler/gcc-4.9.3/local330/lib/x64 -Ld:/Compiler/gcc-4.9.3/local330/lib 
-LC:/Dev/R/R-3.4.1/bin/x64 -lR
*/