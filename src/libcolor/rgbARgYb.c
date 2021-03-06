/*******************************************************************************************
*                                                                                          *
* Following are the facilities to convert RGB values into ARgYb color space.               *
* ARgYb stands for Achromatic, Red-Green, Yellow-Blue. This color space is based on the    *
* opponent theory of human color perception.  One of its advantage is that it expresses    *
* the luminance component of color independently of the chromatic information; very        *
* convenient for color contrast adjustment.                                                *
*                                                                                          *
********************************************************************************************/

#include <stdio.h>
#include <starbase.c.h>
#include <string.h>
#include <math.h>
#include "windows.c.h"
#include "structures.h"



typedef struct {
  float x, y, z;
}FloatVector;

typedef struct {
  float x1, y1, z1;
  float x2, y2, z2;
  float x3, y3, z3;
}FloatMatrix;




SetVectValues (vector, x, y, z)
FloatVector *vector;
float x, y, z;
{
  vector->x = x;
  vector->y = y;
  vector->z = z;
}




SetMatrixValue (matrix, x1, y1, z1, x2, y2, z2, x3, y3, z3)
FloatMatrix *matrix;
float x1, y1, z1, x2, y2, z2, x3, y3, z3;
{
  matrix->x1 = x1;
  matrix->y1 = y1;
  matrix->z1 = z1;
  matrix->x2 = x2;
  matrix->y2 = y2;
  matrix->z2 = z2;
  matrix->x3 = x3;
  matrix->y3 = y3;
  matrix->z3 = z3;
}  




MatVectMult (mat, vect, vector)
FloatMatrix mat;
FloatVector vect, *vector;
{
  vector->x = mat.x1 * vect.x + mat.y1 * vect.y + mat.z1 * vect.z;
  vector->y = mat.x2 * vect.x + mat.y2 * vect.y + mat.z2 * vect.z;
  vector->z = mat.x3 * vect.x + mat.y3 * vect.y + mat.z3 * vect.z;
}




rgb_to_ARgYb (r, g, b, A, Rg, Yb)
int r, g, b;
float *A, *Rg, *Yb;
{
  FloatVector rgb, ARgYb;
  FloatMatrix M;

  /*printf( "\n---------r %d  g %d  b%d", r, g, b);*/
  SetVectValues (&rgb, (float) r, (float) g, (float) b);

  SetMatrixValue (&M, 0.30, 0.59, 0.11, 0.50, -0.50, 0.00, 0.25, 0.25, -0.50);
  
  MatVectMult (M, rgb, &ARgYb);

  /*printf( "\n---------A %.3f  Rg %.3f  Yb %.3f", ARgYb.x, ARgYb.y, ARgYb.z);*/

  *A = ARgYb.x;
  *Rg = ARgYb.y;
  *Yb = ARgYb.z;
}




ARgYb_to_rgb (A, Rg, Yb, r, g, b)
float A, Rg, Yb;
int *r, *g, *b;
{
  FloatVector rgb, ARgYb;
  FloatMatrix M;

  /*printf( "\n---------------------A %.3f  Rg %.3f  Yb %.3f", A, Rg, Yb);*/
  SetVectValues (&ARgYb, A, Rg, Yb);

  SetMatrixValue (&M, 1.00, 1.29, 0.22, 1.00, -0.71, 0.22, 1.00, 0.29, -1.78);

  MatVectMult (M, ARgYb, &rgb);

  /*printf( "\n---------------------r %.3f  g %.3f  b %.3f", rgb.x, rgb.y, rgb.z);*/

  *r = (int) rgb.x;
  *g = (int) rgb.y;
  *b = (int) rgb.z;
}
