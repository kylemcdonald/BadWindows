
#include <stdio.h>
#include <starbase.c.h>
#include <windows.c.h>
#include <structures.h>
#include <math.h>


BevelRectify(W, light,bevel,red, green, blue, border, bred, bgreen, bblue, shadow)     /* v 1.0b */
     struct Window *W;
     int red, green, blue, border, bred, bgreen, bblue, shadow,light,bevel;
{
  int BevelRectWin();
  struct bevelstruct *parms;


  parms = (struct bevelstruct *)malloc(sizeof(struct bevelstruct));
  parms->red = red;
  parms->green = green;
  parms->blue = blue;
  parms->border = border;
  parms->bred = bred;
  parms->bgreen = bgreen;
  parms->bblue = bblue;
  parms->shadow = shadow;
  parms->bevel = bevel;
  parms->light = light;		/* *DKY-25Apr90* */

  parms->lred = MIN(MAX(red + light,0),255);
  parms->lgreen = MIN(MAX(green + light,0),255);
  parms->lblue = MIN(MAX(blue + light,0),255);

  parms->rred = MIN(MAX(red - light,0),255);
  parms->rgreen = MIN(MAX(green - light,0),255);
  parms->rblue = MIN(MAX(blue - light,0),255);

  AttachCommand(W, DRAW, BevelRectWin, parms);
  AttachCommand(W, UPDATE, BevelRectWin, parms);
  AttachCommand(W, FLIP_BEVEL, BevelRectWin, parms); /* *DKY-25Apr90* */
  AttachCommand(W, SET_LIGHT, BevelRectWin, parms); /* *DKY-25Mar91* */
  AttachCommand(W, NEWCOLOR, BevelRectWin, parms); /* *DKY-25Mar91* */
}



