#include <stdio.h>
#include <starbase.c.h>
#include <windows.c.h>
#include <structures.h>
#include <math.h>

static int gamm_init = 1;
static float gamma_table[256];

struct remoterectstruct
{
  int *red, *green, *blue;
  int bred, bgreen, bblue;
  int border, shadow;
};


Rectify(W, red, green, blue, border, bred, bgreen, bblue, shadow)     /* v 1.0b */
     struct Window *W;
     int red, green, blue, border, bred, bgreen, bblue, shadow;
{
  int RectWin();
  struct rectstruct *parms;

  parms = (struct rectstruct *)malloc(sizeof(struct rectstruct));
  parms->red = red;
  parms->green = green;
  parms->blue = blue;
  parms->border = border;
  parms->bred = bred;
  parms->bgreen = bgreen;
  parms->bblue = bblue;
  parms->shadow = shadow;

  AttachCommand(W, DRAW, RectWin, parms);
  AttachCommand(W, NEWCOLOR, RectWin, parms);
  AttachCommand(W, UPDATE, RectWin, parms);
  AttachCommand(W, GETCOLOR, RectWin, parms);
  AttachCommand(W, SET_SHADOW, RectWin, parms);
  AttachCommand(W, CHANGE_BORDER, RectWin, parms); /* Lindi Emoungu 1/11/91 */
}

UnRectify(W)     /* JSH 17 May 90 */
     struct Window *W;
{
  struct rectstruct *r;
  int RectWin();

  r = (struct rectstruct *)GetArguments(W,DRAW,RectWin);
  if(r) free(r);
  DetachCommand(W, DRAW,RectWin);

  r = (struct rectstruct *)GetArguments(W,NEWCOLOR,RectWin);
  if(r) free(r);
  DetachCommand(W, NEWCOLOR,RectWin);

  r = (struct rectstruct *)GetArguments(W,UPDATE,RectWin);
  if(r) free(r);
  DetachCommand(W, UPDATE,RectWin);

  r = (struct rectstruct *)GetArguments(W,GETCOLOR,RectWin);
  if(r) free(r);
  DetachCommand(W, GETCOLOR,RectWin);
}

RectWin(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct clipstruct *cstruct;
  struct rectstruct *parms;
  int screen, cx1, cx2, cy1, cy2, *colors;
  float red, green, blue,lineoff;
  int border, shadow;
  screen = (W->display)->fildes;
  switch (id)    {
  case (UPDATE):
    UpdateWin(W);
    break;
  case (GETCOLOR):		/* *DKY-12May90* */
    /* data is a pointer to an array of 3 integers */
    colors = (int *)data;
    parms = (struct rectstruct *)stuff;

    *colors = parms->red;
    *(colors+1) = parms->green;
    *(colors+2) = parms->blue;
    break;
  case (NEWCOLOR):
    colors = (int *)data;
    parms = (struct rectstruct *)stuff;

    parms->red = *(colors);
    parms->green = *(colors+1);
    parms->blue = *(colors+2);
    /* UpdateWin(W); removed 11/13/89 - dsmall */
    break;

  case SET_SHADOW:		/* *DKY- 2Nov90* */
    parms = (struct rectstruct *)stuff;
    parms->shadow = (int)data;
    break;
  case (CHANGE_BORDER):  /* Change Border Added by Lindi Emoungu 1/11/91 */
    colors = (int *)data;
    parms = (struct rectstruct *)stuff;
    
    parms->bred = *(colors);
    parms->bgreen = *(colors + 1);
    parms->bblue = *(colors +2);
    break; 

  case (DRAW):
    cstruct = (struct clipstruct *)data;

    cx1 = cstruct->x1;
    cy1 = cstruct->y1;
    cx2 = cstruct->x2;
    cy2 = cstruct->y2;

    clip_rectangle(screen, (float)cx1, (float)cx2, (float)cy1, (float)cy2);
    make_picture_current(screen);

  default:
    cx1 = W->x1;
    cy1 = W->y1;
    cx2 = W->x2;
    cy2 = W->y2;

    parms = (struct rectstruct *)stuff;

    if ((W->display->bits&STARBUG)&&(!parms->border)) lineoff = 1.0;
    else lineoff = 0.0;
  
    DrawRect(screen, (float)cx1-lineoff, (float)cy1-lineoff, (float)cx2, (float)cy2, parms->red, parms->green, parms->blue,
	     parms->border, parms->bred, parms->bgreen, parms->bblue, parms->shadow);

    clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		   0.0, (float)((W->display)->height-1));
    make_picture_current(screen);

    break;
  }
  return (0);
}



DrawRect(screen, x1, y1, x2, y2, r, g, b, border, br, bg, bb, shadow)     /* v 1.0b */
     int screen, r, g, b, border, br, bg, bb, shadow;
     float x1, y1, x2, y2;
{
  float red, green, blue;
  int bd, s;

  x2 -= shadow; y2 -= shadow;
  red = ((float)r)/255.0;
  green = ((float)g)/255.0;
  blue = ((float)b)/255.0;
  fill_color(screen, red, green, blue);

  red = ((float)br)/255.0;
  green = ((float)bg)/255.0;
  blue = ((float)bb)/255.0;
   perimeter_color(screen, red, green, blue);
  line_color(screen, red, green, blue);

  if (border<=0)
    interior_style(screen, INT_SOLID, FALSE);
  else
    interior_style(screen, INT_SOLID, TRUE);

  if ((x1 != x2)&&(y1 != y2))
    rectangle(screen, x1, y1, x2, y2);
  else {move2d(screen, x1, y1); draw2d(screen, x2, y2);}

  interior_style(screen, INT_HOLLOW, TRUE);
  for (bd = 1; bd < abs(border); bd++)
    rectangle(screen, (float)(x1+bd), (float)(y1+bd), (float)(x2-bd), (float)(y2-bd));

  for (s = 0; s < shadow; s++)
    {
      move2d(screen, (float)(x2+s+1), (float)(y1+s+1));
      draw2d(screen, (float)(x2+s+1), (float)(y2+s+1));
      move2d(screen, (float)(x1+s+1), (float)(y2+s+1));
      draw2d(screen, (float)(x2+s+1), (float)(y2+s+1));
    }

}




