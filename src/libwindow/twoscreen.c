#include <stdio.h>
#include <windows.c.h>
#include <starbase.c.h>
#include <structures.h>

static int screen_mode = 0, current_screen;



FindTwoScreens(O1, O2)     /* v 1.0b */
     /*** looks for a renaissance screen and one other if O2 ***/
     struct OutDev **O1, **O2;
{
  struct OutDev *OpenRenaissance();

  *O1 = NULL;
   if (!(*O1 = OpenCrx24("/dev/crt", "hpA1439A", CRX24_FLAGS, 1)))
     if (!(*O1 = OpenCrx24("/dev/crt", "hpA1454A", CRX24Z_FLAGS, 1)))
     if (!(*O1 = Open750("/dev/crt", "hp98766", LAME_FLAGS, 1)))
       if (!(*O1 = OpenRenaissance("/dev/graphics", "hp98721", REN24_FLAGS, 1)))
         if (!(*O1 = OpenRenaissance("/dev/crt1", "hp98731", SRX24_FLAGS, 1)))
           *O1 = OpenRenaissance("/dev/graphics", "hp98731", REN24_FLAGS, 1);

  if (!*O1)
    {
      printf("Cannot find renaissance screen.\n");
      return(0);
    }

  Output1 = *O1;

  if (O2)
    {
      if (!(*O2 = OpenRenaissance("/dev/crt0", "hp98731", SRX24_FLAGS, 1)))
        *O2 = OpenRenaissance("/dev/crtren2", "hp98721", REN24_FLAGS, 1);

      if (!*O2)
        {
          printf("Cannot find second renaissance screen. Looking for a 4-plane wimp screen.\n");
          *O2 = (struct OutDev *) ScreenSet4("/dev/crt", 1);
          if (!*O2)
            {
              printf("Cannot find second screen at all.\n");
            }
        }
      Output2 = *O2;
    }
  else Output2 = NULL;

  return(1);
}



/* move windows from one screen to another */

ScreenTransfer(R)     /* v 1.0b */
     struct Window *R;
{
  struct Window *R2, *C;
  struct List *L, *Cur, *L2;

  if (R == Root1) R2 = Root2;
  else  R2 = Root1;

  L = R->port;
  CopyList(L, &L2);
  for (Cur = L2->Front; Cur != L2; Cur = Cur->Front)
    {
      C = Cur->id;
      UnstallWin(C);
      C->display = R2->display;
      ChangeDisplay(C);
      C->parent = R2;
      OpenWindow(C);
    }

  FreeList(L2);
}


ClearScreenII()     /* v 1.0b */
{
  if (Output2)
    if (Output2->bits == 4)
      {
        gclose(Output2->fildes);
        Output2->fildes = gopen("/dev/crt", OUTDEV, NULL, INIT);
        gclose(Output2->fildes);
      }

  printf("leaving clearscreenII\n");
}



ClearScreens()     /* v 1.0b */
{
  if (Output2)
    {
      if (Output2->bits == 4)
        {
          gclose(Output2->fildes);
          Output2->fildes = gopen("/dev/crt", OUTDEV, NULL, INIT);
          gclose(Output2->fildes);
        }
      else if (Output2->bits == REN24)
        {
          gclose(Output2->fildes);
          Output2->fildes = gopen("/dev/crtren2", OUTDEV, NULL, INIT);
          gclose(Output2->fildes);
        }
    }

  if (Output1->bits == REN24)
    {
      gclose(Output1->fildes);
      Output1->fildes = gopen("/dev/crtren", OUTDEV, NULL, INIT);
      gclose(Output1->fildes);
    }

}



FixColorTable(O)     /* v 1.0b */
     struct OutDev *O;
{
  float clist[8][3];
  int color;

  if ((!O)||(O->bits != 4)) return;

  for (color = 0; color<8; color++)
      clist[color][0] = clist[color][1] = clist[color][2] = 0.0;

  clist[0][0] = clist[0][1] = .1;
  clist[0][2] = .15;
  clist[7][0] = clist[7][1] = clist[7][2] = 1.0;
  define_color_table(O->fildes, 8, 8, clist);
}



SetCursorSpace(O1, O2)     /* v 1.0b */
     struct OutDev *O1, *O2;
{
  float clist[1][3];

