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
* **R Packages** (by Hadley Wickham)
* **Advanced R** (by Hadley Wickham)

### System setup to create the package
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

A possible right location call this will be when a package is getting loaded. The **.onLoad()** lifecycle hook provided by R is getting called when a page get loaded. Then we may put the code in that routine.   
**See also** :  .onAttach(),   .onUnload(),  .onDetach(),   .Last.lib()

```R
.onLoad <- function(lib, pkg) 
{
   library.dynam("RCExtension", pkg, lib )
}
```

Once the library has loaded into R interpreter's process space, it looks for the symbol named **R_init_RCExtension**. Similarly, when unloading the object, R looks for a routine named **R_unload_RCExtension**. Where **'RCExtension'** is the name of the extension shared library.  
FYI: By default, R uses the operating system-specific dynamic loader to lookup the symbol in the shared library. The recommended approach is explicitly register routines with R and use a single, platform-independent mechanism for finding the routines in shared library.

```C
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

void R_init_RCExtension(DllInfo *dllInfo)
{
  // Register routines,
  // allocate resources.
}

void R_unload_RCExtension(DllInfo *dllInfo)
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

##### Firing build to creare shared library
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

### Visual Studio for builing the shared library
We can use Visual Studio (here I am using VS 2017) to build the shared library. For this a bit of reverse engineering we have to do first. The R installation has put **R.DLL** but no **R.LIB**, then we have to create **R.LIB from R.DLL** before we can start use Visual Studio.  

I have installed 64 bit version of R at C:\Dev\R\R-3.4.1, then the R.DLL will be located at C:\Dev\R\R-3.4.1\bin\x64.

```bash
#open VS x64 command windows
cd C:\Dev\R\R-3.4.1\bin\x64
dumpbin /EXPORTS R.dll > r.def
```

Edit **r.def** file by only keeping the exported symbols. At the top of the line add a line **EXPORTS**. For your reference r.def file has been included with this repository, it is available at **Scratchpad**. If you cat r.def the output may look like this.  

```bash
$ cat r.def

EXPORTS
ATTRIB
AllDevicesKilled
BODY
CAAR
CAD4R
CADDDR
CADDR
- - - - - - 
- - - - - - 
- - - - - - 
- - - - - - 
windelmenuitem
windelmenus
wingetmenuitems
xdr_double
xdrmem_create
xerbla_
```

Now you can use **LIB** utility to create the library by using the following command   
**lib /def:r.def /OUT:r.lib  /MACHINE:X64**
```bat
C:\Dev\R\R-3.4.1\bin\x64> lib /def:r.def /OUT:r.lib  /MACHINE:X64
Microsoft (R) Library Manager Version 14.11.25506.0
Copyright (C) Microsoft Corporation.  All rights reserved.

   Creating library r.lib and object r.exp
```

If you do a listing of the dir then you may see **r.lib** has created
```
C:\Dev\R\R-3.4.1\bin\x64>ls -l r.*
---------- 1 devc mkpasswd  15792 Aug 20 19:49 r.def
---------- 1 devc mkpasswd 125329 Aug 20 20:41 r.exp
---------- 1 devc mkpasswd 202816 Aug 20 20:41 r.lib
```

You are now ready to use VS to build the shared library. Make sure you have appropriately specified additional **INCLUDE** and **LIB** directories along with **R.LIB**
```bat
# set this on your system env
set MY_R_DIR=C:\Dev\R\R-3.4.1

# VS Project setting, specify additional **INCLUDE** and **LIB** directories with the following.

$(MY_R_DIR)\include
$(MY_R_DIR)\bin\x64

# then also specify R.LIB
r.lib
```

### Exposing C functions to R
By now you may have learned how to build a shared library from C source code. Then it is time to switch focus how to exposing C functions to R. In short it is by telling the **R interpreter** about the **C functions** that we have packed in the **extension shared library**. Eventually these functions will get exposed to R program.  

We may call **R_registerRoutines()** function (a C API by R) to registering C functions that we plan to expose to R. This is typically done when the DLL is first loaded within the initialization routine R_init_**'dll name'** which is described in **dyn.load**.

```C
SEXP Add(SEXP x, SEXP y);
SEXP Subtract(SEXP x, SEXP y);
SEXP Increment(SEXP x);
SEXP MyPi();

