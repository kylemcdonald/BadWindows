#include <stdio.h>
#include <math.h>
#include <starbase.c.h>
#include <windows.c.h>
#include <structures.h>
#include <ctype.h>

extern struct Window *Palette;

struct rangestruct {
  float lh,ls,lv,rh,rs,rv;
  int lr,lg,lb,rr,rg,rb;
  int divisions;
  struct Window *r;
};

struct Window *MakeColorRanger(Root,echo,x,y,w,h)
struct Window *Root;
struct curstruct *echo;
int x,y,w,h;
{
  struct Window *base, *pad, *r;
  struct OutDev *display;
  struct InDev *locator;
  struct rangestruct *rstruct;
  int BufUpdateWin(),RangerDo();

  display = Root->display;
  locator = Root->input;

   base = MakeWindow(display,locator,Root,x,y,w,h,"color ranger");
   Rectify(base,30,25,25,1,0,0,0,2);
   CornerResizer(base,20,40,40,8000,8000);
   MoverII(base,base,24);
   EchoWrap(base,echo,7,0,0); 

   rstruct = (struct rangestruct *)malloc(sizeof(struct rangestruct));
   rstruct->lr = CurrentRed;   rstruct->lh = CurrentHue;
   rstruct->lg = CurrentGreen;   rstruct->ls = CurrentSat;
   rstruct->lb = CurrentBlue;   rstruct->lv = CurrentVal;
   rstruct->rr = CurrentRed;   rstruct->rh = CurrentHue;
   rstruct->rg = CurrentGreen;   rstruct->rs = CurrentSat;
   rstruct->rb = CurrentBlue;   rstruct->rv = CurrentVal;
   rstruct->divisions = 10;
  
   pad = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
   AttachCommand(pad,DO,RangerDo,rstruct);
   EchoWrap(pad,echo,7,0,1);
   DoubleMaker(pad,3,6,-40,-5,  3,6,-5,-40);
   AttachCommand(pad,DRAW,RangerDo,rstruct);
   InstallWin(pad);

   r = (struct Window *)MakeWindow(Root->display,Root->input,base, 70,12,65,18,"tnum");
   Rectify(r,170,170,170,1,0,0,0,0);
   Register(r,1,&rstruct->divisions,0,5,-2,"Swiss",16,8,0,0,0,0);
   FunctionCaller(r,DO,BufUpdateWin,pad,JUSTDOWN);
   EchoWrap(r,echo,JUSTDOWN,0,0); 
   DoubleMaker(r,-35,6,-5,-5, 3,-35,-5,-5);
   InstallWin(r);

   UpdateSizes(base);

  if (Palette) AddRangerToPaletteMenu(pad);

   return (pad);
}



