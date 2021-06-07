#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <vector>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


/////////// .C interface style ////////////////
// The functions should return void in order to deal with .C,
// so we have to use function parameter to return value to the caller.
void Multiply( double *x, double *y, double *result);
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
//
SEXP Flugbahn (SEXP v0, SEXP t, SEXP angle_Schussebenen,SEXP Ziel_Schussebenen,SEXP m, SEXP k);
SEXP FlugbahnV2 (SEXP v0, SEXP t ,SEXP angle_Schussebenen, SEXP Ziel_Schussebenen,SEXP m, SEXP k);

R_CallMethodDef callMethods[] =
{
	// C functions extended by using .Call interface
	{ "Flugbahn",   (DL_FUNC)&Flugbahn,  3 },
	{ "FlugbahnV2",   (DL_FUNC)&FlugbahnV2,  3 },
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
//Konstanten
const static double pi = 3.14159265359; //Kreiszahl
const static double g = -9.81;        // [m / s ^ 2]

double long betrag (double long x, double long y){
  return sqrt(pow(x,2)+pow(y,2));
}

double long a_t (double long t,double long v0[], double m, double k){
  return (-1)*pow(betrag(v0[0],v0[1]),2)*k/m;
  }

//////// Function to be used with .Call interface ////////////
SEXP Flugbahn (SEXP v0, SEXP t ,SEXP angle_Schussebenen, SEXP Ziel_Schussebenen,SEXP m, SEXP k)
{
  // Ziel
  double long angle  = asReal(angle_Schussebenen);

  //iterationen
  const static unsigned int  iter = 300; // it is not possible to protect more memory than for 10000 iterations
  //Beschleunigung
  double long a = (-1)*(pow(asReal(v0),2)*asReal(k))/asReal(m);

  //Geschwindigkeitsänderung
  double long dv_x =  a*cos(angle)*asReal(t);
  double long dv_y = (a*sin(angle)+g)*asReal(t); // -Erdbeschleunigung -9.81

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
  m_data[1][2] = m_data[1][0]*asReal(t); // Position s_x
  m_data[1][3] = m_data[1][1]*asReal(t); // Position s_y
  m_data[0][4] = 0; // t = 0
  m_data[1][4] = asReal(t); // t = 0

  unsigned int long cnt = 0;
  // Flugbahn
  for(unsigned int long i = 2; i < iter; i++){
    // Flugwinkel
    double long angle_ = atan((m_data[i-2][3]-m_data[i-1][3])/(m_data[i-2][2]-m_data[i-1][2]));
    // letzte Distanz
    double long dist_  = betrag(REAL(Ziel_Schussebenen)[0]-m_data[i-1][2],REAL(Ziel_Schussebenen)[1]-m_data[i-1][3]);
    // letzte Geschwindigkeit
    double long v0_   = betrag(m_data[i-1][0],m_data[i-1][1]);
    // Luftwiderstand (Beschleunigung entgegen der Flugrichtung)
    double long a    = -(1)*(pow(v0_,2)*asReal(k)) / asReal(m);
    //Geschwindigkeitsänderung
    double long dv_x =  a*cos(angle_)    * asReal(t);
    double long dv_y = (a*sin(angle_)+g) * asReal(t);   // -Erdbeschleunigung -9.81

    // Neue Geschwindigkeit
    m_data[i][0] = m_data[i-1][0] + dv_x; // alte Geschwindigkeit x + delta v_x
    m_data[i][1] = m_data[i-1][1] + dv_y; // alte Geschwindigkeit y + delta v_y
    // Neue Position
    m_data[i][2] = m_data[i-1][2] + m_data[i][0]*asReal(t); // alte position + v_x*t
    m_data[i][3] = m_data[i-1][3] + m_data[i][1]*asReal(t); // alte position + v_y*t
    //Zeit
    m_data[i][4] = i*asReal(t);

    // Abbruch wenn der Flugwinkel fast -90 Grad wird. Objekt fällt nur noch
    double long test = round(angle_ / pi * 180 * 1000) / 1000;
    if (test < -89.99) {
      break;
    }

    //Abbruch wenn die alte Distanz kleiner als die Neue
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

SEXP FlugbahnV2 (SEXP v0, SEXP t ,SEXP angle_Schussebenen, SEXP Ziel_Schussebenen,SEXP m, SEXP k)
{
  // Sequenz der Werte die abgespeichert werden müssen
  const unsigned int column = 5;
  const unsigned int c_nb_values = 300;

  //Datenspeicher von R lesbar
  SEXP result = PROTECT(allocVector(REALSXP, c_nb_values*column));

  // Distance
  double dist = betrag(REAL(Ziel_Schussebenen)[0], REAL(Ziel_Schussebenen)[1]);

  // init memory to save the results according to distance in respect of time
  unsigned long int  iter = 2 * floor(dist / (asReal(v0) * asReal(t)));

  //Pointers to memory
  long double* p_vx = (long double*)calloc(iter, sizeof(long double));
  long double* p_vy = (long double*)calloc(iter, sizeof(long double));
  long double* p_sx = (long double*)calloc(iter, sizeof(long double));
  long double* p_sy = (long double*)calloc(iter, sizeof(long double));
  long double* p_t = (long double*)calloc(iter, sizeof(long double));

  //Beschleunigung
  long double  a = (-1) * (pow(asReal(v0), 2) * asReal(k)) / asReal(m);

  //Geschwindigkeitsänderung
  long double  dv_x = a * cos(asReal(angle_Schussebenen)) * asReal(t);
  long double  dv_y = (a * sin(asReal(angle_Schussebenen)) + g) * asReal(t); // -Erdbeschleunigung -9.81

  //Flugbahn inital
  p_vx[0] = asReal(v0)*cos(asReal(angle_Schussebenen)); // Anfangsgeschwindigkeit v_x
  p_vy[0] = asReal(v0)*sin(asReal(angle_Schussebenen)); // Anfangsgeschwindigkeit v_y
  p_vx[1] = p_vx[0] + dv_x;   // Init Geschwindigkeit v_x + Geschwindigkeitsänderung
  p_vy[1] = p_vy[0] + dv_y;   // Init Geschwindigkeit v_x + Geschwindigkeitsänderung
  p_sx[0] = 0.0;  // init Position s_x
  p_sy[0] = 0.0;  // init Position s_y
  p_sx[1] = p_vx[0] * asReal(t); // Position s_x
  p_sy[1] = p_vy[0] * asReal(t); // Position s_y
  p_t[0] = 0.0; // t = 0
  p_t[1] = asReal(t);

  // Flugbahn
  for (unsigned int long i = 2; i < iter; i++) {
    // Flugwinkel
    double long angle_ = atan((p_sy[i - 2] - p_sy[i - 1]) / (p_sx[i - 2] - p_sx[i - 1]));
    // letzte Distanz
    double long dist_ = betrag(REAL(Ziel_Schussebenen)[0] - p_sx[i - 1], REAL(Ziel_Schussebenen)[1] - p_sy[i - 1]);
    // letzte Geschwindigkeit
    double long v0_ = betrag(p_vx[i - 1], p_vy[i - 1]);
    // Luftwiderstand (Beschleunigung entgegen der Flugrichtung)
    double long a = -(1) * (pow(v0_, 2) * asReal(k)) / asReal(m);
    //Geschwindigkeitsänderung
    double long dv_x = a * cos(angle_) * asReal(t);
    double long dv_y = (a * sin(angle_) - 9.81) * asReal(t);   // -Erdbeschleunigung -9.81

    // Neue Geschwindigkeit
    p_vx[i] = p_vx[i - 1] + dv_x; // alte Geschwindigkeit x + delta v_x
    p_vy[i] = p_vy[i - 1] + dv_y; // alte Geschwindigkeit y + delta v_y
    // Neue Position
    p_sx[i] = p_sx[i - 1] + p_vx[i] * asReal(t); // alte position + v_x*t
    p_sy[i] = p_sy[i - 1] + p_vy[i] * asReal(t); // alte position + v_y*t
    //Zeit
    p_t[i] = i * asReal(t);

    // Abbruch wenn der Flugwinkel fast -90 Grad wird als0 das Geschoss nur noch fällt.
    double  test = round(angle_ / pi * 180 * 1000) / 1000;

    //Abbruch: ist die alte Distanz kleiner als die neue oder der Winkel fast -90Grad
    if (dist_ < betrag(REAL(Ziel_Schussebenen)[0] - p_sx[i], REAL(Ziel_Schussebenen)[1] - p_sy[i]) || test < -89.99) {

      int ratio = round(i / c_nb_values);
      int c_seq[c_nb_values + 1];
      for (int i = 0; i < (c_nb_values) ; i++) {
        c_seq[i] = i * ratio;
      }
      c_seq[c_nb_values] = i;

      //apply data to pointer
      for (unsigned int i = 0; i < c_nb_values; i++) {
        REAL(result)[i + (0 * c_nb_values)] = p_vx[c_seq[i]];
      }
      for (unsigned int i = 0; i < c_nb_values; i++) {
        REAL(result)[i + (1 * c_nb_values)] = p_vy[c_seq[i]];
      }
      for (unsigned int i = 0; i < c_nb_values; i++) {
        REAL(result)[i + (2 * c_nb_values)] = p_sx[c_seq[i]];
      }
      for (unsigned int i = 0; i < c_nb_values; i++) {
        REAL(result)[i + (3 * c_nb_values)] = p_sy[c_seq[i]];
      }
      for (unsigned int i = 0; i < c_nb_values; i++) {
        REAL(result)[i + (4 * c_nb_values)] = p_t[c_seq[i]];
      }
      break;
    }
  }

  free(p_vx);
  free(p_vy);
  free(p_sx);
  free(p_sy);
  free(p_t);

  UNPROTECT(1);
  return result;
}