R_CallMethodDef callMethods[] = 
{
	{ "Add",        (DL_FUNC)&Add,       2 },
	{ "Subtract",   (DL_FUNC)&Subtract,  2 },
	{ "MyPi",       (DL_FUNC)&MyPi,      0 },
	{ "Increment",  (DL_FUNC)&Increment, 1 },
	{ NULL, NULL, 0 }
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
// The C functions should return void to be used with .C interface.
// Then capture the result through parameter
void Multiply( double *x, double *y, double *result );
void   Divide( double *x, double *y, double *result );

// double *,      int *,     char **,     int *
// REALSXP,       INTSXP,    STRSXP,      LGLSXP
static R_NativePrimitiveArgType argMultiply[] = {  REALSXP, REALSXP, REALSXP };
static R_NativePrimitiveArgType argDivide[] = {  REALSXP, REALSXP, REALSXP };

static const R_CMethodDef cMethods[] = 
{ 
   {"Multiply",   (DL_FUNC) &Multiply, 3, argMultiply},
   {"Divide",     (DL_FUNC) &Divide,   3, argDivide},
   {NULL, NULL, 0, NULL}
};

```
The R types and corresponding param type 
| **R type**   | **Param type** |**Raw C type**
|:-------------|:---------------|:---------------
| `numeric`    | REALSXP        | double *
| `integer`    | INTSXP         | int *
| `logical`    | LGLSXP         | int *
| `single`     | SINGLESXP      | 
| `character`  | STRSXP         | char **
| `list`       | VECSXP         | 


Then we modify the **R_registerRoutines()** function call to include **cMethods** structure also.
```C
void R_init_RCExtension(DllInfo *dllInfo)
{
   R_registerRoutines( dllInfo, cMethods, callMethods, NULL, NULL);
}
```
The .C interface is useful when we decided to expose existing C library function in R that knows nothing about R. Then while binding by using .C interface we are providing more details about the argument type of the native C functions. R uses this additional information during marshaling the value of the parameter while exchanging across language boundary. Where as in case of .Call interface the parameters are being accessed by reference then R assume each of the languages already know about the parameter type and its memory layout.

The .C() interface's automatic back and forth mapping between R vectors and their C equivalents is listed here. 

| **R type**   | **C type** |
|:-------------|:-----------|
| `Logical`    | int*       |
| `Integer`    | int*       | 
| `Double`     | double*    | 
| `Character`  | char**     | 
| `Raw`        | unsigned char*    | 


### Using the C function in R
Those C functions so far we have exposed to R can be called by using  its corresponding interface.

```diff
- FYI: Such usage has broken with R version 3.0 and above
```

```R
.C( "Multiply", x, y, numeric(1) ) [[3]]
```

```R
.Call( "Add", x, y)
```
To make it more user-friendly we could provide wrapper R functions that resonate well with the traditional function usage, then it becomes.
```R
Multiply <- function(x, y) 
{
  .C( "Multiply", x, y, numeric(1) ) [[3]]
}
```

```R
Add <- function(x, y) 
{
  .Call("Add", x, y)
}
```

### Version 3.0 and above: Using the C function in R 
Edit the NAMESPACE file by adding useDynLib directives 
```R
useDynLib(RCExtension, .registration = TRUE, .fixes = "C_") 
```

By specifying these names in the useDynLib directive, the native symbols are resolved when the package is loaded and R variables identifying these symbols are added to the package's namespace with these names. These can be used in the .C, .Call, .Fortran and .External calls in place of the name of the routine and the PACKAGE argument. For instance, we can call the routines **Multiply**, **Add** etc from R with the code

```R
.C( C_Multiply, x, y, numeric(1) ) [[3]]
```

```R
.Call( C_Add, x, y)
```
If we use it with with a R wrapper function then
```R
Multiply <- function(x, y) 
{
  .C( C_Multiply, x, y, numeric(1) ) [[3]]
}
```

```R
Add <- function(x, y) 
{
  .Call(C_Add, x, y)
}
```

**FYI** : If a shared object/DLL is loaded more than once the most recent version is used. More generally, if the same symbol name appears in several shared objects, the most recently loaded occurrence is used.



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
* [R’s C interface (Snippet from Advanced R by Hadley Wickham)](http://adv-r.had.co.nz/C-interface.html)
* [Package structure (R packages by Hadley Wickham)](http://r-pkgs.had.co.nz/package.html)
* [Building, Testing, and Distributing Packages](https://support.rstudio.com/hc/en-us/articles/200486508-Building-Testing-and-Distributing-Packages)
* [RStudio As An IDE For C++](https://charlotte-ngs.github.io/RStudioAsCppEditor/RStudioAsCppEditorDevDoc.html)