RangerDo(W,id,data,stuff)
struct Window *W;
int id;
char *data, *stuff;
{
  struct pickstruct *p;
  struct clipstruct *cstruct;
  int cx1,cy1,cx2,cy2,screen,n,r,g,b,stat,done;
  float fstep,hstep,vstep,sstep,f,h,v,s;
  struct rangestruct *rs;

  rs = (struct rangestruct *)stuff;
  screen = W->display->fildes;

  switch(id) {
    case(DRAW): 
      {
	cstruct = (struct clipstruct *)data;
	cx1 = cstruct->x1;
	cy1 = cstruct->y1;
	cx2 = cstruct->x2;
	cy2 = cstruct->y2;
	
	clip_rectangle(screen, (float)cx1, (float)cx2, (float)cy1, (float)cy2);
	perimeter_color(screen,0.0,0.0,0.0);
	screen = W->display->fildes;
	interior_style(screen,INT_SOLID,TRUE);
	fill_color(screen,FCON(rs->lr),FCON(rs->lg),FCON(rs->lb));
	fstep = (float)(W->width-30)/(float)(rs->divisions);
	hstep = (rs->rh - rs->lh)/(float)(rs->divisions+2);
	sstep = (rs->rs - rs->ls)/(float)(rs->divisions+2);
	vstep = (rs->rv - rs->lv)/(float)(rs->divisions+2);
	f = 15.0;
	h = rs->lh+hstep;  s = rs->ls+sstep;  v = rs->lv+vstep;
	if (W->width > W->height) {
	  rectangle(screen,(float)(W->x1),(float)W->y1,(float)(W->x1+15),(float)W->y2);
	  for (n = 0; n < rs->divisions; n++) {
	    stat = munrgb(h,s,v,&r,&g,&b);
	    fill_color(screen,FCON(r),FCON(g),FCON(b));
	    rectangle(screen,(float)(W->x1+f),(float)W->y1,(float)(W->x1+f+fstep),(float)W->y2);
	    h += hstep;  v += vstep;  s += sstep;  f += fstep;
	  }
	  rectangle(screen,(float)(W->x2-15),(float)W->y1,(float)(W->x2),(float)W->y2);
	}
	else {
	  rectangle(screen,(float)W->x1,(float)W->y1,(float)(W->x2),(float)(W->y1+15));
	  for (n = 0; n < rs->divisions; n++) {
	    stat = munrgb(h,s,v,&r,&g,&b);
	    fill_color(screen,FCON(r),FCON(g),FCON(b));
	    rectangle(screen,(float)(W->x1),(float)(W->y1+f),(float)(W->x2),(float)(W->y1+f+fstep));
	    h += hstep;  v += vstep;  s += sstep;  f += fstep;
	  }
 	  rectangle(screen,(float)(W->x1),(float)(W->y2-15),(float)(W->x2),(float)W->y2);
	}
	clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		       0.0, (float)((W->display)->height-1));
	make_picture_current(screen);
      }
      break;
	
    case (DO):
      p = (struct pickstruct *)data;
      if (p->button == JUSTDOWN)
      {

	fstep = (float)(W->width-30)/(float)(rs->divisions);
	hstep = (rs->rh - rs->lh)/(float)(rs->divisions+2);
	sstep = (rs->rs - rs->ls)/(float)(rs->divisions+2);
	vstep = (rs->rv - rs->lv)/(float)(rs->divisions+2);
	f = 15.0;
	h = rs->lh+hstep;  s = rs->ls+sstep;  v = rs->lv+vstep;
	done = 0;
	if (W->width > W->height) {
	  if (p->x <= W->x1+15) {
	    rs->lr = CurrentRed;   rs->lh = CurrentHue;
	    rs->lg = CurrentGreen;   rs->ls = CurrentSat;
	    rs->lb = CurrentBlue;   rs->lv = CurrentVal;
	    done = 1;
	  }
	  if (!done) for (n = 0; (n < rs->divisions)&&(!done); n++) {
	    stat = munrgb(h,s,v,&r,&g,&b);
	    if (p->x <= W->x1+f+fstep) {
	      CurrentRed = r;   CurrentHue = h;
	      CurrentGreen = g;  CurrentSat = s;
	      CurrentBlue = b;   CurrentVal = v;
	      if (Palette) UpdateWin(Palette);
	      done = 1;
	    }
	    h += hstep;  v += vstep;  s += sstep;  f += fstep;
	  }
	  if (!done) {
	    rs->rr = CurrentRed;   rs->rh = CurrentHue;
	    rs->rg = CurrentGreen;   rs->rs = CurrentSat;
	    rs->rb = CurrentBlue;   rs->rv = CurrentVal;
	    done = 1;
	  }
	  if (done) BufUpdateWin(W);
	}
	else {
	  if (p->y <= W->y1+15) {
	    rs->lr = CurrentRed;   rs->lh = CurrentHue;
	    rs->lg = CurrentGreen;   rs->ls = CurrentSat;
	    rs->lb = CurrentBlue;   rs->lv = CurrentVal;
	    done = 1;
	  }
	  if (!done) for (n = 0; (n < rs->divisions)&&(!done); n++) {
	    stat = munrgb(h,s,v,&r,&g,&b);
	    if (p->y <= W->y1+f+fstep) {
	      CurrentRed = r;   CurrentHue = h;
	      CurrentGreen = g;  CurrentSat = s;
	      CurrentBlue = b;   CurrentVal = v;
	      if (Palette) UpdateWin(Palette);
	      done = 1;
	    }
	    h += hstep;  v += vstep;  s += sstep;  f += fstep;
	  }
	  if (!done) {
	    rs->rr = CurrentRed;   rs->rh = CurrentHue;
	    rs->rg = CurrentGreen;   rs->rs = CurrentSat;
	    rs->rb = CurrentBlue;   rs->rv = CurrentVal;
	    done = 1;
	  }
	  if (done) BufUpdateWin(W);
	}
      }
      break;
  }

  return(0);
}
