#include <stdio.h>
#include <math.h>
#include <starbase.c.h>
#include <windows.c.h>
#include <structures.h>
#include <ctype.h>

extern struct Window *Palette;
extern int NextColorIndex;

float Shmax = 108.8, Shmin = 1.0, Shgap = 9.8;   /* 108.8 = 99.0 + 9.8  */
float Ssmax = 16.5, Ssmin = 0.0, Ssgap = 1.4;    /* 16.5  = 15.0 + 1.5  */  
float Svmax = 9.8, Svmin = 1.0, Svgap = .8;      /* 9.8   = 9.0  +  .8  */
float Shue,Ssat,Sval;

int Srmax = 281, Srmin = 0, Srgap = 26; 
int Sgmax = 281, Sgmin = 0, Sggap = 26;
int Sbmax = 281, Sbmin = 0, Sbgap = 26, colormode = 0;
int Sred,Sgreen,Sblue;

struct Window *hlabel, *hslider, *vslider,*sslider;
struct Window *rlabel, *rslider, *gslider,*bslider;

struct Window *MakeColorSlider(Root,echo,x,y,w,h)
struct Window *Root;
struct curstruct *echo;
int x,y,w,h;
{
  struct Window *base, *pad, *hs, *vs, *ss,*t;
  struct OutDev *display;
  struct InDev *locator;
  struct rangestruct *rstruct;
  int BufUpdateWin(),CsliderDo(),ToggleRGBMode();
  int ConvertHSVVals(), ConvertRGBVals();

  display = Root->display;
  locator = Root->input;

   base = MakeWindow(display,locator,Root,x,y,w,h,"color slider");
   Rectify(base,34,34,34,1,0,0,0,2);
   MoverII(base,base,24);
   EchoWrap(base,echo,7,0,0); 

   pad = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
   MoverII(pad,base,24);
   EchoWrap(pad,echo,7,0,0); 
   AttachCommand(pad,DRAW,CsliderDo,NULL);
   Maker(pad,3,6,-5,60);
   InstallWin(pad);

   t = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
   StringWin(t,"H",5,0,"Swiss",16,8,170,170,170,0);
   StringWin(t,"S",32,0,"Swiss",16,8,170,170,170,0);
   StringWin(t,"V",55,0,"Swiss",16,8,170,170,170,0);
   FunctionCaller(t,DO,ToggleRGBMode,1,JUSTDOWN);
   EchoWrap(t,echo,1,0,1);
   Maker(t,3,65,-5,84);
   InstallWin(t);
   hlabel = t;

  NextColorIndex = CurrentIndex;


  hs = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
  AreaSlider(hs,1,50,50,45,1,0,0,0,100,100,110,1,&Shue,&Shmax,&Shmin,&Shgap);
  FunctionCaller(hs,DO,ConvertHSVVals,1,7);
  FunctionCaller(hs,DO,ConvertRGBVals,0,7);
  FunctionCaller(hs,DO,BufUpdateWin,pad,7);
  EchoWrap(hs,echo,7,0,1);
  Maker(hs,3, 85,23,-5);
  InstallWin(hs);
  hslider = hs;

  ss = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
  AreaSlider(ss,1,50,50,45,1,0,0,0,100,100,110,1,&Ssat,&Ssmax,&Ssmin,&Ssgap);
  FunctionCaller(ss,DO,ConvertHSVVals,1,7);
  FunctionCaller(hs,DO,ConvertRGBVals,0,7);
  FunctionCaller(ss,DO,BufUpdateWin,pad,7);
  EchoWrap(ss,echo,7,0,1);
  Maker(ss,27, 85,47,-5);
  InstallWin(ss);
  sslider = ss;

  vs = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
  AreaSlider(vs,1,50,50,45,1,0,0,0,100,100,110,1,&Sval,&Svmax,&Svmin,&Svgap);
  FunctionCaller(vs,DO,ConvertHSVVals,1,7);
  FunctionCaller(hs,DO,ConvertRGBVals,0,7);
  FunctionCaller(vs,DO,BufUpdateWin,pad,7);
  EchoWrap(vs,echo,7,0,1);
  Maker(vs,51,85,71,-5);
  InstallWin(vs);
  vslider = vs;






   t = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
   StringWin(t,"R",5,0,"Swiss",16,8,170,170,170,0);
   StringWin(t,"G",29,0,"Swiss",16,8,170,170,170,0);
   StringWin(t,"B",55,0,"Swiss",16,8,170,170,170,0);
   FunctionCaller(t,DO,ToggleRGBMode,0,JUSTDOWN);
   EchoWrap(t,echo,1,0,1);
   Maker(t,3,65,-5,84);
  InstallWin(t);
  rlabel = t;
  
  hs = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
  AreaSlider(hs,1,50,50,45,1,0,0,0,100,100,110,0,&Sred,&Srmax,&Srmin,&Srgap);
  FunctionCaller(hs,DO,ConvertRGBVals,1,7);
  FunctionCaller(vs,DO,ConvertHSVVals,0,7);
  FunctionCaller(hs,DO,BufUpdateWin,pad,7);
  EchoWrap(hs,echo,7,0,1);
  Maker(hs,3, 85,23,-5);
  InstallWin(hs);
  rslider = hs;

  ss = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
  AreaSlider(ss,1,50,50,45,1,0,0,0,100,100,110,0,&Sgreen,&Sgmax,&Sgmin,&Sggap);
  FunctionCaller(ss,DO,ConvertRGBVals,1,7);
  FunctionCaller(vs,DO,ConvertHSVVals,0,7);
  FunctionCaller(ss,DO,BufUpdateWin,pad,7);
  EchoWrap(ss,echo,7,0,1);
  Maker(ss,27, 85,47,-5);
  InstallWin(ss);
  gslider = ss;

  vs = (struct Window *)MakeWindow(Root->display,Root->input,base, 0,0,10,10,"portal");
  AreaSlider(vs,1,50,50,45,1,0,0,0,100,100,110,0,&Sblue,&Sbmax,&Sbmin,&Sbgap);
  FunctionCaller(vs,DO,ConvertRGBVals,1,7);
  FunctionCaller(vs,DO,ConvertHSVVals,0,7);
  FunctionCaller(vs,DO,BufUpdateWin,pad,7);
  EchoWrap(vs,echo,7,0,1);
  Maker(vs,51,85,71,-5);
  bslider = vs;
  InstallWin(vs);

  UpdateSizes(base);

  UnstallWin(t);
  UnstallWin(hs);
  UnstallWin(ss);
  UnstallWin(vs);
  return (base);
}