DrawTriangle(screen, x1, y1, x2, y2, r, g, b, direction)
     int screen, r, g, b, direction;
     float x1, y1, x2, y2;
{
  float red, green, blue, midw, midh;
  float p[6];
  int bd, s;

  red = ((float)r)/255.0;
  green = ((float)g)/255.0;
  blue = ((float)b)/255.0;

  fill_color(screen, red, green, blue);
  interior_style(screen, INT_SOLID, FALSE);
  vertex_format(screen, 0,0,0,0,0);
  midw = x1 + ((x2-x1+1)/2.0);  midh = y1 + ((y2-y1+1)/2.0);

  switch (direction) {

  case UP:
    p[0]=x1;  p[1]=y2;  p[2]=midw;  p[3]=y1;  p[4]=x2;  p[5]=y2;
    break;
  case DOWN:
    p[0]=x1;  p[1]=y1;  p[2]=x2;  p[3]=y1;  p[4]=midw;  p[5]=y2;
    break;
  case LEFT:
    p[0]=x1;  p[1]=midh;  p[2]=x2;  p[3]=y1;  p[4]=x2;  p[5]=y2;
    break;
  case RIGHT:
    p[0]=x1;  p[1]=y1;  p[2]=x2;  p[3]=midh;  p[4]=x1;  p[5]=y2;
    break;
  }
  polygon2d(screen,p,3,0);

}



RoundedBox(fildes, x1, y1, x2, y2, d)     /* v 1.0b */
     int fildes, x1, y1, x2, y2, d;
{
  float line[6];
  int c;

  if (x2<x1) c = x1, x1 = x2, x2 = c;
  if (y2<y1) c = y1, y1 = y2, y2 = c;
  if ((d*2 > x2-x1+1) || (d*2 > y2-y1+1))
    {
      printf("error in roundbox -- too round\n");
      return(1);
    }
  vertex_format(fildes, 0, 0, 0, 0, 0);

  partial_arc(fildes, (float)d, (float)d, (float)(x1+d), (float)(y1+d), M_PI, 1.5*M_PI, 0.0, 0, 0);
  partial_arc(fildes, (float)d, (float)d, (float)(x2-d), (float)(y1+d), -M_PI/2.0, 0.0, 0.0, 0, 0);
  partial_arc(fildes, (float)d, (float)d, (float)(x2-d), (float)(y2-d), 0.0, M_PI/2.0, 0.0, 0, 0);
  partial_arc(fildes, (float)d, (float)d, (float)(x1+d), (float)(y2-d), M_PI/2.0, M_PI, 0.0, 0, 0);

  line[0] = (float)(x2-x1)/2.0;
  line[1] = (float)(y2-y1)/2.0;
  line[2] = 1;
  line[3] = (float)(x2-x1)/2.0;
  line[4] = (float)(y2-y1)/2.0;
  line[5] = 1;
  polygon2d(fildes, line, 2, 1);

  return (0);
}


int RoundRectifyMode = 1;

SetRoundRectifyMode( mode)	/* *DKY-15Sep90* -- cause sometimes I don't want things round rectify-ed. */
     int mode;
{
  RoundRectifyMode = mode;	/* 1 to let RoundRectify be normal, 0 to have it just do a Rectify  */
}


RoundRectify(W, 
	     red, green, blue, 
	     shadow, 
	     shadowred, shadowgreen, shadowblue, 
	     curve)    
     /* v 1.0b */
     struct Window *W;
     int red, green, blue, shadow, shadowred, shadowgreen, shadowblue;
     float curve;
{
  struct roundrectstruct *params;
  int RoundRectWin();

  if( RoundRectifyMode) {
    if(curve == 0.0) {
      Rectify( W, red, green, blue, 1, shadowred, shadowgreen, shadowblue, 0);
    }
    else {
      params = (struct roundrectstruct *)malloc(sizeof(struct roundrectstruct));
      params->red = red;
      params->green = green;
      params->blue = blue;
      params->shadow = shadow;
      params->shadowred = shadowred;
      params->shadowgreen = shadowgreen;
      params->shadowblue = shadowblue;
      params->curvature = curve;
      
      AttachCommand(W, DRAW, RoundRectWin, (char *)params);
      AttachCommand(W, NEWCOLOR, RoundRectWin, (char *)params);
      AttachCommand(W, CHANGE_CURVE, RoundRectWin, (char *)params);
    }
  }
  else
    Rectify( W, red, green, blue, 1, shadowred, shadowgreen, shadowblue, 0);
}