  if (O1->bits & OFFSCREEN)
    {
      ReserveCursorSpace(O1, 2000, 832, 2047, 1023);
      /* Reserve space moved from y = 624 to 0 and size changed from 1023 to 400 SL 12/4/91 */
      /*    ReserveBackgroundSpace(O1, 1280, 624, 1800, 1023); */
      ReserveBackgroundSpace(O1, 1280, 0, 1800, 400);
    }
  if (O2)
    {
      if (O2->bits & OFFSCREEN)
        {
          ReserveCursorSpace(O2, 2000, 832, 2047, 1023); /* for a maximum cursor size of 48x48 */
/* Reserve space moved from y = 624 to 0 and size changed from 1023 to 400 SL 12/4/91 */
/*        ReserveBackgroundSpace(O2, 1280, 624, 1800, 1023); */
          ReserveBackgroundSpace(O2, 1280, 0, 1800, 400);
        }
      else if (O2->bits == 4)
        {
          ReserveCursorSpace(O2, 0, 768, 191, 816); /* for a maximum cursor size of 48x48 */
          ReserveBackgroundSpace(O2, 192, 768, 400, 1023);
        }
    }
}



Spacebar_Switch(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct pickstruct *p;

  p = (struct pickstruct *)data;
  if (p->character == ' ')
    {
      if(!Root2) return (0);
      while (getchar() == ' ');
      if (screen_toggle == Root1) screen_toggle = Root2;
      else if (screen_toggle == Root2) screen_toggle = Root1;
      else return(0);
      if(Root1->display == Root2->display) {  /* <---------------------------this should let you do spacebar switching */
                                                                          /* between the two roots on the same display */
                                                                          /* (4/16/91) --Tim */
        DrawOnly(screen_toggle);
        EchoOff(screen_toggle->display,FALSE);
        UpdateWin(screen_toggle);
        EchoOn(123456,0, screen_toggle->display);
      }
    }
  return(0);
}



ReadLocII(locator, screen, a, b, p)     /* v 1.0b */
     int locator, *a, *b;
     float *p;
     struct OutDev *screen;
{
  int valid, side = 0, button;
  float fx, fy, fz;

  button = ReadAll(locator, a, b, p);

  if (screen&&Output2)
    {
      if (*a > Output2->viswidth)
        {
          *a -= Output2->viswidth;
          side = 0;
        }
      else side = 1;

      if ((screen == Output1) && (side)) *a = 0;
      else if ((screen == Output2) && (!side)) *a = screen->viswidth;
    }

  return (button);
}


ReadLocIII(locator, screen, a, b, p, average)     /* v 1.0b */
     int locator, *a, *b, average;
     float *p;
     struct OutDev *screen;
{
  int valid, side = 0, button,aa = 0,bb=0,i;
  float fx, fy, fz,pp = 0.0;

  if (average) {
    *a = *b = *p = 0;
    for (i = 0; i < average; i++)
      {
	button = ReadAll(locator, &aa, &bb, &pp);
	*a += aa;
	*b += bb;
	*p += pp;
      }
    *a = (int)((float)*a/ (float)average);
    *b = (int)((float)*b/ (float)average);
    *p = (float)((float)*p/ (float)average);
  }
  else button = ReadAll(locator, a, b, p);

  if (screen&&Output2)
    {
      if (*a > Output2->viswidth)
        {
          *a -= Output2->viswidth;
          side = 0;
        }
      else side = 1;

      if ((screen == Output1) && (side)) *a = 0;
      else if ((screen == Output2) && (!side)) *a = screen->viswidth;
    }

  return (button);
}



MoverII(W, M, bits)     /* v 1.0b */
     struct Window *W, *M;
     int bits;
{
   int MoveAWindowII();

   if ((Root2)&&((!bits)||(Output2->bits >= 24))&&(!screen_toggle))
     AttachCommand(W, DO, MoveAWindowII, M);
   else  Mover(W, M);
   }



MoveAWindowII(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  int x, y, dx, dy;
  struct pickstruct *pick;

  pick = (struct pickstruct *)data;
  x = pick->x;
  y = pick->y;
  dx = x-W->x1;
  dy = y-W->y1;

  if (pick->button == JUSTDOWN)
    {
      MoveII((struct Window *)stuff, &x, &y);
      pick->x = W->x1+dx;
      pick->y = W->y1+dy;
    }

  return(0);
}



