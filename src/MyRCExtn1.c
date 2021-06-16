#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
//#include <stddef.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Memory.h>

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
SEXP FlugbahnV3 (SEXP v0, SEXP t ,SEXP angle_Schussebenen, SEXP Ziel_Schussebenen,SEXP m, SEXP k);
SEXP foo(SEXP x, SEXP c_lenght);

R_CallMethodDef callMethods[] =
{
	// C functions extended by using .Call interface
	{ "Flugbahn",   (DL_FUNC)&Flugbahn,  6 },
	{ "foo",   (DL_FUNC)&foo,  2 },
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
  return sqrt(R_pow(x,2)+R_pow(y,2));
}

double long a_t (double long t,double long v0[], double m, double k){
  return (-1)*R_pow(betrag(v0[0],v0[1]),2)*k/m;
  }

//////// Function to be used with .Call interface ////////////

SEXP Flugbahn (SEXP v0, SEXP t ,SEXP angle_Schussebenen, SEXP Ziel_Schussebenen,SEXP m, SEXP k)
{
  // Werte die abgespeichert werden und an R zurueckgegeben werden
  const unsigned int column = 5;
  const unsigned int c_nb_values = 200;

  //Datenspeicher von R lesbar
  SEXP result = PROTECT(allocVector(REALSXP, c_nb_values*column));

  // Distance
  double dist = betrag(REAL(Ziel_Schussebenen)[0], REAL(Ziel_Schussebenen)[1]);

  // init memory to save the results according to distance in respect of time
  unsigned long int  iter = 10 * round(dist / (asReal(v0) * asReal(t)));

  // Pointers to user controlled memory in R
  // https://colinfay.me/writing-r-extensions/the-r-api-entry-points-for-c-code.html
  // 6.1.2 User-controlled memory
  double* p_vx = (double *)Calloc(iter, double);
  double* p_vy = (double *)Calloc(iter, double);
  double* p_sx = (double *)Calloc(iter, double);
  double* p_sy = (double *)Calloc(iter, double);
  double* p_t  = (double *)Calloc(iter, double);

  //Beschleunigung
  double  a = (-1) * (R_pow(asReal(v0), 2) * asReal(k)) / asReal(m);

  //Geschwindigkeitsänderung
  double  dv_x = a * cos(asReal(angle_Schussebenen)) * asReal(t);
  double  dv_y = (a * sin(asReal(angle_Schussebenen)) + g) * asReal(t); // -Erdbeschleunigung -9.81

  //Flugbahn inital
  p_vx[0] = asReal(v0)*cos(asReal(angle_Schussebenen)); // Anfangsgeschwindigkeit v_x
  p_vy[0] = asReal(v0)*sin(asReal(angle_Schussebenen)); // Anfangsgeschwindigkeit v_y
  p_vx[1] = p_vx[0] + dv_x;   // Init Geschwindigkeit v_x + Geschwindigkeitsänderung
  p_vy[1] = p_vy[0] + dv_y;   // Init Geschwindigkeit v_x + Geschwindigkeitsänderung
  p_sx[0] = 0;  // init Position s_x
  p_sy[0] = 0;  // init Position s_y
  p_sx[1] = p_vx[0] * asReal(t); // Position s_x
  p_sy[1] = p_vy[0] * asReal(t); // Position s_y
  p_t[0]  = 0; // t = 0
  p_t[1]  = asReal(t);

  // Flugbahn
  unsigned int long cnt = 0;
  for (unsigned int long i = 2; i < iter; i++) {
    // Flugwinkel
    double angle_ = atan((p_sy[i-2] - p_sy[i-1]) / (p_sx[i-2] - p_sx[i-1]));
    // letzte Distanz
    double dist_ = betrag(REAL(Ziel_Schussebenen)[0] - p_sx[i-1], REAL(Ziel_Schussebenen)[1] - p_sy[i-1]);
    // letzte Geschwindigkeit
    double v0_ = betrag(p_vx[i-1], p_vy[i-1]);
    // Luftwiderstand (Beschleunigung entgegen der Flugrichtung)
    double a = -(1) * (R_pow(v0_, 2) * asReal(k)) / asReal(m);
    //Geschwindigkeitsänderung
    double dv_x = a * cos(angle_) * asReal(t);
    double dv_y = (a * sin(angle_) - 9.81) * asReal(t);   // -Erdbeschleunigung -9.81

    // Neue Geschwindigkeit
    p_vx[i] = p_vx[i - 1] + dv_x; // alte Geschwindigkeit x + delta v_x
    p_vy[i] = p_vy[i - 1] + dv_y; // alte Geschwindigkeit y + delta v_y
    // Neue Position
    p_sx[i] = p_sx[i - 1] + p_vx[i] * asReal(t); // alte position + v_x*t
    p_sy[i] = p_sy[i - 1] + p_vy[i] * asReal(t); // alte position + v_y*t
    //Zeit
    p_t[i] = i * asReal(t);

    // Solange das Geschoss aufsteigt darf nicht abgebrochen werden
    if (!(p_vy[i] >= 0)) {
      // Abbruch: ist die alte Distanz kleiner als die neue oder der Winkel fast -90Grad
      // Abbruch wenn der Flugwinkel fast -90 Grad wird als0 das Geschoss nur noch fällt.
      // Abbruch wenn for loop fertig
      if (dist_ < betrag(REAL(Ziel_Schussebenen)[0] - p_sx[i], REAL(Ziel_Schussebenen)[1] - p_sy[i]) || round(angle_ / pi * 180 * 1000) / 1000 < -89.99 || i == iter-1) {

        int ratio = floor(i / c_nb_values-1);
        int c_seq[c_nb_values];
        for (int i = 0; i < (c_nb_values) ; i++) {
          c_seq[i] = i * ratio;
        }
        c_seq[c_nb_values-1] = i;

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

    }else {
      // Abbruch wenn am Ziel vorbei
      if (p_sx[i] > REAL(Ziel_Schussebenen)[0]) {

        int ratio = floor(i / c_nb_values-1);
        int c_seq[c_nb_values];
        for (int i = 0; i < (c_nb_values) ; i++) {
          c_seq[i] = i * ratio;
        }
        c_seq[c_nb_values-1] = i;

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
    cnt = cnt+1;
  }
  Free(p_vx);
  Free(p_vy);
  Free(p_sx);
  Free(p_sy);
  Free(p_t);

  UNPROTECT(1);
  return result;
}

SEXP foo(SEXP x, SEXP c_lenght)
{
  // User-controlled memory
  // https://colinfay.me/writing-r-extensions/the-r-api-entry-points-for-c-code.html
  double* p_x = (double *)Calloc(asReal(c_lenght)+10, double);

  //init
  for(unsigned long int i = 0; i < asReal(c_lenght)+10; i++){
    p_x[i] = 0.000001;
  }

  //assigne values
  for(unsigned long int i = 0; i < asReal(c_lenght); i++){
    *(p_x+i) = REAL(x)[i];
  }

  //Datenspeicher von R lesbar
  SEXP result = PROTECT(allocVector(REALSXP,asReal(c_lenght)+10));

  //apply data to vector
  for(unsigned long int i = 0; i < asReal(c_lenght)+10; i++){
    REAL(result)[i] = p_x[i];
  }

  Free(p_x);
  UNPROTECT(1);
  return result;
}

SEXP penulum_get_theta(SEXP t, SEXP L, SEXP delta_t, SEXP THETA_0, SEXP THETA_DOT_0, SEXP mu)
{
    for(unsigned long int i = 0; i < )
}

