#include <stdio.h>
#include <math.h>
#include <starbase.c.h>
#include <windows.c.h>
#include <structures.h>

struct textscrollstruct
{
  struct curstruct *echo;
  int *line, *area, *min, *max;
  int c1, c2, c3, s1, s2, s3, m1, m2, m3, mode;
  char **text, title[200];
  char *font;
  int *selection, size;
};

/* This procedure takes the structures for a scrolling window and its mat
   plus the necessary arguments, then opens the thing

   The address supplied is NOT really the scrolling window, but actually the
   inner window that controls the selection. The caller need not know this.
   PopMat is the window returned by make */



PopUpScroller(PopMat, s, x, y, text, commands, mode)     /* v 1.0b */
     /*** mode 0 will only allow selection of one item, mode 1 any or all ***/
     struct Window *PopMat;
     int x, y, mode, *commands;
     char **text, *s; 
{
  subPopUpScroller(PopMat, s, x, y, text, commands, mode,1); 
}


subPopUpScroller(PopMat, s, x, y, text, commands, mode,init)     /* v 1.0b */
     /*** mode 0 will only allow selection of one item, mode 1 any or all ***/
     struct Window *PopMat;
     int x, y, mode, *commands,init;
     char **text, *s;
{
  int viswidth, visheight, width, height, numlines, stringwide, t;
  struct textscrollstruct *popstruct;
  struct Window *PopScroller;
  struct resizestruct *rstruct;

  PopScroller = PopMat->parent->parent;

  if (PopScroller->stat)
    {
      printf("Sorry, this scroller is already open\n");
      return;
    }

  popstruct = (struct textscrollstruct *)GetStuff(PopMat, DO);
  popstruct->text = text;
  popstruct->selection = commands;
  popstruct->mode = mode;
  strcpy(popstruct->title, s);

  viswidth = PopScroller->display->viswidth-20;
  visheight = PopScroller->display->height-20;
  set_font(popstruct->font, popstruct->size, 8);
  stringwide = string_width(s)+35;

  PopScroller->relx = x;
  PopScroller->rely = y;

  GetArrayInfo(text, &numlines, &width, 0);
  if (init) for (t = 0; t < numlines; t++)
    commands[t] = 0;

  width += 100;
  width = MAX(width, stringwide);
  height = (numlines+1) * (20+3)+60;
  if (height > 500) height = 500;
  if (width > 1000) width = 1000;

/***
****  rstruct = (struct resizestruct *)(PopScroller->port->Front->id->dostuff);
****  rstruct->x = width;
***/

  if (PopScroller->relx+width > viswidth) PopScroller->relx = viswidth-width;
  if (PopScroller->rely+height > visheight) PopScroller->rely = visheight-height;

  /* Have the mat resize itself and adjust all the line values */
  InstallWin(PopScroller);
  TransformCoords(PopScroller, PopScroller->relx, PopScroller->rely, width, height);
  UnstallWin(PopScroller);

  Query(PopScroller); /* The ok and cancel buttons on the scroller will close it later */
}



GetArrayInfo(text, lines, width, fontsize)     /* v 1.0b */
     char **text;
     int *lines, *width, fontsize;
{
  char **temptext;
  int tempwidth;

  temptext = text;
  *lines = 0;
  *width = 0;

  while (*temptext != NULL)
    {
      (*lines)++;
      tempwidth = string_width(*temptext);
      if (tempwidth > *width) *width = tempwidth;
      temptext = temptext+1;
    }

}



/* non-global file scroller */
/* returns in *textarray the address of the string array that */
/* the scroller uses */