BevelRectWin(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct clipstruct *cstruct;
  struct bevelstruct *parms;
  int screen, cx1, cx2, cy1, cy2, *colors;
  float red, green, blue;
  int bd, s,x,y,noline = 0,crxflag = 0;
  int border, shadow, bevel;

  screen = (W->display)->fildes;
  switch (id)
    {
    case UPDATE:
      UpdateWin(W);
      break;

    case FLIP_BEVEL:		/* *DKY-25Apr90* */
      parms = (struct bevelstruct *)stuff;
      parms->light = -parms->light;

      parms->lred = MIN(MAX(parms->red + parms->light,0),255);
      parms->lgreen = MIN(MAX(parms->green + parms->light,0),255);
      parms->lblue = MIN(MAX(parms->blue + parms->light,0),255);

      parms->rred = MIN(MAX(parms->red - parms->light,0),255);
      parms->rgreen = MIN(MAX(parms->green - parms->light,0),255);
      parms->rblue = MIN(MAX(parms->blue - parms->light,0),255);

      break;


    case SET_LIGHT:		/* *DKY-25Mar91* */
      parms = (struct bevelstruct *)stuff;
      parms->light = (int)data;;

      parms->lred = MIN(MAX(parms->red + parms->light,0),255);
      parms->lgreen = MIN(MAX(parms->green + parms->light,0),255);
      parms->lblue = MIN(MAX(parms->blue + parms->light,0),255);

      parms->rred = MIN(MAX(parms->red - parms->light,0),255);
      parms->rgreen = MIN(MAX(parms->green - parms->light,0),255);
      parms->rblue = MIN(MAX(parms->blue - parms->light,0),255);

      break;


    case NEWCOLOR:		/* *DKY-25Mar91* */
      {
	colors = (int *)data;

	parms = (struct bevelstruct *)stuff;
	parms->red = *(colors);
	parms->green = *(colors+1);
	parms->blue = *(colors+2);
      }
      break;

      
    case DRAW:
      cstruct = (struct clipstruct *)data;

      cx1 = cstruct->x1;
      cy1 = cstruct->y1;
      cx2 = cstruct->x2;
      cy2 = cstruct->y2;

      clip_rectangle(screen, (float)cx1, (float)cx2, (float)cy1, (float)cy2);

    default:
      cx1 = W->x1;
      cy1 = W->y1;
      cx2 = W->x2;
      cy2 = W->y2;

      parms = (struct bevelstruct *)stuff;
      bevel = parms->bevel;
      shadow = parms->shadow;
      border = parms->border;


      parms->lred = MIN(MAX(parms->red + parms->light,0),255);
      parms->lgreen = MIN(MAX(parms->green + parms->light,0),255);
      parms->lblue = MIN(MAX(parms->blue + parms->light,0),255);

      parms->rred = MIN(MAX(parms->red - parms->light,0),255);
      parms->rgreen = MIN(MAX(parms->green - parms->light,0),255);
      parms->rblue = MIN(MAX(parms->blue - parms->light,0),255);


      /* Draw the Shape */


      cx2 -= shadow; cy2 -= shadow;
      red = ((float)parms->red)/255.0;
      green = ((float)parms->green)/255.0;
      blue = ((float)parms->blue)/255.0;
      fill_color(screen, red, green, blue);
    
      if (parms->bred == -1)
	{
	  red = 0.0;
	  noline = 1;
	}
      else red = ((float)parms->bred)/255.0;
      green = ((float)parms->bgreen)/255.0;
      blue = ((float)parms->bblue)/255.0;
      perimeter_color(screen, red, green, blue);
      line_color(screen, red, green, blue);
    
      if ((border<=0)||(noline))
	{
	  interior_style(screen, INT_SOLID, FALSE);
	  if (W->display->bits & STARBUG) crxflag = 1;
	}
      else
	interior_style(screen, INT_SOLID, TRUE);
    
      if ((cx1+bevel != cx2-bevel)&&(cy1+bevel != cy2-bevel))
	rectangle(screen, (float)(cx1+bevel+border-crxflag), (float)(cy1+bevel+border-crxflag),
		  (float)(cx2-bevel-border),(float)(cy2-bevel-border));
      else {move2d(screen, cx1, cy1); draw2d(screen, cx2, cy2);}
    
      interior_style(screen, INT_HOLLOW, TRUE);

      /* Bevels */
      red = ((float)parms->lred)/255.0;
      green = ((float)parms->lgreen)/255.0;
      blue = ((float)parms->lblue)/255.0;
      line_color(screen, red, green, blue);
      for (s = 0; s < bevel; s++)
	{
	  move2d(screen, (float)(cx1+border+s), (float)(cy1+border+s));
	  draw2d(screen, (float)(cx2-border-s), (float)(cy1+border+s));
	  move2d(screen, (float)(cx1+border+s), (float)(cy1+border+s));
	  draw2d(screen, (float)(cx1+border+s), (float)(cy2-border-s));
	}

      red = ((float)parms->rred)/255.0;
      green = ((float)parms->rgreen)/255.0;
      blue = ((float)parms->rblue)/255.0;
      line_color(screen, red, green, blue);
      for (s = 0; s < bevel; s++)
	{
	  move2d(screen, (float)(cx2-border-s), (float)(cy2-border-s));
	  draw2d(screen, (float)(cx2-border-s), (float)(cy1+border+s));
	  move2d(screen, (float)(cx2-border-s), (float)(cy2-border-s));
	  draw2d(screen, (float)(cx1+border+s), (float)(cy2-border-s));
	}

      if (parms->bred == -1)
	{
	  red = 0.0;
	  noline = 1;
	}
      else red = ((float)parms->bred)/255.0;
      green = ((float)parms->bgreen)/255.0;
      blue = ((float)parms->bblue)/255.0;
      perimeter_color(screen, red, green, blue);
      line_color(screen, red, green, blue);

      /* diagonals */
      if (!noline ) {  /* SUPERKLUGE!!!!*/
	move2d(screen, (float)(cx1+border), (float)(cy1+border));
	draw2d(screen, (float)(cx1+border+bevel), (float)(cy1+border+bevel));
	
	move2d(screen, (float)(cx2-border), (float)(cy1+border));
	draw2d(screen, (float)(cx2-border-bevel), (float)(cy1+border+bevel));
	
	move2d(screen, (float)(cx2-border), (float)(cy2-border));
	draw2d(screen, (float)(cx2-border-bevel), (float)(cy2-border-bevel));
	
	move2d(screen, (float)(cx1+border), (float)(cy2+border));
	draw2d(screen, (float)(cx1+border+bevel), (float)(cy2-border-bevel));
      }
      for (bd = 0; bd < abs(parms->border); bd++)
	rectangle(screen, (float)(cx1+bd), (float)(cy1+bd), (float)(cx2-bd), (float)(cy2-bd));
    
      for (s = 0; s < shadow; s++)
	{
	  move2d(screen, (float)(cx2+s+1), (float)(cy1+s+1));
	  draw2d(screen, (float)(cx2+s+1), (float)(cy2+s+1));
	  move2d(screen, (float)(cx1+s+1), (float)(cy2+s+1));
	  draw2d(screen, (float)(cx2+s+1), (float)(cy2+s+1));
	}

      /* End Draw */

      clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		     0.0, (float)((W->display)->height-1));
      make_picture_current(screen);

      break;
    }
  return(0);
}