RoundRectWin(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct roundrectstruct *params;
  int x1, x2, y1, y2, screen, shadow, d, way = 0, t;
  float fx1, fx2, fy1, fy2, fd;
  float cx1, cx2, cy1, cy2;
  float red, green, blue, curve;
  struct clipstruct *cstruct;
  unsigned char *control;
  int *colors;

  control = W->display->control;
  params = (struct roundrectstruct *)stuff;

  switch (id)
    {
    case (NEWCOLOR):
      colors = (int *)data;
      params = (struct roundrectstruct *)stuff;
    
      params->red = *(colors);
      params->green = *(colors+1);
      params->blue = *(colors+2);
      break;

    case ( CHANGE_CURVE):
      params->curvature = *((float *)data);
      break;

    case (DRAW):
      cstruct = (struct clipstruct *)data;
      cx1 = cstruct->x1;  cy1 = cstruct->y1;
      cx2 = cstruct->x2;  cy2 = cstruct->y2;
      clip_rectangle((W->display)->fildes, cx1, cx2, cy1, cy2);

      screen = (W->display)->fildes;
      shadow = params->shadow;
      if (shadow <0) shadow *= -1, way =1;
      curve = params->curvature;
    
      red = ((float)(params->shadowred)/255.0);
      green = ((float)(params->shadowgreen)/255.0);
      blue = ((float)(params->shadowblue)/255.0);
      line_color(screen, red, green, blue);
      perimeter_color(screen, red, green, blue);
      fill_color(screen, red, green, blue);
      vertex_format(screen, 0, 0, 0, 0, 0);
      interior_style(screen, INT_SOLID, TRUE);
    
      x1 = W->x1; x2 = W->x2;
      y1 = W->y1; y2 = W->y2;
    
      if (!way)
	x2 -= (float)shadow, y2 -= (float)shadow;
      else x1 += (float)shadow, y1 += (float)shadow;
      fx1=x1; fx2=x2;
      fy1=y1; fy2=y2;
      d = curve*(fx2-fx1);

      fd = d;
    
      for (t=0; t< shadow; t++)
	{
	  fx1 += 1.0;
	  fx2 += 1.0;
	  fy1 += 1.0;
	  fy2 += 1.0;
	  move2d(screen, fx1+fd, fy2); draw2d(screen, fx2-fd, fy2);
	  move2d(screen, fx2, fy1+fd); draw2d(screen, fx2, fy2-fd);
	  arc(screen, fd, fd, fx2-fd, fy2-fd, 0.0, M_PI/2.0, 0.0, 0);
	  arc(screen, fd, fd, fx2-fd, fy1+fd, 0.0, -M_PI/2.0, 0.0, 0);
	  arc(screen, fd, fd, fx1+fd, fy2-fd, M_PI, M_PI/2.0, 0.0, 0);
	  make_picture_current(screen);
	}
    
      RoundedBox(screen, x1, y1, x2, y2, MAX(d, 2));
      make_picture_current(screen);
      red = ((float)(params->red)/255.0);
      green = ((float)(params->green)/255.0);
      blue = ((float)(params->blue)/255.0);
      fill_color(screen, red, green, blue);
      RoundedBox(screen, x1+1, y1+1, x2-1, y2-1, MAX(d-3, 1));
    
      clip_rectangle((W->display)->fildes, (float)0.0, (float)((W->display)->width-1),
		     0.0, (float)((W->display)->height-1));
      make_picture_current((W->display)->fildes);
      break;
    default:
      printf("RoundRectWin: unknown message %d\n", id);
    }
}


/*===== Routine to draw a filled triangle within a rectify window.  Size of
  triangle is determined by size of rectangle.  Direction refers to the
  direction of the point -- either UP, DOWN, LEFT, or RIGHT... ===========*/

TriRectify(W, red, green, blue, border, bred, bgreen, bblue, shadow, trired, trigreen, triblue, direction)
     struct Window *W;
     int red, green, blue, border, bred, bgreen, bblue, shadow;
     int trired, trigreen, triblue, direction;
{
  int TriRectWin();
  struct trirectstruct *parms;

  parms = (struct trirectstruct *)malloc(sizeof(struct trirectstruct));
  parms->red = red;
  parms->green = green;
  parms->blue = blue;
  parms->border = border;
  parms->bred = bred;
  parms->bgreen = bgreen;
  parms->bblue = bblue;
  parms->shadow = shadow;
  parms->trired = trired;
  parms->trigreen = trigreen;
  parms->triblue = triblue;
  parms->direction = direction;

  AttachCommand(W, DRAW, TriRectWin, parms);
  AttachCommand(W, NEWCOLOR, TriRectWin, parms);
  AttachCommand(W, NEWTRICOLOR, TriRectWin, parms);
}


TriRectWin(W, id, data, stuff)
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct clipstruct *cstruct;
  struct trirectstruct *parms;
  int screen, cx1, cx2, cy1, cy2, *colors;
  float red, green, blue;
  int border, shadow;
  screen = (W->display)->fildes;
  switch (id)
    {
    case (NEWCOLOR):
      colors = (int *)data;
      parms = (struct trirectstruct *)stuff;

      parms->red = *(colors);
      parms->green = *(colors+1);
      parms->blue = *(colors+2);
      break;

    case (NEWTRICOLOR):
      colors = (int *)data;
      parms = (struct trirectstruct *)stuff;

      parms->trired = *(colors);
      parms->trigreen = *(colors+1);
      parms->triblue = *(colors+2);
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

      parms = (struct trirectstruct *)stuff;

      DrawRect(screen, (float)cx1, (float)cy1, (float)cx2, (float)cy2, parms->red, parms->green, parms->blue,
	       parms->border, parms->bred, parms->bgreen, parms->bblue, parms->shadow);

      DrawTriangle(screen,(float)cx1, (float)cy1, (float)cx2, (float)cy2, 
		   parms->trired, parms->trigreen, parms->triblue, parms->direction);

      clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		     0.0, (float)((W->display)->height-1));
      make_picture_current(screen);

      break;
    }
}


WashRectify(W, ulred, ulgreen, ulblue, urred, urgreen, urblue, 
	    lrred, lrgreen, lrblue, llred, llgreen, llblue, 
	    shadow, shadowred, shadowgreen, shadowblue, border)
     struct Window *W;
     int ulred, ulgreen, ulblue, urred, urgreen, urblue, lrred, lrgreen, lrblue, llred, llgreen, llblue;
     int shadow, shadowred, shadowgreen, shadowblue, border;
{
  struct washrectstruct *params;
  int WashRectWin();

  params = (struct washrectstruct *)malloc(sizeof(struct washrectstruct));
  params->ulred = ulred;  params->ulgreen = ulgreen;  params->ulblue = ulblue;
  params->urred = urred;  params->urgreen = urgreen;  params->urblue = urblue;
  params->lrred = lrred;  params->lrgreen = lrgreen;  params->lrblue = lrblue;
  params->llred = llred;  params->llgreen = llgreen;  params->llblue = llblue;
  params->shadow = shadow;
  params->shadowred = shadowred;
  params->shadowgreen = shadowgreen;
  params->shadowblue = shadowblue;
  params->border = border;

  AttachCommand(W, DRAW, WashRectWin, (char *)params);
  AttachCommand(W, NEWCOLOR, WashRectWin, (char *)params);
}