struct Window *MakeScroller(display, locator, Root, echo, vfont, size, x, y, b1, b2, b3, m1, m2, m3, n1, n2, n3, h1, h2, h3)     /* v 1.0b */
     struct OutDev *display;
     struct InDev *locator;
     struct Window *Root;
     struct curstruct *echo;
     int x, y;
     int b1, b2, b3, m1, m2, m3, n1, n2, n3, h1, h2, h3;
{
  struct Window *Scroller, *ScrollMat, *pad, *headback;
  struct Window *resizer, *headline, *OKwin, *Cancel, *Oback, *Cback;
  struct stepperstruct *s;
  int p1, p2, p3, sp;
  struct Window *MakeTextScroller(), *MakeStepper();
  char *OKString, *CancelString;
  struct roundrectstruct *rstruct;
  struct textscrollstruct *tstruct;
  struct curstruct *back;

  Scroller = MakeWindow(display, locator, Root, x, y, 100, 100, "object selecting window");
  RoundRectify(Scroller, b1, b2, b3, 2, NONE, NONE, NONE, .01);
  MoverII(Scroller, Scroller, 0);
  EchoWrap(Scroller, echo, 1, 0, 0);
  InstallWin(Scroller);

  resizer = MakeWindow(display, locator, Scroller, 100, 100, 100, 100, "resizer");
  ResizerPlus(resizer, Scroller, 120, 160, 800, 1000);
  EchoWrap(resizer, echo, 7, 0, 0);
  RoundRectify(resizer, n1, n2, n3, 0, NONE, NONE, NONE, .48);
  Maker(resizer, -20, -25, -5, -10);
  InstallWin(resizer);

  ScrollMat = MakeStepper(display, locator, Scroller, echo, 100, 100, 100, 100, 0,
			  m1, m2, m3, n1, n2, n3, h1, h2, h3);
  s = (struct stepperstruct *)GetStuff(ScrollMat, UPDATE);
  pad = s->pad;  /* THIS is the inner window that will be returned to the caller */

  TextSelector(pad, echo, vfont, size, &s->line, &s->area, &s->min, &s->max, n1, n2, n3, h1, h2, h2, m1, m2, m3);
  Maker(ScrollMat, 5, 5+size+4+3, -7, -7-size-4-8);
  InstallWin(ScrollMat);
  tstruct = (struct textscrollstruct *)GetStuff(pad, DO);

  /* THE BIG GUYS */
  set_font(vfont,size,8);
  p1 = string_width(" OK ");
  p2 = string_width(" CANCEL ");

  headback = MakeWindow(display, locator, Scroller, 0, 0, 100, 100, "background");
  RoundRectify(headback, m1, m2, m3, 0, 0, 0, 0, .03);
  StringWin(headback, tstruct->title, 9, 1, vfont, size, 8, n1, n2, n3, 0);
  StringWin(headback, tstruct->title, 8, 0, vfont, size, 8, h1, h2, h3, 0);
  MoverII(headback, Scroller, 0);
  EchoWrap(headback, echo, 1, 0, 0);
  Maker(headback, 5, 5, -7, 5+size+4);
  InstallWin(headback);

  Oback = MakeWindow(display, locator, Scroller, 0, 0, 100, 100, "background");
  RoundRectify(Oback, m1, m2, m3, 0, 0, 0, 0, .03);
  StringWin(Oback, " OK ", 1, 1, vfont, size, 8, n1, n2, n3, 0);
  StringWin(Oback, " OK ", 0, 0, vfont, size, 8, h1, h2, h3, 0);
  Messenger(Oback, DO, pad, OK, NULL, 1);
  EchoWrap(Oback, echo, 1, 0, 0);
  Maker(Oback, 5, -7-size-4-5, p1+5, -7);
  InstallWin(Oback);

  Cback = MakeWindow(display, locator, Scroller, 0, 0, 100, 100, "background");
  RoundRectify(Cback, m1, m2, m3, 0, 0, 0, 0, .03);
  StringWin(Cback, " CANCEL ", 1, 1, vfont, size, 8, n1, n2, n3, 0);
  StringWin(Cback, " CANCEL ", 0, 0, vfont, size, 8, h1, h2, h3, 0);
  Messenger(Cback, DO, pad, CANCEL, NULL, 1);
  EchoWrap(Cback, echo, 1, 0, 0);
  Maker(Cback, -p2-36, -7-size-4-5, -36, -7);
  InstallWin(Cback);

  /* Fix Sizes */
  TransformCoords(Scroller, Scroller->relx, Scroller->rely, 400, 300);
  UnstallWin(Scroller);

  return(pad); /* the opening routine will know to open pad's parent's parent */
}



TextSelector(W, echo, vfont, size, line, area, min, max, c1, c2, c3, s1, s2, s3, m1, m2, m3)     /* v 1.0b */
     struct Window *W;
     struct curstruct *echo;
     char *vfont;
     int *area, *min, *max, *line, c1, c2, c3, s1, s2, s3, m1, m2, m3, size;
{
  struct textscrollstruct *params;
  int TextPicker(), TextDisplayer();

  params = (struct textscrollstruct *)malloc(sizeof(struct textscrollstruct));
  params->line = line;
  params->area = area;
  params->min = min;
  params->max = max;
  params->echo = echo;
  params->c1 = c1;
  params->c2 = c2;
  params->c3 = c3;
  params->s1 = s1;
  params->s2 = s2;
  params->s3 = s3;
  params->m1 = m1;
  params->m2 = m2;
  params->m3 = m3;
  params->text = NULL;
  params->font = vfont;
  params->size = size;
  params->selection = NULL;
  *(params->title) = NULL;

  AttachCommand(W, DRAW, TextDisplayer, (char *)params);
  AttachCommand(W, DO, TextPicker, (char *)params);
  AttachCommand(W, MAKE, TextPicker, (char *)params);
  AttachCommand(W, OK, TextPicker, (char *)params);
  AttachCommand(W, CANCEL, TextPicker, (char *)params);
}