/**********************************************************************
 *  HatchRectify()
 *
 *  'type' can be the values:  CROSSHATCH, PARALLEL_HATCH
 *  'vector' is to specify the angle of the hatch marks.  a vector of 1.0 1.0 is a 45degree angle.
 *
 * *DKY-30Jul90*
 **********************************************************************/





Hatchify( W, type, spacing, xvector, yvector, border, shadow, hatchR, hatchG, hatchB)
     struct Window *W;
     int type, spacing, hatchR, hatchG, hatchB, border, shadow, xvector, yvector;
{
  struct HatchRectStruct *parms;
  int HatchRectWin();

  parms = (struct HatchRectStruct *)malloc( sizeof( struct HatchRectStruct));
  parms->type = type;
  parms->shadow = shadow;
  parms->border = border;
  parms->hatchR = hatchR;
  parms->hatchG = hatchG;
  parms->hatchB = hatchB;
  parms->spacing = spacing;
  parms->xvector = xvector;
  parms->yvector = yvector;

  AttachCommand(W, DRAW, HatchRectWin, parms);
}


HatchRectWin(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct clipstruct *cstruct;
  struct HatchRectStruct *parms;
  int screen, cx1, cx2, cy1, cy2, *colors;
  float red, green, blue;

  screen = (W->display)->fildes;

  switch (id)    {
  case (UPDATE):
    UpdateWin(W);
    break;

  case (DRAW):
    cstruct = (struct clipstruct *)data;

    cx1 = cstruct->x1;
    cy1 = cstruct->y1;
    cx2 = cstruct->x2;
    cy2 = cstruct->y2;

    clip_rectangle(screen, (float)cx1, (float)cx2, (float)cy1, (float)cy2);

  default:
    cx1 = W->x1;
    cy1 = W->y1;
    cx2 = W->x2;
    cy2 = W->y2;

    parms = (struct HatchRectStruct *)stuff;


    hatch_type( screen, parms->type);
    hatch_spacing( screen, (float)(parms->spacing), VDC_UNITS);
    hatch_orientation( screen, (float)(parms->xvector), (float)(parms->yvector));

    drawing_mode( screen, 3);
    fill_color(screen, (float)(parms->hatchR) / 255.0, (float)(parms->hatchG) / 255.0, (float)(parms->hatchB) / 255.0);
    interior_style(screen, INT_HATCH, FALSE);
    rectangle(screen, (float)W->x1 + (float)parms->border, (float)W->y1 + (float)parms->border,
	      (float)W->x2 - (float)parms->border - (float)parms->shadow,
	      (float)W->y2 - (float)parms->border - (float)parms->shadow);
    drawing_mode(screen, 3);
    make_picture_current( screen);
    perimeter_type( screen, 0);

    clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		   0.0, (float)((W->display)->height-1));
    make_picture_current(screen);

    break;
  }
  return (0);
}