WashRectWin(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct washrectstruct *params;
  int x1, x2, y1, y2, screen, shadow, border, bd, s, *colors;
  float cx1, cx2, cy1, cy2;
  float red, green, blue, ulr,ulg,ulb, llr,llg,llb, urr,urg,urb, lrr,lrg,lrb;
  float ptlist[20];
  struct clipstruct *cstruct;
  unsigned char *control;

  control = W->display->control;

  cstruct = (struct clipstruct *)data;
  cx1 = cstruct->x1;  cy1 = cstruct->y1;
  cx2 = cstruct->x2;  cy2 = cstruct->y2;
  clip_rectangle((W->display)->fildes, cx1, cx2, cy1, cy2);

  x1 = (float)(W->x1);  x2 = (float)(W->x2);
  y1 = (float)(W->y1);  y2 = (float)(W->y2);

  screen = (W->display)->fildes;

  switch (id) {
  case NEWCOLOR:
    colors = (int *)data;
    params = (struct washrectstruct *)stuff;

    params->ulred = *(colors);
    params->ulgreen = *(colors+1);
    params->ulblue = *(colors+2);

    params->urred = *(colors+3);
    params->urgreen = *(colors+4);
    params->urblue = *(colors+5);

    params->lrred = *(colors+6);
    params->lrgreen = *(colors+7);
    params->lrblue = *(colors+8);

    params->llred = *(colors+9);
    params->llgreen = *(colors+10);
    params->llblue = *(colors+11);

    break;

  case DRAW:
    params = (struct washrectstruct *)stuff;
    shadow = params->shadow;
    border = params->border;
    ulr = ((float)(params->ulred)/255.0);  
    ulg = ((float)(params->ulgreen)/255.0);  
    ulb = ((float)(params->ulblue)/255.0);
    llr = ((float)(params->llred)/255.0);  
    llg = ((float)(params->llgreen)/255.0);  
    llb = ((float)(params->llblue)/255.0);
    urr = ((float)(params->urred)/255.0);  
    urg = ((float)(params->urgreen)/255.0);  
    urb = ((float)(params->urblue)/255.0);
    lrr = ((float)(params->lrred)/255.0);  
    lrg = ((float)(params->lrgreen)/255.0);  
    lrb = ((float)(params->lrblue)/255.0);

    ptlist[0] =  x1; ptlist[1] =  y1; ptlist[2] =  ulr; ptlist[3] =  ulg; ptlist[4] =  ulb;
    ptlist[5] =  x2; ptlist[6] =  y1; ptlist[7] =  urr; ptlist[8] =  urg; ptlist[9] =  urb;
    ptlist[10] = x2; ptlist[11] = y2; ptlist[12] = lrr; ptlist[13] = lrg; ptlist[14] = lrb;
    ptlist[15] = x1; ptlist[16] = y2; ptlist[17] = llr; ptlist[18] = llg; ptlist[19] = llb;
    
    x2 -= shadow; y2 -= shadow;
    
    red = ((float)(params->shadowred)/255.0);
    green = ((float)(params->shadowgreen)/255.0);
    blue = ((float)(params->shadowblue)/255.0);
    line_color(screen, red, green, blue);
    perimeter_color(screen, red, green, blue);
    vertex_format(screen, 3, 3, 1, 0, 0);
    
    if (border<=0)
      interior_style(screen, INT_SOLID, FALSE);
    else
      interior_style(screen, INT_SOLID, TRUE);
    
    if ((x1 != x2)&&(y1 != y2))
      polygon2d(screen, ptlist, 4, 0);

    else {move2d(screen, x1, y1); draw2d(screen, x2, y2);}

    interior_style(screen, INT_HOLLOW, TRUE);
    for (bd = 1; bd < abs(border); bd++)
      rectangle(screen, (float)(x1+bd), (float)(y1+bd), (float)(x2-bd), (float)(y2-bd));
    
    for (s = 0; s < shadow; s++)
      {
	move2d(screen, (float)(x2+s+1), (float)(y1+s+1));
	draw2d(screen, (float)(x2+s+1), (float)(y2+s+1));
	move2d(screen, (float)(x1+s+1), (float)(y2+s+1));
	draw2d(screen, (float)(x2+s+1), (float)(y2+s+1));
      }
    
    clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		   0.0, (float)((W->display)->height-1));
    make_picture_current(screen);

    break;
  }
}


/*------------This is used for adaptive text. It will fill the window with---------DB-12/27/90-------*/
/*------------a gamma corrected gradation from white to black, top to bottom------------------------*/


GradRectify(W, border, bred, bgreen, bblue, shadow, grad_arrays)     
     struct Window *W;
     int border, bred, bgreen, bblue, shadow;
     struct grad_data *grad_arrays;
{
  int GradRectWin();
  struct gradrectstruct *parms;


  parms = (struct gradrectstruct *)malloc(sizeof(struct gradrectstruct));
  parms->rect = (struct rectstruct *) malloc (sizeof (struct rectstruct));
  
  parms->rect->red = 0;
  parms->rect->green = 0;
  parms->rect->blue = 0;
  parms->rect->border = border;
  parms->rect->bred = bred;
  parms->rect->bgreen = bgreen;
  parms->rect->bblue = bblue;
  parms->rect->shadow = shadow;

  parms->grad = grad_arrays;

  AttachCommand(W, DRAW, GradRectWin, parms);
  AttachCommand(W, UPDATE, GradRectWin, parms);
}