TextDisplayer(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  int cx1, cy1, cx2, cy2;
  struct clipstruct *cstruct;
  struct textscrollstruct *parms;
  int startline, length, leftedge, ygap, y, xspace, yspace, xgap, line, maxline;
  float onred, ongreen, onblue, offred, offgreen, offblue;
  char **text;
  int *selection, screen;

  screen = W->display->fildes;

  if (id == DRAW)
    {
      cstruct = (struct clipstruct *)data;
      cx1 = cstruct->x1;  cy1 = cstruct->y1;
      cx2 = cstruct->x2;  cy2 = cstruct->y2;
    }
  else
    {
      cx1 = 0; cy1 = 0;
      cx2 = W->display->viswidth;
      cy2 = W->display->visheight;
    }

  clip_rectangle(screen, (float)cx1, (float)cx2, (float)cy1, (float)cy2);
  set_text_clip_rectangle(cx1, cy1, cx2, cy2);

  parms = (struct textscrollstruct *)stuff;
  text = parms->text;
  selection = parms->selection;

  startline = *parms->line;
  maxline = MIN(startline+*parms->area, *parms->max);
  offred = (float)parms->c1/255.0;
  offgreen = (float)parms->c2/255.0;
  offblue = (float)parms->c3/255.0;
  onred = (float)parms->s1/255.0;
  ongreen = (float)parms->s2/255.0;
  onblue = (float)parms->s3/255.0;

  yspace = 3;
  xspace = 3;
  ygap = parms->size;
  leftedge = W->x1+5;
  xgap = 40;
  y = W->y1+5;

  set_font(parms->font, parms->size, 8);
  set_monitor(W->display->bits, W->display->fildes);
  perimeter_color(screen, 0.0, 0.0, 0.0);
  interior_style(screen, INT_SOLID, TRUE);
  fill_color(screen, (float)parms->m1/255.0, (float)parms->m2/255.0, (float)parms->m3/255.0);
  line_color(screen, 0.0, 0.0, 0.0);
  rectangle(screen, (float)W->x1, (float)W->y1, (float)W->x2, (float)W->y2);

  for (line = startline; line<maxline; line++)
    {
      if (selection[line]) fill_color(screen, onred, ongreen, onblue);
      else fill_color(screen, offred, offgreen, offblue);
      rectangle(screen, (float)leftedge, (float)y, (float)(leftedge+xgap-1), (float)(y+ygap-1));
      move2d(screen, (float)(leftedge+xgap), (float)(y+1));
      draw2d(screen, (float)(leftedge+xgap), (float)y+ygap);
      move2d(screen, (float)(leftedge+1), (float)y+ygap);
      draw2d(screen, (float)(leftedge+xgap), (float)y+ygap);
      y += ygap + yspace;
    }
  make_picture_current(screen);
  y = W->y1+3;
  for (line = startline; line<maxline; line++)
    {
      if (selection[line])
	{
	  set_font_color(parms->s1, parms->s2, parms->s3, 0);
	  display_string(*(text+line), leftedge+xgap+xspace, y);
        }
      else
	{
	  set_font_color(parms->c1, parms->c2, parms->c3, 0);
	  display_string(*(text+line), leftedge+xgap+xspace, y);
	}
      y += ygap + yspace;
    }

  clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		 0.0, (float)((W->display)->height-1));
  make_picture_current(screen);
}



