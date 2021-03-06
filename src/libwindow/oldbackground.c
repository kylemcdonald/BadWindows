/* Off-screen background storage for two-screens */

#include <stdio.h>
#include <starbase.c.h>
#include <windows.c.h>
#include <structures.h>

static struct OutDev *screen[2] = {NULL, NULL}, *E_current = NULL;
static int E_maxwide[2] = {NULL, NULL};
static int E_maxtall[2] = {NULL, NULL};
static int E_sourcex[2], E_sourcey[2];
static int E_wide[2], E_tall[2];
static int E_oldx[2], E_oldy[2];


ReserveBackgroundSpace(O, x1, y1, x2, y2)     /* v 1.0b */
struct OutDev *O;
int x1, y1, x2, y2;
{
  int i, orientation;

  if (!screen[0]) i = 0;  /* if one screen's taken, fill in the other */
  else i = 1;

  screen[i] = O;
  E_maxwide[i] = x2-x1+1;
  E_maxtall[i] = y2-y1+1;
  E_sourcex[i] = x1;
  E_sourcey[i] = y1;
}


SaveBackground(W)     /* v 1.0b */
struct Window *W;
{
  int i;
  struct OutDev *O;

  O = W->display;
  if (O == screen[0]) i = 0;
  else if (O == screen[1]) i = 1;
  else printf("ERROR: screen value not known\n");

  if ((W->width > E_maxwide[i]) || (W->height > E_maxtall[i]))
    {
    E_current = NULL; /* not enough space */
    }
  else
    {
      E_current = O;
      E_oldx[i] = W->x1;
      E_oldy[i] = W->y1;
      E_wide[i] = W->width;
      E_tall[i] = W->height;
      dcblock_move(O->fildes,
		   W->x1, W->y1,
		   W->width, W->height,
		   E_sourcex[i], E_sourcey[i]);
    }
}


RestoreBackground()     /* v 1.0b */
{
  int val = 1, i;
  struct OutDev *O;

  O = E_current;
  if (O)
    {
      if (O == screen[0]) i = 0;
      else if (O == screen[1]) i = 1;
      dcblock_move(O->fildes, E_sourcex[i], E_sourcey[i],
		   E_wide[i], E_tall[i], E_oldx[i], E_oldy[i]);
      make_picture_current(O->fildes);
      E_current = NULL;
      val = 0;
    }
  return (val);
}