GradRectWin(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct clipstruct *cstruct;
  struct gradrectstruct *parms;
  int screen, cx1, cx2, cy1, cy2, *colors;
  float red, green, blue;
  int border, shadow;

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

    parms = (struct gradrectstruct *)stuff;

    GradDrawRect(screen, (float)cx1, (float)cy1, (float)cx2, (float)cy2, 
	     parms->rect->border, parms->rect->bred, parms->rect->bgreen, parms->rect->bblue, parms->rect->shadow, parms->grad);

    clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		   0.0, (float)((W->display)->height-1));
    make_picture_current(screen);

    break;
  }
  return (0);
}



GradDrawRect(screen, x1, y1, x2, y2, border, br, bg, bb, shadow, grad_arrays)     
     int screen, border, br, bg, bb, shadow;
     float x1, y1, x2, y2;
     struct grad_data *grad_arrays;
{
  float red, green, blue, step, r, g, b;
  int bd, s, height, i, count, val_index;

  x2 -= shadow; y2 -= shadow;

  height = y2 - y1;
  count = 0;
  step = 256.0 / (float) height;

  for (i = 0 ; i < height ; i++) {
    val_index = (int) ceil ((double)(count * step));
    if (val_index < 255) {
      r = grad_arrays->redvals [val_index] / 255.0;
      g = grad_arrays->greenvals [val_index] / 255.0;
      b = grad_arrays->bluevals [val_index] / 255.0;
      if ((r>=0.0)&&(r<=1.0)&&
	  (g>=0.0)&&(g<=1.0)&&
	  (b>=0.0)&&(b<=1.0)) {
	/*printf( "\nR %f  G %f  B %f", r, g, b);*/
	line_color (screen, r, g, b);
      }
      else
	line_color (screen, 1.0, 1.0, 1.0);
    }
    move2d (screen, x1, y1+i);
    draw2d (screen, x2, y1+i);
    count++;
  }
/*
  red = ((float)br)/255.0;
  green = ((float)bg)/255.0;
  blue = ((float)bb)/255.0;
  perimeter_color(screen, red, green, blue);
  line_color(screen, red, green, blue);

  if (border<=0)
    interior_style(screen, INT_SOLID, FALSE);
  else
    interior_style(screen, INT_SOLID, TRUE);

  if ((x1 != x2)&&(y1 != y2))
    rectangle(screen, x1, y1, x2, y2);
  else {move2d(screen, x1, y1); draw2d(screen, x2, y2);}

  interior_style(screen, INT_HOLLOW, TRUE);
  for (bd = 1; bd < abs(border); bd++)
    rectangle(screen, (float)(x1+bd), (float)(y1+bd), (float)(x2-bd), (float)(y2-bd));

  for (s = 0; s < shadow; s++)
    {
      move2d(screen, (float)(x2+s+1), (float)(y1+s+1));
      draw2d(screen, (float)(x2+s+1), (float)(y2+s+1));
      move2d(screen, (float)(x1+s+1), (float)(y2+s+1));
      draw2d(screen, (float)(x2+s+1), (float)(y2+s+1));
    }
*/
}


/*-------------------------------------------------------------------------------------------------------*/


RemoteRectify(W, red, green, blue, border, bred, bgreen, bblue, shadow)     /* v 1.0b */
     struct Window *W;
     int *red, *green, *blue, border, bred, bgreen, bblue, shadow;
{
  int RemoteRectWin();
  struct remoterectstruct *parms;

  parms = (struct remoterectstruct *)malloc(sizeof(struct remoterectstruct));
  parms->red = red;
  parms->green = green;
  parms->blue = blue;
  parms->border = border;
  parms->bred = bred;
  parms->bgreen = bgreen;
  parms->bblue = bblue;
  parms->shadow = shadow;

  AttachCommand(W, DRAW, RemoteRectWin, parms);
  AttachCommand(W, UPDATE, RemoteRectWin, parms);
}



RemoteRectWin(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct clipstruct *cstruct;
  struct remoterectstruct *parms;
  int screen, cx1, cx2, cy1, cy2;
  float red, green, blue;
  int border, shadow;
  screen = (W->display)->fildes;

  if (id == UPDATE)
    {
      cx1 = W->x1; cy1 = W->y1; cx2 = W->x2; cy2 = W->y2;
      clip_rectangle(screen, (float)cx1, (float)cx2, (float)cy1, (float)cy2);
    }

  else if (id == DRAW)
    {
      cstruct = (struct clipstruct *)data;

      cx1 = cstruct->x1; cy1 = cstruct->y1; cx2 = cstruct->x2; cy2 = cstruct->y2;
      clip_rectangle(screen, (float)cx1, (float)cx2, (float)cy1, (float)cy2);
      cx1 = W->x1; cy1 = W->y1; cx2 = W->x2; cy2 = W->y2;
    }

  parms = (struct remoterectstruct *)stuff;
  cx2 -= parms->shadow; cy2 -= parms->shadow;
  red = ((float)*parms->red)/255.0;
  green = ((float)*parms->green)/255.0;
  blue = ((float)*parms->blue)/255.0;
  fill_color(screen, red, green, blue);

  red = ((float)parms->bred)/255.0;
  green = ((float)parms->bgreen)/255.0;
  blue = ((float)parms->bblue)/255.0;
  perimeter_color(screen, red, green, blue);
  line_color(screen, red, green, blue);

  if (parms->border<=0) interior_style(screen, INT_SOLID, FALSE);
  else interior_style(screen, INT_SOLID, TRUE);

  rectangle(screen, (float)cx1, (float)cy1, (float)cx2, (float)cy2);

  interior_style(screen, INT_HOLLOW, TRUE);
  for (border = 1; border < abs(parms->border); border++)
    rectangle(screen, (float)(cx1+border), (float)(cy1+border), (float)(cx2-border), (float)(cy2-border));

  for (shadow = 0; shadow < parms->shadow; shadow++)
    {
      move2d(screen, (float)(cx2+shadow+1), (float)(cy1+shadow+1));
      draw2d(screen, (float)(cx2+shadow+1), (float)(cy2+shadow+1));
      move2d(screen, (float)(cx1+shadow+1), (float)(cy2+shadow+1));
      draw2d(screen, (float)(cx2+shadow+1), (float)(cy2+shadow+1));
    }

  clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		 0.0, (float)((W->display)->height-1));
  make_picture_current(screen);
  return(0);
}