TextPicker(W, id, data, stuff)     /* v 1.0b */
     struct Window *W;
     int id;
     char *data, *stuff;
{
  int cx1, cy1, cx2, cy2;
  struct clipstruct *cstruct;
  static new=0, mode;
  int x, y, button;
  struct curstruct *echo;
  struct pickstruct *pick;
  struct trapstruct T;
  struct textscrollstruct *parms;
  int startline, length, leftedge, ygap, ys, yspace, xgap, line, xspace, maxline, t;
  float onred, ongreen, onblue, offred, offgreen, offblue;
  char **text;
  int *selection, notdone, screen, notdone2;

  parms = (struct textscrollstruct *)stuff;
  echo = parms->echo;
  screen = W->display->fildes;

  text = parms->text;
  selection = parms->selection;

  startline = *parms->line;
  maxline = MIN(startline+*parms->area, *parms->max);
  offred = (float)parms->c1/255.0;
  offgreen = (float)parms->c2/255.0;
  offblue = (float)parms->c3/255.0;
  onred = (float)parms->s1/255.0;
  ongreen = (float)parms->s2/255.0;
  onblue = (float)parms->s3/255.0;
  set_font(parms->font, parms->size, 8);
  set_monitor(W->display->bits, W->display->fildes);
  set_text_clip_rectangle(W->x1, W->y1, W->x2, W->y2);

  yspace = 3;
  ygap = parms->size;
  leftedge = W->x1+5;
  xgap = 40;
  xspace = 3;
  perimeter_color(screen, 0.0, 0.0, 0.0);
  interior_style(screen, INT_SOLID, FALSE);
  line_color(screen, 0.0, 0.0, 0.0);

  switch (id) {

  case (DO):

    pick = (struct pickstruct *)data;
    x = pick->x;
    y = pick->y;
    button = pick->button;

    switch (button) {
    case (JUSTDOWN):
      T.window = W, T.until = BEENUP;
      Dispatch(W, TRAP, &T, NULL);
      new = 1;
    case (BEENDOWN):
      EchoOff(W->display, 1);
      if (new) {
	notdone = 1;
	ys = W->y1+5;
	for (line = startline; (line< maxline)&&(notdone); line++)
	  {
	    if ((x>W->x1)&&(x<W->x2)&&(y>ys)&&(y<ys+ygap))
	      {
		if ((button == JUSTDOWN)&&(parms->mode)) mode = !selection[line];
		else if (!parms->mode) mode = TRUE;
		if (selection[line] != mode)
		  {
		    fill_color(screen, (float)parms->m1/255.0, (float)parms->m2/255.0, (float)parms->m3/255.0);

		    if (!parms->mode) {
		      /* get rid of lighted object if there is one and if mode is 0 */
		      notdone2 = 1;
		      for (t = 0; (t < *parms->max)&&(notdone2); t++)
			if (selection[t])
			  {
			    notdone2 = 0;
			    selection[t] = FALSE;
			    if ((t>= startline)&&(t<maxline))
			      {
				int ytop, ybot;

				ytop = W->y1+5+(t-startline)*(ygap+yspace);
				ybot = ytop+ygap;
				rectangle(screen, (float)leftedge+xgap+1, (float)(ytop),
					  (float)(W->x2-1), (float)(ybot+yspace));

				interior_style(screen, INT_SOLID, TRUE);
				fill_color(screen, offred, offgreen, offblue);
				rectangle(screen, (float)leftedge, (float)ytop,
					  (float)(leftedge+xgap-1), (float)ybot-1);
				make_picture_current(screen);
				set_font_color(parms->c1, parms->c2, parms->c3, 0);
				display_string(*(text+t), leftedge+xgap+xspace, ytop-2);
				ShowTops(W, W->x1, ytop, W->x2, ybot);
				interior_style(screen, INT_SOLID, FALSE);
				fill_color(screen, (float)parms->m1/255.0, (float)parms->m2/255.0,
					   (float)parms->m3/255.0);
			      }
			  }
		    }
		    rectangle(screen, (float)leftedge+xgap+1, (float)ys, (float)(W->x2-1), (float)(ys+ygap));
		    interior_style(screen, INT_SOLID, TRUE);


		    selection[line] = mode;
		    if (selection[line]) fill_color(screen, onred, ongreen, onblue);
		    else fill_color(screen, offred, offgreen, offblue);
		    rectangle(screen, (float)leftedge, (float)ys, (float)(leftedge+xgap-1), (float)(ys+ygap-1));
		    make_picture_current(screen);
		    if (selection[line]) {
		      set_font_color(parms->s1, parms->s2, parms->s3, 0);
		      display_string(*(text+line), leftedge+xgap+xspace, ys-2);
		    }
		    else  {
		      printf("select\n");
		      set_font_color(parms->c1, parms->c2, parms->c3, 0);
		      display_string(*(text+line), leftedge+xgap+xspace, ys-2);
		    }
		    ShowTops(W, W->x1, ys, W->x2, ys+ygap);
		  }
		notdone = 0;
	      }
	    ys += ygap + yspace;
	  }


      } /* if new */
      EchoOn(x, y, W->display);
      break;

    case (JUSTUP):    new = 0;
    case (BEENUP):    UpdateEcho(x, y, W->display, echo);
      break;

    }
    break;

  case (MAKE):
    {
      int width, lines, dy;

      dy = (W->height-10)/(parms->size+3);
      GetArrayInfo(parms->text, &lines, &width, 0);

      *parms->area = MAX(dy, 1);
      *parms->max = MAX(lines, 1);
      *parms->line = 0;
    }
    break;

  case (OK):
  case (CANCEL):
    {
      selection[*parms->max] = id;
      CloseWindow(W->parent->parent);
    }
    break;
  }

  return(0);
}

