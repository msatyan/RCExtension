##### Copyright 2017 Sathyanesh Krishnan
##### Licensed under the Apache License, Version 2.0 

## RCExtension
Simple R Extensions with C language

This is a sample R package to demonstrate how to extent R  functionality by using C language routines. The R supports writing extensions by using a verity of languages. Extension module by using C is very efficient and popular. The R provides flexibility and simplicity for statistical programing, at the same time it is inherently slow. The C language extensions can act as a turbocharger for R modules where it needs speed and efficiency.

### References
If you plan to create R Extensions then you may find the following information interesting.
* [Writing R extension](https://cran.r-project.org/doc/manuals/R-exts.html)  
* [System and foreign language interfaces](https://cran.r-project.org/doc/manuals/r-release/R-exts.html#System-and-foreign-language-interfaces)
* [Developing Packages with RStudio](https://support.rstudio.com/hc/en-us/articles/200486488)
#### Books:
* **R Packages** by (Hadley Wickham)
* **Advanced R** by (Hadley Wickham)

### Recommitted system setup to create the package
#### Windows
* [Install R](https://cran.r-project.org/)
* [Rtools](https://cran.rstudio.com/bin/windows/Rtools/) (GNU C/C++ compiler is part of it. **FYI**: I may advice it to be installed at default location which is **C:\Rtools**, to avoid extra build setup)
* [LaTeX](https://miktex.org/download) (To create R manuals; a complete build of R including PDF manuals too)
* [RStudio](https://www.rstudio.com/) (Optional: it is convenient to use, but no need of it if you plan to use only command line build.)

#### Linux
TODO: 

### The extension module
The core of the R extension by using C is a shared library (**.so** on Linux and **.DLL** on Windows). This shared library will be loaded on demand into R interpreter's process space by using **dyn.load** and unloaded by **dyn.unload**. 
```C
// Example:
library.dynam("RCExtension", package, lib.loc)
```
Once the library has loaded into R interpreter's process space, it looks for the symbol named **R_init_RCExtension**. Similarly, when unloading the object, R looks for a routine named **R_unload_RCExtension**. Where **'RCExtension'** is the name of the extension shared library.  
FYI: By default, R uses the operating system-specific dynamic loader to lookup the symbol in the shared library. The recommended approach is explicitly register routines with R and use a single, platform-independent mechanism for finding the routines in shared library.

```C
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

void R_init_RCExtension(DllInfo *info)
{
  // Register routines,
  // allocate resources.
}

void R_unload_RCExtension(DllInfo *info)
{
  // Release resources.
}
```

### Building the shared library in nutshell
The two main activities involved in building an R package is building the R runtime library and building R manuals.  For building shared library from it C/C++ source code R uses GNU C/C++ compiler.  When you are building it on windows platform you may install Rtools, it contain all necessary utility to complete the build. In short Rtools provides a toolchain for Windows platform that work well with R. It mainly includes GNU make, GNU gcc (from MinGW), and other utilities commonly used on UNIX-ish platform.

##### My Windows Env for R build
```
SET PATH=C:\Dev\R\R-3.4.1\bin\x64;C:\Dev\MiKTeX2.9\miktex\bin\x64\;c:\Rtools\bin;c:\Rtools\mingw_64\bin;%PATH%
```

##### Firing a shared library build
R CMD SHLIB *.c
```bash
C:\work\RCExtension\src>R CMD SHLIB *.c
# The above command that you typed will translate to the following commands.
c:/Rtools/mingw_64/bin/gcc  -I"C:/Dev/R/R-3.4.1/include" -DNDEBUG     -I"d:/Compiler/gcc-4.9.3/local330/include"     -O2 -Wall  -std=gnu99 -mtune=core2 -c MyRCExtn1.c -o MyRCExtn1.o

c:/Rtools/mingw_64/bin/gcc -shared -s -static-libgcc -o MyRCExtn1.dll tmp.def MyRCExtn1.o -Ld:/Compiler/gcc-4.9.3/local330/lib/x64 -Ld:/Compiler/gcc-4.9.3/local330/lib -LC:/Dev/R/R-3.4.1/bin/x64 -lR
```

```bash
C:\work\RCExtension\src>ls -l
total 21
---------- 1 devc mkpasswd   234 Aug 19 22:31 MyRCExtn1.c
---------- 1 devc mkpasswd 15360 Aug 20 10:58 MyRCExtn1.dll
---------- 1 devc mkpasswd   863 Aug 20 10:58 MyRCExtn1.o
```


##### See also:
* [Creating shared objects](https://cran.r-project.org/doc/manuals/R-exts.html#Creating-shared-objects)
* [Package Development Prerequisites](https://support.rstudio.com/hc/en-us/articles/200486498-Package-Development-Prerequisites)
* [Building R for Windows](https://cran.r-project.org/bin/windows/Rtools/)
* [MinGW](http://www.mingw.org/)
* [Using external compilers with R](http://www.stats.uwo.ca/faculty/murdoch/software/compilingDLLs/)
* [Writing packages for R](http://www.stats.uwo.ca/faculty/murdoch/software/compilingDLLs/readme.packages.txt)
* [Customizing package compilation](https://cran.r-project.org/doc/manuals/R-admin.html#Customizing-package-compilation)
* [The R API: entry points for C code](https://cran.r-project.org/doc/manuals/R-exts.html#The-R-API)
* [System and foreign language interfaces](https://cran.r-project.org/doc/manuals/R-exts.html#System-and-foreign-language-interfaces)
* [R’s C interface (Snippet from Advanced R)](http://adv-r.had.co.nz/C-interface.html)
* [Package structure (R packages)](http://r-pkgs.had.co.nz/package.html)
* [RStudio As An IDE For C++](https://charlotte-ngs.github.io/RStudioAsCppEditor/RStudioAsCppEditorDevDoc.html)

### Exposing C functions to R
By now you may have learned how to build a shared library from C source code. Then it is time to switch focus how to exposing C functions to R. In short it is by telling the **R interpreter** about the **C functions** that we have packed in the **extension shared library**. Eventually these functions will get exposed to R program.  

We may call **R_registerRoutines()** function (a C API by R) to registering C functions that we plan to expose to R.This is typically done when the DLL is first loaded within the initialization routine R_init_**'dll name'** which is described in **dyn.load**.

```C
SEXP myCFunc1(SEXP a, SEXP b, SEXP c);
SEXP myCFunc2(SEXP a, SEXP b);
SEXP myCFunc3(SEXP a, SEXP b, SEXP c, SEXP d);

R_CallMethodDef callMethods[]  = 
{
  {"myFunc1inR", (DL_FUNC) &myCFunc1, 3},
  {"myFunc2inR", (DL_FUNC) &myCFunc2, 2},
  {"myFunc3inR", (DL_FUNC) &myCFunc3, 4},
  {NULL, NULL, 0}
};

void R_init_RCExtension(DllInfo *dllInfo)
{
   R_registerRoutines(dllInfo, NULL, callMethods, NULL, NULL);
}
```

R provides four interface for extending it {**.C, .Call, .Fortran, .External**}. In this example we are focusing more on **.Call** interface, it provides nice flexibility and efficiency by giving **reference access to memory structures** across language boundaries (R and C/C++) within the process. The **.C** interface is relatively simple but not that efficient though. Likely endup half the performance compared to **.Call** (if there is lot of data exchange with the function call) because it copies objects while exchanging language boundaries.  

**FYI:** If you are planning to writer **C or C++** extension then you may also want to explore the **'Rcpp'** package. Though it is not an integral part of R, it may make the integration simpler without much performance impact. If the system need extremely high efficiency, scalability and fault tolerance then the **.Call** is likely be the choice.

```
.C                R_CMethodDef
.Call             R_CallMethodDef
.Fortran          R_FortranMethodDef
.External         R_ExternalMethodDef
```

By default, R uses the operating system-specific dynamic loader to lookup the symbol in the shared library. The recommended approach is explicitly register routines with R and use a single, platform-independent mechanism for finding the routines in shared library. With this approach R can get and store additional information about the methods to be more efficient based on the circumstances it is operating on. This additional information is highly useful while exchanging data during a function call from language boundary. For example the .Call interface makes reference access to the parameter where as .C interface copy and replicate parameters when it exchange the language boundary. 

R allows mixing interface styles in the extensions, in the following example we have used both .C and .Call interfaces. Let us say, we are enhance the existing package by adding a couple of more C functions, this time by using .C interface then it may look like this.

```C
void myCFunc5(double *x, int *n, char **names, int *status);
void myCFunc6(double *x, int *n, char **names);

static R_NativePrimitiveArgType myCFunc5ArgType[] = {  REALSXP, INTSXP, STRSXP, LGLSXP };
static R_NativePrimitiveArgType myCFunc6ArgType[] = {  REALSXP, INTSXP, STRSXP };

static const R_CMethodDef cMethods[] = {
   {"myFunc5inR", (DL_FUNC) &myCFunc5, 4, myCFunc5ArgType},
   {"myFunc6inR", (DL_FUNC) &myCFunc6, 3, myCFunc6ArgType},
   {NULL, NULL, 0, NULL}
};

void R_init_RCExtension(DllInfo *dllInfo)
{
   R_registerRoutines( dllInfo, cMethods, callMethods, NULL, NULL);
}
```
During the .C interface call we are providing more details about the argument type of the C function. R uses this additional information during marshaling the value of the parameter across language boundary. Where as in case of .Call interface the parameters are being accessed by reference then R assume the other language already know about the parameter type and its memory layout. 



