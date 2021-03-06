#include <stdio.h>
#include <math.h>
#include <starbase.c.h>
#include <windows.c.h>
#include <structures.h>

Magnifier(W,pixel)
     struct Window *W;
     int pixel;
{
  int MagWin();
  int *factor;

  factor = (int *)malloc(3*sizeof(int));

  *factor = pixel;
  *(factor+1) = 0;
  *(factor+2) = 0;
   
  AttachCommand(W,DRAW,MagWin,(char *)factor);
}


int *
MagnifierII(W,pixel, offx,offy)
     struct Window *W;
     int pixel,offx,offy;
{
  int MagWin();
  int *factor;

  factor = (int *)malloc(3*sizeof(int));

  *factor = pixel;
  *(factor+1) = offx;
  *(factor+2) = offy;
   
  AttachCommand(W,DRAW,MagWin,(char *)factor);

  return( factor);		/* *DKY-20Feb91* */
}


MagWin(W,id,data,stuff)
     struct Window *W;
     int id;
     char *data,*stuff;
{
  struct clipstruct *cstruct;
  int screen,cx1,cx2,cy1,cy2,pixel,w,h,*factor;

  screen = W->display->fildes;
  switch (id) {
    case(DRAW):
    cstruct = (struct clipstruct *)data;
    cx1 = cstruct->x1; 
    cy1 = cstruct->y1; 
    cx2 = cstruct->x2;
    cy2 = cstruct->y2;
/*    clip_rectangle(screen,(float)cx1,(float)cx2,(float)cy1,(float)cy2); */
    /* CAN'T CLIP BECAUSE SOURCE MIGHT BE OUTSIDE WINDOW */
    clip_rectangle(screen,0.0,(float)cx2,0.0,(float)cy2);   /* KLUGE!! KLUGE!!!  I can't believe it */

  default:
    cx1 = W->x1;
    cy1 = W->y1;
    cx2 = W->x2;
    cy2 = W->y2;

    factor = (int *)stuff;
    pixel = *factor;
    w = (W->width/pixel)+1;
    h = (W->height/pixel)+1;

/*    printf("FastMag %d %d  %d %d  factor %d\n",W->x1+*(factor+1),W->y1+*(factor+2),W->x1,W->y1,*factor); */
    FastMag(screen, W->x1 + *(factor+1), W->y1 + *(factor+2), w, h, W->x1,W->y1,pixel,0,0,0,0);
    clip_rectangle(screen,(float)0.0,(float)((W->display)->width-1),
		   0.0,(float)((W->display)->height-1));
    make_picture_current(screen);
    break;
  }
  return (0);
}



/**************************************
 *  MakeMagWin()
 *   Makes a snazzy magnifier window with
 *   a slider to change the magnification
 *   factor.
 *
 *   Feel free to improve this, such as:
 *     -ability to resize window
 *     -debug when mag factor is too small
 *     -etc.
 **************************************/


struct Window *
MakeMagWin( base, pointer)
     struct Window *base;
     struct curstruct *pointer;
{
  struct Window *mag, *temp, *slider, *magedArea;
  static int maxSlider = 30, minSlider = 5;
  int *magStuff, UpdateWin();
  
  mag = (struct Window *)MakeWindow(base->display,base->input,base,100,100,210,255,"Mag base (invisible)");
  MoverII( mag, mag, 0);
  EchoWrap( mag, pointer, JUSTDOWN, 0, 0);
  
  magedArea = temp = (struct Window *)MakeWindow(base->display,base->input,mag,5,50,200,200,"magnifier");
  magStuff = (int *)MagnifierII(temp, 5, 0,-45);
  MoverII(temp, mag, 0);
  EchoWrap(temp,pointer,JUSTDOWN,0,0);
  InstallWin(temp);
  
  temp = (struct Window *)MakeWindow(base->display,base->input,mag,0,0,210,5,"top bar");
  Rectify(temp,30,30,30,1,0,0,0,2);
  MoverII(temp, mag, 0);
  EchoWrap(temp,pointer,JUSTDOWN,0,0);
  InstallWin(temp);
  
  temp = (struct Window *)MakeWindow( base->display, base->input, mag, 0,0,5,255, "left side");
  Rectify(temp,30,30,30,1,0,0,0,2);
  MoverII(temp, mag, 0);
  EchoWrap(temp,pointer,JUSTDOWN,0,0);
  InstallWin(temp);
  
  temp = (struct Window *)MakeWindow(base->display,base->input,mag,0,250,210,5,"bottom bar");
  Rectify(temp,30,30,30,1,0,0,0,2);
  MoverII(temp, mag, 0);
  EchoWrap(temp,pointer,JUSTDOWN,0,0);
  InstallWin(temp);
  
  temp = (struct Window *)MakeWindow(base->display,base->input,mag,205,0,5,255,"right side");
  Rectify(temp,30,30,30,1,0,0,0,2);
  MoverII(temp, mag, 0);
  EchoWrap(temp,pointer,JUSTDOWN,0,0);
  InstallWin(temp);
  
  temp = (struct Window *)MakeWindow(base->display,base->input,mag,0,45,210,5,"bar top of magnified area");
  Rectify(temp,30,30,30,1,0,0,0,2);
  MoverII(temp, mag, 0);
  EchoWrap(temp,pointer,JUSTDOWN,0,0);
  InstallWin(temp);
  
  temp = (struct Window *)MakeWindow(base->display,base->input,mag,45,0,185,45,"big black area (top right)");
  Rectify(temp,30,30,30,1,0,0,0,2);
  MoverII(temp, mag, 0);
  EchoWrap(temp,pointer,JUSTDOWN,0,0);
  InstallWin(temp);

  slider = (struct Window *)MakeWindow( base->display, base->input, temp, 0, 0, 10, 10,"mag factor slider");
  EzSlider( slider, HORIZONTAL, 100,100,100, 0, 0,0,0, 0,0,0, INT, magStuff, &maxSlider, &minSlider);
  FunctionCaller( slider, DO, UpdateWin, mag, JUSTDOWN|BEENDOWN|JUSTUP);
  Maker( slider, 5, 25, -25, 45);
  EchoWrap( slider, pointer, JUSTDOWN|BEENDOWN|JUSTUP, 1, 0);
  InstallWin( slider);

  UpdateSizes( temp);
  
  return( mag);
}
