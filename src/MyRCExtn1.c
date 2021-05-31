#include <stdio.h>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


SEXP Add(SEXP x, SEXP y);
SEXP Subtract(SEXP x, SEXP y);
SEXP Increment(SEXP x);
SEXP MyPi();
SEXP MultiplyVector(SEXP x , SEXP y);
SEXP SchussWinkel(SEXP v0, SEXP target_hit_error, SEXP angle_Schussebenen,SEXP Ziel_Schussebenen,SEXP m, SEXP k);


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
	{ "SchussWinkel",  (DL_FUNC)&SchussWinkel, 3 },
	{ "MultiplyVector",  (DL_FUNC)&MultiplyVector, 2 },
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

SEXP MultiplyVector(SEXP x , SEXP y)
 {
   int n = length(x);
   SEXP out = PROTECT(allocVector(REALSXP, n));

   for (int i = 0; i < n; i++) {
     REAL(out)[i] =  REAL(x)[i]*asReal(y);
   }
   UNPROTECT(1);
   return out;
 }

double long betrag (double long x, double long y){
  return sqrt(pow(x,2)+pow(y,2));
}

double long a_t (double long t,double long v0[], double m, double k){
  return (-1)*pow(betrag(v0[0],v0[1]),2)*k/m;
  }

SEXP SchussWinkel(SEXP v0, SEXP target_hit_error ,SEXP angle_Schussebenen, SEXP Ziel_Schussebenen,SEXP m, SEXP k)
{
  // Ziel
  double long angle  = asReal(angle_Schussebenen);
  double  long dist   = betrag(REAL(Ziel_Schussebenen)[0],REAL(Ziel_Schussebenen)[1]);
  //iterationen
  //double  t      = 1/(asReal(v0)/asReal(target_hit_error));
  static double t      = 0.0001;

  //unsigned int long iter = floor(asReal(v0)/dist/asReal(target_hit_error)/t*0.1);
  static unsigned int long iter = 100000;
  //Beschleunigung
  double long a = (-1)*(pow(asReal(v0),2)*asReal(k))/asReal(m);

  //Geschwindigkeitsänderung
  double long dv_x =  a*cos(angle)*t;
  double long dv_y = (a*sin(angle)-9.81)*t; // -Erdbeschleunigung -9.81

  static int column = 4;
  //Datenspeicher
  double long m_data[iter+1][column];

  //Flugbahn inital
  m_data[0][0] = asReal(v0)*cos(angle); // Anfangsgeschwindigkeit v_x
  m_data[0][1] = asReal(v0)*sin(angle); // Anfangsgeschwindigkeit v_y
  m_data[1][0] = m_data[0][0] + dv_x;   // Init Geschwindigkeit v_x + Geschwindigkeitsänderung
  m_data[1][1] = m_data[0][1] + dv_y;   // Init Geschwindigkeit v_x + Geschwindigkeitsänderung
  m_data[0][2] = 0;              // init Position s_x
  m_data[0][3] = 0;              // init Position s_y
  m_data[1][2] = m_data[1][0]*t; // Position s_x
  m_data[1][3] = m_data[1][1]*t; // Position s_y

  long unsigned int cnt = 0;
  // Flugbahn
  for(unsigned long int i = 2; i < iter; i++){
    // Flugwinkel
    double long angle = atan((m_data[i-2][3]-m_data[i-1][3])/(m_data[i-2][2]-m_data[i-1][2]));
    // letzte Distanz
    double long dist_  = betrag(REAL(Ziel_Schussebenen)[0]-m_data[i-1][2],REAL(Ziel_Schussebenen)[1]-m_data[i-1][3]);
    // letzte Geschwindigkeit
    double long v0_   = betrag(m_data[i-1][0],m_data[i-1][1]);
    // Luftwiderstand (Beschleunigung entgegen der Flugrichtung)
    double long a    = -(1)*(pow(v0_,2)*asReal(k)) / asReal(m);
    //Geschwindigkeitsänderung
    double long dv_x =  a*cos(angle)       * t;
    double long dv_y = (a*sin(angle)-9.81) * t;   // -Erdbeschleunigung -9.81

    // Neue Geschwindigkeit
    m_data[i][0] = m_data[i-1][0] + dv_x; // alte Geschwindigkeit x + delta v_x
    m_data[i][1] = m_data[i-1][1] + dv_y; // alte Geschwindigkeit y + delta v_y
    // Neue Position
    m_data[i][2] = m_data[i-1][2] + m_data[i][0]*t; // alte position + v_x*t
    m_data[i][3] = m_data[i-1][3] + m_data[i][1]*t; // alte position + v_y*t

    //Abbruch: ist die alte Distanz kleiner als die neue
    if(dist_ < betrag(REAL(Ziel_Schussebenen)[0]-m_data[i][2],REAL(Ziel_Schussebenen)[1]-m_data[i][3])){
      break;
      }
    cnt++;
  }

  //Datenspeicher von R lesbar
  SEXP result = PROTECT(allocVector(REALSXP, 4*cnt));
  //apply data to vector
  for(unsigned long int i = 0; i < column; i++){
    for(unsigned long int j = 0; j < cnt; j++){
      REAL(result)[j+(i*cnt)] = m_data[j][i];
      }
  }

  UNPROTECT(1);
  return result;
}

/*
c:/Rtools/mingw_64/bin/gcc  -I"C:/Dev/R/R-3.4.1/include" -DNDEBUG     -I"d:/Compiler/gcc-4.9.3/local330/include"     -O2 -Wall  -std=gnu99 -mtune=core2 -c MyRCExtn1.c -o MyRCExtn1.o

c:/Rtools/mingw_64/bin/gcc -shared -s -static-libgcc -o MyRCExtn1.dll tmp.def MyRCExtn1.o -Ld:/Compiler/gcc-4.9.3/local330/lib/x64 -Ld:/Compiler/gcc-4.9.3/local330/lib
-LC:/Dev/R/R-3.4.1/bin/x64 -lR
*/
