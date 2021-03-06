

/******************************************************************************
Copyright 1990 by the Massachusetts Institute of Technology.  All 
rights reserved.

Developed by the Visible Language Workshop at the Media Laboratory, MIT, 
Cambridge, Massachusetts, with support from Hewlett Packard, DARPA, and Nynex.

For use by Suguru Ishizaki.  
This distribution is approved by Nicholas Negroponte, Director of 
the Media Laboratory, MIT.

Permission to use, copy, or modify these programs and their 
documentation for educational and research purposes only and 
without fee is hereby granted, provided that this copyright notice 
appears on all copies and supporting documentation.  For any other 
uses of this software, in original or modified form, including but not 
limited to distribution in whole or in part, specific prior permission 
must be obtained from MIT.  These programs shall not be used, 
rewritten, or adapted as the basis of a commercial software or 
hardware product without first obtaining appropriate licenses from 
MIT.  MIT makes no representations about the suitability of this 
software for any purpose.  It is provided "as is" without express or 
implied warranty."
******************************************************************************/





/***************************

  HAIRY2.C
  David Small 3/89

  Contains modifications of Russell Greenlee's AA line
  routines to handle hairy brushes a la Steve Strassmann.
  Any confusion in this code is directly atributable to 
  Russell and Straz of course, and problems should be
  directed to /dev/null.

****************************/


#include <stdio.h>
#include <math.h>
#include <starbase.c.h>

struct ColorStruct {
  int r, g, b;
};

struct hairy_brush {
  int bristles;
  int *ink;
  float *opacity;
  struct ColorStruct *color;
};

static struct hairy_brush *brush;
static int init = TRUE;

dip_brush(bristles, ink, variation, r, g, b)
int bristles, ink;
float variation;
int r, g, b;
{
  int i;
  double drand48();
  float middle;
  int total_ink, new_ink;
  float per_ink;

  if ((!init) && (bristles != brush->bristles)) {
    free (brush->ink);
    free (brush->color);
    free (brush->opacity);
    free (brush);
    init = TRUE;
  }

  if (init) {
    if ((brush = (struct hairy_brush *)malloc(sizeof(struct hairy_brush))) == 0)
      printf("Error mallocing brush\n");
    
    brush->bristles = bristles;
    
    if ((brush->ink = (int *)calloc(bristles, sizeof(int))) == 0)
      printf("Error callocing ink\n");
    if ((brush->opacity = (float *)calloc(bristles, sizeof(float))) == 0)
      printf("Error callocing opacity\n");
    if ((brush->color = (struct ColorStruct *)calloc(bristles, sizeof(struct ColorStruct))) == 0)
      printf("Error callocing color\n");
    init = FALSE;
  }


  for (i = 0; i < bristles; i++) {
    new_ink = ink + (int) ((drand48() - 0.5) * variation * ink);
    if (brush->ink[i] < 0) brush->ink[i] = 0;
    total_ink = brush->ink[i] + new_ink;
    per_ink = (float)new_ink / (float)total_ink;

    (brush->color+i)->r = (int) ((r*per_ink) + 
				 ((brush->color+i)->r * (1.0 - per_ink)));
    (brush->color+i)->g = (int) ((g*per_ink) + 
				 ((brush->color+i)->g * (1.0 - per_ink)));
    (brush->color+i)->b = (int) ((b*per_ink) + 
				 ((brush->color+i)->b * (1.0 - per_ink)));

    if ((brush->color+i)->r > 255)
      printf("r = %d\n", (brush->color+i)->r);
    brush->ink[i] = total_ink;
/*    brush->opacity[i] = 1.0 - (variation * 0.5 * drand48());*/
    brush->opacity[i] = 1.0 - (0.4 * drand48());
  }

}

show_brush(fildes, x1, y1)
int fildes, x1, y1;
{
  int x, y;
  
  fill_color(fildes, 0.2, 0.2, 0.2);
  interior_style(fildes, 1, 0);
  rectangle(fildes, (float) x1, (float) y1, 
	    (float) x1 + 50.0, (float) y1 + 100.0);

  for (x = 0; x < brush->bristles; x += 2) {
    line_color(fildes, (brush->color+x)->r / 255.0, 
	       (brush->color+x)->g / 255.0, 
	       (brush->color+x)->b / 255.0);
    move2d(fildes, (float) (x/2 + x1), (float) y1);
    draw2d(fildes, (float) (x/2 + x1), (float) y1 + (float) brush->ink[x] * 0.1);
  }
  make_picture_current(fildes);
}


