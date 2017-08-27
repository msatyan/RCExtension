#include <stdio.h>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


double Multiply(double a, double b);
double Divide(double a, double b);

SEXP Add(SEXP a, SEXP b);
SEXP Subtract(SEXP a, SEXP b);
SEXP Increment(SEXP a);
SEXP MyPi();

/////////// .C interface style ////////////////
// static R_NativePrimitiveArgType argMultiply[] = {  REALSXP, REALSXP };
// static R_NativePrimitiveArgType argDivide[] = {  REALSXP, REALSXP };

// static const R_CMethodDef cMethods[] = {
//    {"Multiply", (DL_FUNC) &Multiply, 2, argMultiply},
//    {"Divide", (DL_FUNC) &Divide, 2, argDivide},
//    {NULL, NULL, 0, NULL}
// };


///////// .Call interface style ///////////
R_CallMethodDef callMethods[] =
{
	{ "Add",        (DL_FUNC)&Add, 2 },
	{ "Subtract",   (DL_FUNC)&Subtract, 2 },
	{ "MyPi",       (DL_FUNC)&MyPi, 0 },
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

////// Pure C functions to be used with .C interface /////
double Multiply(double a, double b)
{
	double result = a * b;
	return result;
}

double Divide(double a, double b)
{
	double result = 0;

	if( b != 0 )
		result = a / b;

	return result;
}


//////// Function to be used with .Call interface //////////// 
SEXP Add(SEXP a, SEXP b)
{
	SEXP result = PROTECT(allocVector(REALSXP, 1));
	REAL(result)[0] = asReal(a) + asReal(b);
	UNPROTECT(1);
  
	return result;
}

SEXP Subtract(SEXP a, SEXP b)
{
	SEXP result = PROTECT(allocVector(REALSXP, 1));
	REAL(result)[0] = asReal(a) - asReal(b);
	UNPROTECT(1);
  
	return result;
}


SEXP Increment(SEXP a)
{
	SEXP result = PROTECT(allocVector(REALSXP, 1));
	REAL(result)[0] = asReal(a) + 1;
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