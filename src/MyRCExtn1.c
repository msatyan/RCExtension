#include <stdio.h>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


SEXP myCFunc1(SEXP a, SEXP b, SEXP c);
SEXP myCFunc2(SEXP a, SEXP b);
SEXP myCFunc3(SEXP a, SEXP b, SEXP c, SEXP d);

R_CallMethodDef callMethods[] =
{
	{ "myFunc1inR", (DL_FUNC)&myCFunc1, 3 },
	{ "myFunc2inR", (DL_FUNC)&myCFunc2, 2 },
	{ "myFunc3inR", (DL_FUNC)&myCFunc3, 4 },
	{ NULL, NULL, 0 }
};

void R_init_RCExtension(DllInfo *dllInfo)
{
	R_registerRoutines(dllInfo, NULL, callMethods, NULL, NULL);
}


void R_unload_RCExtension(DllInfo *info)
{
  // Release resources.
}

SEXP myCFunc1(SEXP a, SEXP b, SEXP c)
{
	return(a);
}

SEXP myCFunc2(SEXP a, SEXP b)
{
	return(a);
}

SEXP myCFunc3(SEXP a, SEXP b, SEXP c, SEXP d)
{
	return(a);
}

// c:/Rtools/mingw_64/bin/gcc -shared -s -static-libgcc -o MyRCExtn1.dll tmp.def MyRCExtn1.o 
// -Ld:/Compiler/gcc-4.9.3/local330/lib/x64 -Ld:/Compiler/gcc-4.9.3/local330/lib -LC:/Dev/R/R-3.4.1/bin/x64 -lR