float get_ink(near, del, width, p, swapped, r, g, b)
float p;
int width, near, swapped, del, *r, *g, *b;
{
  float ink = 0.0, bristle_wd;
  int bristle1, bristle2;
  int i, count = 0, count2 = 0;

  del = del >> 12;
  
  near = -near;
  *r = 0;
  *g = 0;
  *b = 0;

  bristle_wd = (float)width / (float)brush->bristles;
  if (bristle_wd > 0) {
    bristle1 = (int) (near / bristle_wd);
    bristle2 = (int) ((near + del) / bristle_wd);
    
    if (swapped) {
      bristle1 = brush->bristles - bristle1;
      bristle2 = brush->bristles - bristle2;
    }
    
    if (bristle1 < 0) bristle1 = 0;
    if (bristle1 >= brush->bristles) bristle1 = brush->bristles - 1;
    
    if (bristle2 < 0) bristle2 = 0;
    if (bristle2 >= brush->bristles) bristle2 = brush->bristles - 1;
    
    if (bristle1 < bristle2)
      for (i = bristle1; i <= bristle2; i++) {
	if (brush->ink[i] > 0) {
	  ink += brush->opacity[i];
	  /*	  brush->ink[i] -= 1;*/
	  *r += (brush->color+i)->r;
	  *g += (brush->color+i)->g;
	  *b += (brush->color+i)->b;
	  count2++;
	}
	count++;
      }
    else if (bristle1 > bristle2) 
      for (i = bristle2; i <= bristle1; i++) {
	if (brush->ink[i] > 0) {
	  ink += brush->opacity[i];
	  /*	  brush->ink[i] -= 1;*/
	  *r += (brush->color+i)->r;
	  *g += (brush->color+i)->g;
	  *b += (brush->color+i)->b;
	  count2++;
	}
	count++;
      }
    if (count)
      ink /= count;
    if (count2) {
      *r /= count2;
      *g /= count2;
      *b /= count2;
    }
    return(ink);
  }
  else
    return(0.0);
}

remove_ink(amount)
int amount;
{
  int b, count = 0;

  for (b = 0; b < brush->bristles; b++) 
    brush->ink[b] -= amount;
}

/********


float get_ink(near, del, width, p, swapped)
float p;
int width, near, swapped, del;
{
  float ink = 0.0, bristle_wd, bristle1, bristle2;
  int b, count = 0;

  del = del >> 12;
  
  near = -near;

  bristle_wd = (float)width / (float)brush->bristles;
  bristle1 = (float) near / bristle_wd;
  bristle2 = (float) (near + del) / bristle_wd;

  if (bristle1 < 0.0) bristle1 = 0.0;
  if (bristle1 >= brush->bristles) bristle1 = brush->bristles - 1;

  if (bristle2 < 0.0) bristle2 = 0.0;
  if (bristle2 >= brush->bristles) bristle2 = brush->bristles - 1;

  if (floor(bristle1) == floor(bristle2)) {
    b = (int) (floor(bristle1));
    if (brush->ink[b] > 0) {
      ink = brush->opacity[b];
      brush->ink[b] -= 1;
    }
  }
  else if (bristle1 < bristle2) {
    for (b = (int) (floor(bristle1)); b <= (int) (floor(bristle2)); b++) {
      if (brush->ink[b] > 0) {
	ink += brush->opacity[b];
	brush->ink[b] -= 1;
      }
      count++;
    }
    ink /= count;
  }
  else if (bristle1 > bristle2) {
    for (b = (int) (floor(bristle2)); b <= (int) (floor(bristle1)); b++) {
      if (brush->ink[b] > 0) {
	ink += brush->opacity[b];
	brush->ink[b] -= 1;
      }
      count++;
    }
    ink /= count;
  }

  return(ink);
}

*************************/
