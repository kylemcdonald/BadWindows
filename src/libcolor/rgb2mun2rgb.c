/* rgb2mun2rgb.c           august 28 1990
 *
 *   This file contains the functions that will convert RGB value
 *   from the vlw munsell palette into munsell HVC values or munsell values into RGB
 *
 */


#include <stdio.h>
#include <sys/file.h>
#include <math.h>
#include <munsell.h>
#include <cie.h>

#define DEBUG 0


rgb2mun (r, g, b, H, V, C)
     int r, g, b;
     float *H, *V, *C;
{
  float x, y, Y;		/* CIE coordinates */

#if DEBUG
  printf ("\nrgb2mun() r %d  g %d  b %d", r, g, b);
#endif

  if (rgbcie(r, g, b, &x, &y, &Y) == -1) {
    *H = *V = *C = 0.0;

#if DEBUG
    printf("\nrgb2mun() rgbcie() failed");
#endif 

    return (-1);
  }
  if (ciemun(x, y, Y, H, V, C) == BADCIE) {
    *H = *V = *C = 0.0;

#if DEBUG
    printf("\nrgb2mun() ciemun() failed");
#endif

    return (-1);    
  }

#if DEBUG
  printf ("\nMireille est belle quand elle dance!");
  printf ("\n H %.3f  V %.3f  C %.3f", *H, *V, *C);
#endif

  return(0);
}


mun2rgb (H, V, C, r, g, b)
     float H, V, C;
     int *r, *g, *b;
{
  float x, y, Y;		/* CIE coordinates */

  if (muncie(H, V, C, &x, &y, &Y) == -2) {
    *r = *g = *b = 0;

#if DEBUG
    printf("\nmun2rgb() muncie()failed");
#endif

    return (-1);
  }

  if (ciergb(x, y,Y, r, g, b) == -1) {
    *r = *g = *b = 0;

#if DEBUG
    printf("\nmun2rgb() ciergb()failed");
#endif

    return (-1);    
  }
  return(0);
}