MoveII(W, x, y)     /* v 1.0b */
     struct Window *W;
     int *x, *y;
{
  int dx, dy, x1, x2, y1, y2, ox, oy, width, height, xs, ys, xe, ye, ddx;
  float fx1, fx2, fy1, fy2, fx, fy, fz, p;
  int value, valid, display1, display2, locator;

  dx = *x - W->x1;
  dy = *y - W->y1;

  value = 1;
  width = W->width;
  height = W->height;
  display1 = (Root1->display)->fildes;
  display2 = (Root2->display)->fildes;
  locator = (W->input)->fildes;

  if ((Root1->display) == W->display) *x = *x +Root2->width;

  clip_rectangle(display1, 0.0, (float)Root1->width, 0.0, (float)Root1->height);
  clip_rectangle(display2, 0.0, (float)Root2->width, 0.0, (float)Root2->height);
  interior_style(display1, INT_HOLLOW, TRUE);
  perimeter_color(display1, 1.0, 1.0, 1.0);
  drawing_mode(display1, 6);
  interior_style(display2, INT_HOLLOW, TRUE);
  perimeter_color(display2, 1.0, 1.0, 1.0);
  drawing_mode(display2, 6);

  xs = *x-dx; ys = *y-dy; xe = *x-dx+width-1; ye = *y-dy+height-1;

  if (xs < Root2->width) {
    rectangle(display2, (float)xs, (float)ys, (float)xe, (float)ye);
    make_picture_current(display1); /* *DKY-23Sep90* */
    make_picture_current(display2); /* *DKY-23Sep90* */
  }
  if (xe >= Root2->width) {
    rectangle(display1, (float)(xs-Root2->width), (float)ys, (float)(xe-Root2->width), (float)ye);
    make_picture_current(display1); /* *DKY-23Sep90* */
    make_picture_current(display2); /* *DKY-23Sep90* */
  }

  while (value >0)
    {
      value = ReadLocII(locator, NULL, x, y, &p);
      *x -= dx;
      *y -= dy;

      if (*x<0) *x=0;
      if (*y<0) *y= 0;
      if (*x+width-1> Root2->width+Root1->width-1) *x = (Root2->width+Root1->width-1)-width;
      if (*y+height-1> Root1->height-1) *y = (Root1->height-1)-height;

      if ((*x != xs)||(*y != ys)||(*x+width-1 != xe)||(*y+height-1 != ye))
        {
          if (xs < Root2->width) {
            rectangle(display2, (float)xs, (float)ys, (float)xe, (float)ye);
            make_picture_current(display1); /* *DKY-23Sep90* */
            make_picture_current(display2); /* *DKY-23Sep90* */
          }
          if (xe >= Root2->width) {
            rectangle(display1, (float)(xs-Root2->width), (float)ys, (float)(xe-Root2->width), (float)ye);
            make_picture_current(display1); /* *DKY-23Sep90* */
            make_picture_current(display2); /* *DKY-23Sep90* */
          }

          xs = *x; ys = *y; xe = *x+width-1; ye = *y+height-1;

          if (xs < Root2->width) {
            rectangle(display2, (float)xs, (float)ys, (float)xe, (float)ye);
            make_picture_current(display1); /* *DKY-23Sep90* */
            make_picture_current(display2); /* *DKY-23Sep90* */
          }
          if (xe >= Root2->width) 
            rectangle(display1, (float)(xs-Root2->width), (float)ys, (float)(xe-Root2->width), (float)ye);

          make_picture_current(display1);
          make_picture_current(display2);
        }

    }

  /* SLIDE OVER */

  if ((*x <Root2->width) && (xe > Root2->width))
    {
      if (Root2->width - *x > xe-Root2->width) ddx = -10;
      else ddx = 10;

      while ((*x <Root2->width) && (xe >= Root2->width))
        {
          rectangle(display2, (float)*x, (float)*y, (float)xe, (float)ye);
          rectangle(display1, (float)(*x-Root2->width), (float)*y, (float)(xe-Root2->width), (float)ye);

          *x += ddx ;
          xe = *x+width-1; ye = *y+height-1;

          rectangle(display2, (float)*x, (float)*y, (float)xe, (float)ye);
          rectangle(display1, (float)(*x-Root2->width), (float)*y, (float)(xe-Root2->width), (float)ye);

          make_picture_current(display1);
          make_picture_current(display2);
        }
    }


  rectangle(display2, (float)*x, (float)*y, (float)xe, (float)ye);
  rectangle(display1, (float)(*x-Root2->width), (float)*y, (float)(xe-Root2->width), (float)ye);

  if ((*x < Root2->width)&&(*y+height > Root2->height)) *y = Root2->height-height;

  drawing_mode(display1, 3);
  drawing_mode(display2, 3);
  make_picture_current(display1);
  make_picture_current(display2);

  CloseWindow(W);
  if (*x < Root2->width) W->parent = Root2, W->display = Root2->display;
  else W->parent = Root1, *x -= Root2->width, W->display = Root1->display;

  ChangeDisplay(W);
  MoveCoords(W, *x, *y);
  OpenWindow(W);

  *x += dx; *y += dy;

  clip_rectangle(display1, 0.0, (float)Root1->display->width, 0.0, (float)Root1->display->height);
  clip_rectangle(display2, 0.0, (float)Root2->display->width, 0.0, (float)Root2->display->height);
  return;
}



ChangeDisplay(W)     /* v 1.0b */
     struct Window *W;
{
  struct List *L, *C;

  L = W->port;
  for (C = L->Front; C != L; C = C->Front)
    {
      C->id->display = W->display;
      ChangeDisplay(C->id);
    }
}