/* This is here for some reason...

RemoteHSVRect(W, h, s, v)
     struct Window *W;
     float *h, *s, *v;
{
  int RemoteHSVRectWin();
  float **triplet;

  triplet = (float **)malloc(3*sizeof(float *));
  triplet[0] =  h;
  triplet[1] =  s;
  triplet[2] =  v;

  AttachCommand(W, DRAW, RemoteHSVRectWin, triplet);
  AttachCommand(W, UPDATE, RemoteHSVRectWin, triplet);
}



RemoteHSVRectWin(W, id, data, stuff)
     struct Window *W;
     int id;
     char *data, *stuff;
{
  int x1, x2, y1, y2, screen;
  float fx1, fx2, fy1, fy2;
  float cx1, cx2, cy1, cy2;
  int red, green, blue;
  float h, s, v;
  float **triplet;
  struct clipstruct *cstruct;

  cx1 = W->x1; cy1 = W->y1; cx2 = W->x2; cy2 = W->y2;
  switch (id)
    {
    case (DRAW):
      cstruct = (struct clipstruct *)data;
      cx1 = cstruct->x1;  cy1 = cstruct->y1;
      cx2 = cstruct->x2;  cy2 = cstruct->y2;

    default:
      screen = (W->display)->fildes;
      clip_rectangle(screen, cx1, cx2, cy1, cy2);
      triplet  = (float **)stuff;

      h = *triplet[0];
      s = *triplet[1];
      v = *triplet[2];

      if (munrgb(h, s, v, &red, &green, &blue))
	fill_color(screen, red / 255.0, green / 255.0, blue / 255.0);
      else fill_color(screen, 0.0, 0.0, 0.0);
      interior_style(screen, INT_SOLID, FALSE);

      rectangle(screen, (float)W->x1, (float)W->y1, (float)W->x2, (float)W->y2);

      clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		     0.0, (float)((W->display)->height-1));
      make_picture_current(screen);
    }

}

This is here for some reason... */








/*
 * Borderify()
 *  draws a border around _W_ _indent_ pixels in from the outside, _thickness_ pixels wide.
 *
 *  *DKY*
 */
Borderify( W, indent, thickness, r, g, b)
     struct Window *W;
     int indent, thickness, r, g, b;
{
  int BorderWin();
  struct borderstruct *parms;

  parms = (struct borderstruct *)malloc( sizeof( struct borderstruct));
  parms->red = r;
  parms->green = g;
  parms->blue = b;
  parms->indent = indent;
  parms->thickness = thickness;

  AttachCommand( W, DRAW, BorderWin, parms);
}


BorderWin( W, id, data, stuff)
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct clipstruct *cstruct;
  struct borderstruct *parms;
  int screen, cx1, cx2, cy1, cy2, i;
  float red, green, blue, indent;

  screen = (W->display)->fildes;

  switch (id)
    {

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

      parms = (struct borderstruct *)stuff;
      red = (float)(parms->red) / 255.0;
      green = (float)(parms->green) / 255.0;
      blue = (float)(parms->blue) / 255.0;
      indent = parms->indent;
    
      perimeter_color(screen, red, green, blue);
      line_color(screen, red, green, blue);

      for( i = 0; i < parms->thickness; i++) {
	move2d(screen, (float)(cx1 + indent + i), (float)(cy1 + indent + i));
	draw2d(screen, (float)(cx1 + indent + i), (float)(cy2 - indent - i));

	move2d(screen, (float)(cx1 + indent + i), (float)(cy1 + indent + i));
	draw2d(screen, (float)(cx2 - indent - i), (float)(cy1 + indent + i));

	move2d(screen, (float)(cx2 - indent - i), (float)(cy2 - indent - i));
	draw2d(screen, (float)(cx1 + indent + i), (float)(cy2 - indent - i));

	move2d(screen, (float)(cx2 - indent - i), (float)(cy2 - indent - i));
	draw2d(screen, (float)(cx2 - indent - i), (float)(cy1 + indent + i));
      }

      clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		     0.0, (float)((W->display)->height-1));
      make_picture_current(screen);

      break;
    }
}

  


/*----Will draw an axis from top to bottom of the window at a distance from left edge equal to indent in pixels---*/

Axisify (W, indent, thickness, r, g, b)
     struct Window *W;
     int indent, thickness, r, g, b;
{
  int AxisWin();
  struct borderstruct *parms;

  parms = (struct borderstruct *)malloc( sizeof( struct borderstruct));
  parms->red = r;
  parms->green = g;
  parms->blue = b;
  parms->indent = indent;
  parms->thickness = thickness;

  AttachCommand( W, DRAW, AxisWin, parms);
}



AxisWin( W, id, data, stuff)
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct clipstruct *cstruct;
  struct borderstruct *parms;
  int screen, cx1, cx2, cy1, cy2, i;
  float red, green, blue, indent;

  screen = (W->display)->fildes;

  switch (id)
    {

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

      parms = (struct borderstruct *)stuff;
      red = (float)(parms->red) / 255.0;
      green = (float)(parms->green) / 255.0;
      blue = (float)(parms->blue) / 255.0;
      indent = parms->indent;
    
      line_color(screen, red, green, blue);

      for( i = 0; i < parms->thickness; i++) {
	move2d(screen, (float)(cx1 + indent + i), (float)(cy1 + indent));
	draw2d(screen, (float)(cx1 + indent + i), (float)(cy2 - indent));
      }
	
      clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		     0.0, (float)((W->display)->height-1));
      make_picture_current(screen);

      break;
    }
}



