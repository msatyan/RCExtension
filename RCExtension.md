This is a sample module to demonstrate how to extent R language functionality by using C language routines. The R support writing extensions by using a verity of languages. The R language provides flexibility and simplicity for statistical programing, it is inherently slow. You may turbocharge the performance by creating native extension module. Writing extension module by using C language is one of the most efficient and also the popular choice. 

If you are new to writing R extension then you may find the following URL very useful.  
[writing R extension](https://cran.r-project.org/doc/manuals/R-exts.html)  
Especially the topic discussed on the section System and foreign language interfaces  
[System and foreign language interfaces](https://cran.r-project.org/doc/manuals/r-release/R-exts.html#System-and-foreign-language-interfaces)

Let us skim through the minimum steps involved, by the time we can execute a C module from R.

* C language source code with one or more functions that we can be called from R.
* Compile the above C module and create a shared library (.so or DLL based on the OS)

This shared library will be loaded on demand into R interpreter's process space by using dyn.load and unloaded by dyn.unload.
```
Example:
library.dynam("mylib", package, lib.loc)
```
Once the library has loaded into R interpreter's process space, it looks for the symbol named R_init_mylib.
Similarly, when unloading the object, R looks for a routine named R_unload_mylib

```C
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
```

Now you may tell R interpreter about the functions you have packed in the native shared library to exposing it for R program; this is done by Registering Native Routines.
To register routines with R, one calls the C routine R_registerRoutines().

FYI: By default, R uses the operating system-specific dynamic loader to lookup the symbol in the shared library. The recommended approach is explicitly register routines with R and use a single, platform-independent mechanism for finding the routines in shared library.


R provides a set of interface for extending it, they are listed below for you to explore further 
In this example we will be using .Call interface, it provides nice flexibility and efficiency by giving reference access to memory structures across language boundaries (R vs C). The .C interface is relatively simple but not that efficient. Likely half the performance compared to .Call because it copies objects while exchanging language boundaries.  

```
.C                R_CMethodDef
.Call             R_CallMethodDef
.Fortran          R_FortranMethodDef
.External         R_ExternalMethodDef
```

FYI: If you are planning to writer C or C++ extension then you may also want to explore the 'Rcpp' package though it is not an integral part of R.

