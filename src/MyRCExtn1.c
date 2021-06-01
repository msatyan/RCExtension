#include <stdio.h>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

/////////// .C interface style ////////////////
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

////// .cal interface style ///////////////
SEXP Flugbahn (SEXP v0, SEXP target_hit_error, SEXP angle_Schussebenen,SEXP Ziel_Schussebenen,SEXP m, SEXP k);

R_CallMethodDef callMethods[] =
{
	// C functions extended by using .Call interface
	{ "Flugbahn",   (DL_FUNC)&Flugbahn,  3 },
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

///////// Functions in C to be used by C and only C
double long betrag (double long x, double long y){
  return sqrt(pow(x,2)+pow(y,2));
}

double long a_t (double long t,double long v0[], double m, double k){
  return (-1)*pow(betrag(v0[0],v0[1]),2)*k/m;
  }

//////// Function to be used with .Call interface ////////////
SEXP Flugbahn (SEXP v0, SEXP target_hit_error ,SEXP angle_Schussebenen, SEXP Ziel_Schussebenen,SEXP m, SEXP k)
{
  // Ziel
  double long angle  = asReal(angle_Schussebenen);
  double  long dist   = betrag(REAL(Ziel_Schussebenen)[0],REAL(Ziel_Schussebenen)[1]);
  //iterationen
  //double  t      = 1/(asReal(v0)/asReal(target_hit_error));
  static double t      = 0.0001;

  //unsigned int long iter = floor(asReal(v0)/dist/asReal(target_hit_error)/t*0.1);
  const static unsigned int long iter = 100000;
  //Beschleunigung
  double long a = (-1)*(pow(asReal(v0),2)*asReal(k))/asReal(m);

  //Geschwindigkeitsänderung
  double long dv_x =  a*cos(angle)*t;
  double long dv_y = (a*sin(angle)-9.81)*t; // -Erdbeschleunigung -9.81

  const static int column = 5;
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
  m_data[0][4] = 0; // t = 0
  m_data[1][4] = t; // t = 0

  unsigned int long cnt = 0;
  // Flugbahn
  for(unsigned int long i = 2; i < iter; i++){
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
    //Zeit
    m_data[i][4] = i*t;

    //Abbruch: ist die alte Distanz kleiner als die neue
    if(dist_ < betrag(REAL(Ziel_Schussebenen)[0]-m_data[i][2],REAL(Ziel_Schussebenen)[1]-m_data[i][3])){
      break;
      }
    cnt = cnt + 1;
  }

  //Datenspeicher von R lesbar
  SEXP result = PROTECT(allocVector(REALSXP, column*cnt));
  //apply data to vector
  for(unsigned long int i = 0; i < column; i++){
    for(unsigned long int j = 0; j < cnt; j++){
      REAL(result)[j+(i*cnt)] = m_data[j][i];
      }
  }

  UNPROTECT(1);
  return result;
}