TransRectify(W,red,green,blue,border,shadowred,shadowgreen,shadowblue,shadow,trans)
     struct Window *W;
     int red,green,blue,shadow,shadowred,shadowgreen,shadowblue,border,trans;
{
  struct transrectstruct *params;
  int TranslucentRectWin();

  params = (struct transrectstruct *)malloc(sizeof(struct transrectstruct));
  params->red = red;
  params->green = green;
  params->blue = blue;
  params->shadow = shadow;
  params->border = border;
  params->bred = shadowred;
  params->bgreen = shadowgreen;
  params->bblue = shadowblue;
  params->trans = trans;
  AttachCommand( W, DRAW, TranslucentRectWin,(char *)params);
  /* AttachCommand( W, CHANGETRANS, TranslucentRectWin,(char *)params);  REPLACED WITH TR_CHANGETRANS */
  AttachCommand(W, TR_CHANGETRANS, TranslucentRectWin, (char *)params);
  AttachCommand(W, NEWCOLOR, TranslucentRectWin, (char *)params);

}

TranslucentRectWin(W, id,data,stuff)
     struct Window *W;
     int id;
     char *data,*stuff;
{
  struct transrectstruct *params;
  int x1,x2,y1,y2,screen,shadow,t,border,trans;
  float fx1,fx2,fy1,fy2;
  int cx1, cx2, cy1, cy2;
  float bred,bgreen,bblue;
  unsigned char red,green,blue;
  struct clipstruct *cstruct;
  int *colors, *tran;

  /* NOTE:
   *
   *      If there is a shadow or border on the rectangle, we do not
   *      change the translucency of it for speed considerations...
   */


  screen = (W->display)->fildes;
  params = (struct transrectstruct *)stuff;
  shadow = params->shadow;
  border = params->border;
  trans  = params->trans;

  x1=W->x1;  x2=W->x2;  y1=W->y1;  y2=W->y2;
  cx1=0;  cx2=W->width-1;  cy1=0;  cy2=W->height-1;


  /* record the clip rectangle coordinates */
  if (id == DRAW) {
    cstruct = (struct clipstruct *)data;
    
    if ((cstruct->x1 <= x2) && (cstruct->y1 <= y2) &&
	(cstruct->x2 >= x1) && (cstruct->y2 >= y1)) {

      if (cstruct->x1 >= W->x1) 
	cx1 = cstruct->x1 - W->x1;
      else cx1 = 0;
     
      if (cstruct->y1 >= W->y1)
	cy1 = cstruct->y1 - W->y1;
      else cy1 = 0;
     
      if (cstruct->x2 <= W->x2)
	cx2 = cstruct->x2 - W->x1;
      else cx2 = W->width - 1;

      if (cstruct->y2 <= W->y2)
	cy2 = cstruct->y2 - W->y1;
      else cy2 = W->height - 1;

      /* record the RGB of the rectangle as unsigned chars */
      red   = ((unsigned char)params->red);
      green = ((unsigned char)params->green);
      blue  = ((unsigned char)params->blue);

      /* record the RGB of border color as floats */
      bred = ((float)(params->bred)/255.0);
      bgreen = ((float)(params->bgreen)/255.0);
      bblue = ((float)(params->bblue)/255.0);
      line_color(screen,bred,bgreen,bblue);
      perimeter_color(screen,bred,bgreen,bblue);

      if (x1>x2) {t = x2; x2 = x1; x1 = t;}
      if (y1>y2) {t = y2; y2 = y1; y1 = t;}
 
      x2 -= shadow; y2 -= shadow;

      fx1=x1; fx2=x2;
      fy1=y1; fy2=y2;
      
      if( trans == 0)		/* call DrawRect */
	{
/*
	DrawRect(screen, (float)cx1, (float)cy1, (float)cx2, (float)cy2, red, green, blue,
		 border, bred, bgreen, bblue, shadow);
*/
	DrawRect(screen, fx1, fy1, fx2, fy2, red, green, blue,
		 border, bred, bgreen, bblue, shadow);
	make_picture_current(screen);

      }
      else {
	/* if we're just dealing with a line -- draw it... */
	if ((fx1 == fx2) || (fy1 == fy2))
	  move2d(screen,fx1,fy1),draw2d(screen,fx2,fy2);
	else {
	  /* if `not transparent, draw translucent rectangle */
	  if (params->red >= 0)
	    { 
	      DrawTranslucentRect(red, green, blue, W->width, W->height, (W->display)->control,(W->display)->bits,
				  (W->display)->buffer, (W->display)->width, (W->display)->height, x1, y1,
				  cx1, cy1, cx2, cy2, trans);
	    }
	 
	  for (t = 0; t < border; t++, x1++, y1++, x2--, y2--) {
	    interior_style (screen, INT_HOLLOW, TRUE);
	    if (x2 > 1280)
	      rectangle(screen, (float)x1, (float)y1, (float)(x2 - 1280), (float)y2);
	    else
	      rectangle(screen, (float)x1, (float)y1, (float)x2, (float)y2);
	  }

	}
       
 
	/* draw the shadow if there is one... */
	for (t=0; t<shadow; t++)
	  {
	    fx1 += 1.0;
	    fy1 += 1.0;
	    fy2 += 1.0;
	    fx2 += 1.0;
	 
	    move2d(screen,fx1,fy2);
	    draw2d(screen,fx2,fy2);
	    move2d(screen,fx2,fy1);
	    draw2d(screen,fx2,fy2);
	  }
      }
    } 
  }
    /*  else if (id == CHANGETRANS) {
    params->trans = (int) data;
  }
    */  

  else if (id == TR_CHANGETRANS) {   /* added by mf on mar/3/91 as replacement for above CHANGETRANS. Remove CHANGETRANS
                                      as soon as confirming replies to mail message received */
    tran = (int *)data;
    params->trans = *(tran);
  }

  else if (id == NEWCOLOR)
    {
      colors = (int *)data;
    
      params->red = *(colors);
      params->green = *(colors+1);
      params->blue = *(colors+2);
    
  }
  else
    printf("TransRectify: Unknown message: %d.  Perhaps you want TR_CHANGETRANS\n", id);
}