CsliderDo(W,id,data,stuff)
struct Window *W;
int id;
char *data, *stuff;
{
  struct pickstruct *p;
  struct clipstruct *cstruct;
  int cx1,cy1,cx2,cy2,screen,n,r,g,b,stat,done;
  float fstep,hstep,vstep,sstep,f,h,v,s,cl[3];
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

	cl[0] = FCON(CurrentRed);	cl[1] = FCON(CurrentGreen);	cl[2] = FCON(CurrentBlue);
	if ((W->display->bits&BUFTYPE) != CRX8)
	  fill_color(screen,cl[0],cl[1],cl[2]);
	else fill_color_index(screen,CurrentIndex);

	rectangle(screen,(float)(W->x1),(float)W->y1,(float)(W->x2),(float)W->y2);
	clip_rectangle(screen, (float)0.0, (float)((W->display)->width-1),
		       0.0, (float)((W->display)->height-1));
	make_picture_current(screen);
      }
      break;
	
  }

  return(0);
}



ToggleRGBMode(mode)
{
  if (mode) {
    CloseWindow(hlabel);
    CloseWindow(hslider);
    CloseWindow(sslider);
    CloseWindow(vslider);

    OpenWindow(rlabel);
    OpenWindow(rslider);
    OpenWindow(gslider);
    OpenWindow(bslider);
    colormode = 1;
  }
  else {
    colormode = 0;
    CloseWindow(rlabel);
    CloseWindow(rslider);
    CloseWindow(gslider);
    CloseWindow(bslider);

    OpenWindow(hlabel);
    OpenWindow(hslider);
    OpenWindow(sslider);
    OpenWindow(vslider);
  }
}



ConvertHSVVals(mode)
int mode;
{
  int stat;
  float cl[3];
  if (mode) {
    CurrentHue = 99.0 - Shue +1.0;
    CurrentSat = 15.0 - Ssat;
    CurrentVal =  9.0 - Sval + 1.0;
    stat = munrgb(CurrentHue,CurrentSat,CurrentVal,&CurrentRed,&CurrentGreen,&CurrentBlue);
    cl[0] = FCON(CurrentRed);	cl[1] = FCON(CurrentGreen);	cl[2] = FCON(CurrentBlue);
    define_color_table(hslider->display->fildes,CurrentIndex,1,cl);
    ChooserColorUpdate(CurrentIndex,CurrentRed,CurrentGreen,CurrentBlue,CurrentHue,CurrentSat,CurrentVal);
  }
  else {
    Shue = 99.0 - CurrentHue + 1.0;
    Ssat = 15.0 - CurrentSat;
    Sval =  9.0 - CurrentVal + 1.0;
  }
  return (0);
}


ConvertRGBVals(mode)
int mode;
{
  int stat;
  float cl[3];
  if (mode) {
    CurrentRed =   255 - Sred;
    CurrentGreen = 255 - Sgreen;
    CurrentBlue =  255 - Sblue;
    stat = rgbmun(CurrentRed,CurrentGreen,CurrentBlue,&CurrentHue,&CurrentSat,&CurrentVal);
    cl[0] = FCON(CurrentRed);	cl[1] = FCON(CurrentGreen);	cl[2] = FCON(CurrentBlue);
    define_color_table(hslider->display->fildes,CurrentIndex,1,cl);
    ChooserColorUpdate(CurrentIndex,CurrentRed,CurrentGreen,CurrentBlue,CurrentHue,CurrentSat,CurrentVal);
  }
  else {
    Sred =   255 - CurrentRed;
    Sgreen = 255 - CurrentGreen;
    Sblue =  255 - CurrentBlue;
  }
  return (0);
}


NewCsliderColor()
{
  ConvertRGBVals(0);
  ConvertHSVVals(0);
}