DrawTranslucentRect (sred,sgreen,sblue,sbufw,sbufh,mcontrol,bits,dbuf,dbufw,dbufh,dx,dy,x1,y1,x2,y2,T) 
     unsigned char sred,sgreen,sblue;                  /* source RGB */
     int sbufw, sbufh;                                 /* source width and height */
     unsigned char *mcontrol;                          /* ptr to controller */
     unsigned char *dbuf;                              /* ptr to upper-left of frame buffer */
     int dbufw,dbufh;                                  /* framebuffer width and height */
     int dx,dy;                                        /* upper-left position of source */
     int x1,y1,x2,y2;                                  /* clipped coordinates of source */
     int bits,T;                                       /* translucency factor (0 - 255, 0 is opaque) */
{
  register unsigned char *dpos,*done;
  register int xcount,xgap,counter,invT;
  register int *idpos, *idone,red,green,blue;

  invT = 255 - T;

  if( T == 255)			/* clear -- do nothing */
    return( 0);

  switch (bits) {
  case (24):
    dpos = dbuf+dx+(dy+y1)*dbufw+x1;
    done = dbuf+dx+(dy+y2)*dbufw+x2;
    
    xgap = dbufw-(x2-x1+1);
    counter = xcount = x2-x1+1;
    
    while (dpos<done)
      {
	mcontrol[0x40bf] = 4;
	*dpos   = (*dpos * T + sred * invT) >> 8;
	mcontrol[0x40bf] = 2;
	*dpos   = (*dpos * T + sgreen * invT) >> 8;
	mcontrol[0x40bf] = 1;
	*dpos   = (*dpos * T + sblue * invT) >> 8;
	
	dpos++;
	
	if (!(--xcount)) {
	  dpos += xgap;
	  xcount = counter;
	} 
      }
    mcontrol[0x40bf] = 7;
    break;
  case (32):
    idpos = ((int *) (dbuf)) + ((dy+y1)*dbufw+(dx+x1));
    idone = ((int *) (dbuf)) + (dx+(dy+y2)*dbufw+x2);
    
    xgap = dbufw-(x2-x1+1);
    counter = xcount = x2-x1+1;
    
    while (idpos<idone)
      {
	red = (unsigned char) ((*idpos & 0x00ff0000) >> 16);
	green = (unsigned char) ((*idpos & 0x0000ff00) >> 8);
	blue = (unsigned char) ((*idpos & 0x000000ff) >> 0);

	red = (red * T + sred * invT) >> 8;
	green = (green * T + sgreen * invT) >> 8;
	blue = (blue * T + sblue * invT) >> 8;
	
	*idpos = ((int) ((red << 16) | (green << 8) | blue));

	idpos++;
	
	if (!(--xcount)) {
	  idpos += xgap;
	  xcount = counter;
	} 
      }
    break;
  }
}



/* don't call this with offscreenx,y = 1280,0 because this function uses that address as a temp
work space */

AttachScaleIcon(W,icon,offscreenx,offscreeny)
struct Window *W;
struct curstruct *icon;
int offscreenx,offscreeny;
{
  int DrawScaleIcon(),screen;

  icon->hotdx = offscreenx;
  icon->hotdy = offscreeny;
  screen = W->display->fildes;

  (W->display)->control[0x0003] = 0x04;
  (W->display)->control[0x40bf] = 7;
  if (W->display->bits == 24)
    shape_write24(W->display->control, icon->source, icon->dx, icon->dy, W->display->buffer, W->display->width,
		  W->display->height, offscreenx, offscreeny, icon->rule, 0, 0, W->display->width, W->display->height);
  else if (W->display->bits == 32)
    shape_write32(W->display->control, icon->source, icon->dx, icon->dy, W->display->buffer, W->display->width,
		  W->display->height, offscreenx, offscreeny, icon->rule, 0, 0, W->display->width, W->display->height);

  AttachCommand(W,DRAW,DrawScaleIcon,icon);
  return(0);
}




DrawScaleIcon(W,display,data,stuff)
struct Window *W;
int display;
char *data, *stuff;
{
  struct curstruct *icon;

  icon = (struct curstruct *)stuff;
  FastScale(W->display->fildes,icon->hotdx,icon->hotdy,1280,0,W->x1,W->y1,icon->dx,icon->dy,W->width,W->height);
  return(0);
}


HLiner(W,y,thick,r,g,b)
struct Window *W;
int thick,y,r,g,b;
{
  int HLineWin(), *parms;

  parms = (int *)malloc(5*sizeof(int));
  *(parms+0) = y;
  *(parms+1) = thick;
  *(parms+2) = r;
  *(parms+3) = g;
  *(parms+4) = b;
  AttachCommand(W, DRAW, HLineWin, parms);
}

HLineWin(W,id,data,stuff)
struct Window *W;
int id;
char *data,*stuff;
{
  struct clipstruct *cstruct;
  int screen, cx1, cx2, cy1, cy2, i,*parms,y,thick,r,g,b;
  float red, green, blue, indent;

  screen = (W->display)->fildes;

  switch (id)
    {

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

      parms = (int *)stuff;
      y = *(parms+0);
      thick = *(parms+1);
      r = *(parms+2);
      g = *(parms+3);
      b = *(parms+4);
      red = (float)(r) / 255.0;
      green = (float)(g) / 255.0;
      blue = (float)(b) / 255.0;

      line_color(screen, red, green, blue);

      for( i = W->y1+y; i < W->y1+y+thick; i++) {
	move2d(screen, (float)W->x1, (float)i);
	draw2d(screen, (float)W->x2, (float)i);
      }

      clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		     0.0, (float)((W->display)->height-1));
      make_picture_current(screen);

      break;
    }
  return(0);
}
