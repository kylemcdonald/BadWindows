
/***********************************************/
/*                                             */
/*   SOUNDIFY.C                                */
/*   (was MIDI.C)                              */
/*                                             */
/*      Helpfull utilities for using the       */
/*       Akai s900                             */
/*                                             */
/*           -Dave "IO" Small 5/18/89          */
/*                                             */
/*                                                              */
/*   struct Window *MakeMidiMaster(Parent, echo, x, y, wd, ht); */
/*   MakeKeyboard(parent,echo,chl,x,y);                         */
/*   Midify(W, channel, note, velocity);                        */
/*   PianoKeyify(W, channel, note)                              */
/*                                                              */
/*   Panify, Volumify, Akaify added by AL  1December91          */
/*   Midify revised                 by AL  1December91          */
/****************************************************************/


/*--------------------------------------------------------------------*/
/*                                                                    */
/*    Midify is the basic midi communication window.  Clicking in it  */
/*  will trigger the specified sound.  It also takes the KEYUP,       */
/*  KEYDOWN, and PITCHBEND messages.  If the data is NULL, the        */
/*  velocity is the default, otherwise it is an int pointed to by     */
/*  data (range: 0 - 127).  The note is also an int. Middle C is 60,  */
/*  and it goes in half tones from there.  The channel should match   */
/*  the channel of your sample.                                       */
/*    If you want the note to turn off on JUSTUP add the following    */
/*  function to the window:                                           */
/*                                                                    */
/*      Messenger(W, DO, W, KEYUP, NULL, JUSTUP);                     */
/*                                                                    */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <termio.h>
#include <starbase.c.h>
#include <windows.c.h>
#include <structures.h>
#include <mixer.h>
#include <midistructs.h>

#define  DRAG_BAR_COLOR 45,91,93
#define  CLOSER_COLOR   116,57,65
#define  SIZER_COLOR    0,0,0
#define  IVORY 185,175,152
#define  EBONY 15,15,12

#define TRUE  1
#define FALSE 0
#define DEBUG FALSE 

/*** A GLOBAL (Yikes!) *****/
struct Window *MidiMaster;
extern struct Window *CurrentMidiWindow;



Panify(W, track)
struct Window *W;
int track;
{
  struct MixerStruct *fd;
  int DoPanWin();
  
  if ((fd = (struct MixerStruct *) malloc (sizeof (struct MixerStruct))) == 0)
    printf ("error mallocing MixerStruct\n");

  fd->track = track;

  AttachCommand(W,DRAW,DoPanWin,(char *)fd);
  return(0);
}


DoPanWin(W, id, data, stuff)
struct Window *W;
int id;
char *data,*stuff;
{
  struct clipstruct *cstruct;
  struct pickstruct *pick;
  struct MixerStruct *params;
  int maxx, maxy;
  int winx, winy;
  int button;
  int track;
  float level;

  fprintf(stderr, "pan msg rcvd: ");
  params = (struct MixerStruct *)stuff;
  track  = params->track;
  
  switch (id) {
  case (DRAW):
    fprintf(stderr, "DRAW  ");

    maxx = Root1->width;
    winx = W->x1;
    level = (winx/maxx) * 127;

    fprintf(stderr, "level at track %d = %.2f\n", track, level);

    SetMixerPan( track, (int)level );   
    break;
  default:
    printf("Pan: Unknown message: %d\n", id);
    break;
  }
  return(0);
}


Volumify(W, track)
struct Window *W;
int track;
{
  struct MixerStruct *fd;
  int DoVolumeWin();

  if ((fd = (struct MixerStruct *) malloc (sizeof (struct MixerStruct))) == 0)
    printf ("error mallocing MixerStruct\n");

  fd->track = track;

  AttachCommand(W,DRAW,DoVolumeWin,(char *)fd);
  return(0);
}


DoVolumeWin(W, id, data, stuff)
struct Window *W;
int id;
char *data,*stuff;
{
  struct clipstruct *cstruct;
  struct pickstruct *pick;
  struct MixerStruct *params;
  struct List *Temp, *L;
  struct Window *NewTop;
  int button;
  int track;
  int below = 0, above = 0;
  float level;

  params = (struct MixerStruct *)stuff;
  track  = params->track;
  
  switch (id) {
  case (DRAW):
    pick = (struct pickstruct *)data;
    button = pick->button;

    do
      {
	if ((((W->mask)->Behind)->id) == W)
	  {
	    below++;
	    break;
	  }
	for( Temp = (W->mask)->Front;
	    ( (Temp->id != W) && (Temp != W->mask) ); Temp = Temp->Front, above++);
      }
    while (W->parent != NULL);
    
    level = (below / below+above) * 127;
    SetMixerVolume( track, (int)level); 
    break;
  default:
    printf("Volume: Unknown message: %d\n", id);
    break;
  }
  return(0);
}


Akaify(W, channel, note, velocity)
struct Window *W;
int channel, note, velocity;
{
  struct MidiStruct *fd;
  int DoAkaiWin();

  if ((fd = (struct MidiStruct *) malloc (sizeof (struct MidiStruct))) == 0)
    printf ("error mallocing MidiStruct\n");

  fd->note = note;
  fd->channel = channel - 1;
  fd->velocity = velocity;
  fd->keystat = KEYUP;


  AttachCommand(W,DO,DoAkaiWin,(char *)fd);
  AttachCommand(W,PITCHBEND,DoAkaiWin,(char *)fd);
  AttachCommand(W,KEYDOWN,DoAkaiWin,(char *)fd);
  AttachCommand(W,KEYUP,DoAkaiWin,(char *)fd);
}


DoAkaiWin(W,id,data,stuff)
struct Window *W;
int id;
char *data,*stuff;
{
  struct clipstruct *cstruct;
  struct pickstruct *pick;
  int    x, y, button, i;
  struct MidiStruct *params;
  int keystat;

  params = (struct MidiStruct *)stuff;

  switch (id) {
  case (DO):
    pick = (struct pickstruct *)data;
    button = pick->button;

    switch (button) {
    case (JUSTDOWN):  
      AkaiNoteOn(params->channel, params->note, params->velocity);
      break;
    }
    case (JUSTUP):  
      AkaiNoteOff(params->channel, params->note, params->velocity);
      break;
    break;
  case (PITCHBEND):
    AkaiPitchbend(params->channel, params->note, params->velocity);
    break;
  case (KEYDOWN):
    AkaiNoteOn(params->channel, params->note, params->velocity);
    params->keystat = KEYDOWN;
    break;
  case (KEYUP):
    AkaiNoteOff(params->channel, params->note, params->velocity);
    params->keystat = KEYUP;
    break;
  case (ECHO):
    break;
  case (NOECHO):
    break;
  default:
    printf("Akai: Unknown message: %d\n", id);
    break;
  }
  return(0);
}


/*--------------------------------------------------------------------*/
/*                                                                    */
/*    MakeMidiMaster creates the window which handles all of the      */
/* incoming and outgoing midi signals.  It looks simply like an       */
/* indicator light -> it blinks red whenever there is a midi signal.  */
/* This function should be called only once, and must be called before*/
/* using midify, the generic midi note window.                        */
/*                                                                    */
/*    This window initializes the MidiMaster global.  If you want to  */
/* communicate with it directly you must declare:                     */
/*       extern Window *MidiMaster;                                   */
/*                                                                    */
/*    It understands the following messages:                          */
/*         KEYDOWN                                                    */
/*         KEYUP                                                      */
/*         PITCHBEND                                                  */
/*         SENDBYTE                                                   */
/*                                                                    */
/*   All of them take a midistruct as data except SENDBYTE which      */
/*   takes and unsigned char *.                                       */
/*                                                                    */
/*--------------------------------------------------------------------*/

struct Window *MakeMidiMaster(Parent, echo, x, y, wd, ht)
struct Window *Parent;
struct curstruct *echo;
int x, y, wd, ht;
{
  struct Window *W;

  struct MidiMasterStruct *fd;
  int DoMidiMasterWin();
  int *fildes;

  W = (struct Window *)MakeWindow(Parent->display, Parent->input, Parent, 
				  x, y,wd,ht, "Midi Master");
  
  
  if ((fildes = (int *) malloc (sizeof (int))) == 0)
    printf ("error mallocing MidiMasterStruct\n");

/*  *fildes = open_akai("/dev/tty02"); */
/*  printf("open_akai: fildes = %d\n", *fildes); */
  *fildes = OpenAkai();



  AttachCommand(W,DO,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,DRAW,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,MIDI_ON,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,MIDI_OFF,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,KEYDOWN,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,KEYUP,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,PITCHBEND,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,SENDBYTE,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,READNOTE,DoMidiMasterWin,(char *)fildes);
  AttachCommand(W,DIE,DoMidiMasterWin,(char *)fildes);

  EchoWrap(W, echo, JUSTDOWN, 0, 0);

  InstallWin(W);
  MidiMaster = W;

  return (W);
}


Midify(W, channel, note, velocity)
struct Window *W;
int channel, note, velocity;
{
  struct MidiStruct *fd;
  int DoMidiWin();

  if ((fd = (struct MidiStruct *) malloc (sizeof (struct MidiStruct))) == 0)
    printf ("error mallocing MidiStruct\n");

  fd->note = note;
  fd->channel = channel - 1;
  fd->velocity = velocity;
  fd->keystat = KEYUP;


  AttachCommand(W,DO,DoMidiWin,(char *)fd);
  AttachCommand(W,PITCHBEND,DoMidiWin,(char *)fd);
  AttachCommand(W,KEYDOWN,DoMidiWin,(char *)fd);
  AttachCommand(W,KEYUP,DoMidiWin,(char *)fd);
}


DoMidiWin(W,id,data,stuff)
struct Window *W;
int id;
char *data,*stuff;
{
  struct clipstruct *cstruct;
  struct pickstruct *pick;
  int    x, y, button, i;
  struct MidiStruct *params;
  int keystat;

  params = (struct MidiStruct *)stuff;

  switch (id) {
  case (DO):
    pick = (struct pickstruct *)data;
    button = pick->button;

    switch (button) {
    case (JUSTDOWN):  
      SendMessage(MidiMaster, KEYDOWN, stuff);
      break;
    }
    break;
  case (PITCHBEND):
    SendMessage(MidiMaster, PITCHBEND, stuff);
    break;
  case (KEYDOWN):
    SendMessage(MidiMaster, KEYDOWN, stuff);
    params->keystat = KEYDOWN;
    break;
  case (KEYUP):
    SendMessage(MidiMaster, KEYUP, stuff);
    params->keystat = KEYUP;
    break;
  case (ECHO):
    break;
  case (NOECHO):
    break;
  default:
    printf("Midi: Unknown message: %d\n", id);
    break;
  }
  return(0);
}

/*--------------------------------------------------------------------*/
/*                                                                    */
/*    Pianokeyfy is a window which makes a generic piano key          */
/*    window.  The note goes on when you press down and goes off      */
/*    when you let up.  It takes a pointer to an int for the channel  */
/*    This is so that you can make an entire keyboard and have all of */
/*    the keys pointing to the same channel                           */
/*                                                                    */
/*--------------------------------------------------------------------*/


PianoKeyify(W, channel, note)
struct Window *W;
int *channel, note;
{
  struct PianoKeyStruct *fd;
  int DoPianoKey();

  if ((fd = (struct PianoKeyStruct *) malloc (sizeof (struct PianoKeyStruct))) == 0)
    printf ("error mallocing PianoKeyStruct\n");

  fd->note = note;
  fd->channel = channel;
  fd->keystat = KEYUP;

  AttachCommand(W,DO,DoPianoKey,(char *)fd);
  AttachCommand(W,ECHO,DoPianoKey,(char *)fd);
  AttachCommand(W,NOECHO,DoPianoKey,(char *)fd);
  AttachCommand(W,KEYDOWN,DoPianoKey,(char *)fd);
  AttachCommand(W,KEYUP,DoPianoKey,(char *)fd);
}

DoPianoKey(W,id,data,stuff)
struct Window *W;
int id;
char *data,*stuff;
{
  struct clipstruct *cstruct;
  struct pickstruct *pick;
  int    x, y, button, i;
  struct PianoKeyStruct *params;
  struct MidiStruct midi;
  int    keystat;
  
  params = (struct PianoKeyStruct *)stuff;
  keystat = params->keystat;

  switch (id) {
  case (DO):
    pick = (struct pickstruct *)data;
    button = pick->button;

    switch (button) {
    case (JUSTDOWN):  
      if (keystat == KEYUP)
	SendMessage(W, KEYDOWN, NULL);
      break;
    case (JUSTUP):  
      if (keystat == KEYDOWN)
	SendMessage(W, KEYUP, NULL);
      break;
    }
    break;
  case (ECHO):
    pick = (struct pickstruct *)data;
    button = pick->button;

    switch (button) {
    case (JUSTDOWN):  
    case (BEENDOWN):  
      if (keystat == KEYUP)
	SendMessage(W, KEYDOWN, NULL);
      break;
    }
    break;
  case (NOECHO):
    if (keystat == KEYDOWN)
      SendMessage(W, KEYUP, NULL);
    break;
  case (KEYDOWN):
    midi.note = params->note;
    midi.channel = *(params->channel) - 1;
    midi.velocity = 100;
    SendMessage(MidiMaster, KEYDOWN, &midi);
    params->keystat = KEYDOWN;
    break;
  case (KEYUP):
    midi.note = params->note;
    midi.channel = *(params->channel) - 1;
    midi.velocity = 100;
    SendMessage(MidiMaster, KEYUP, &midi);
    params->keystat = KEYUP;
    break;
  default:
    printf("Midi: Unknown message: %d\n", id);
    break;
  }
  return(0);
}


DoMidiMasterWin(W,id,data,stuff)
struct Window *W;
int id;
char *data,*stuff;
{
  struct clipstruct *cstruct;
  int    *params, fildes;
  struct pickstruct *pick;
  int    x, y, button, i;
  int    cx1, cx2, cy1, cy2;
  struct MidiStruct *Note, TestNote;
  int    screen;
  unsigned char byte;
  static char c;

  params = (int *)stuff;
  fildes = *params;
  screen = W->display->fildes;

  switch (id) {
  case (DRAW):
    cstruct = (struct clipstruct *)data;
    cx1 = cstruct->x1;
    cy1 = cstruct->y1;
    cx2 = cstruct->x2;
    cy2 = cstruct->y2;

    clip_rectangle(screen, (float)cx1, (float)cx2, (float)cy1, (float)cy2);

    perimeter_color(screen, 0.0, 0.0, 0.0);
    interior_style(screen, INT_SOLID, TRUE);
    fill_color(screen, 0.2, 0.2, 0.2);
    rectangle(screen, (float)W->x1, (float)W->y1, (float)W->x2, (float)W->y2);

    clip_rectangle(screen, 0.0, 2047.0, 0.0, 1023.0);
    make_picture_current(screen);
    break;
  case (MIDI_ON):
    perimeter_color(screen, 0.0, 0.0, 0.0);
    interior_style(screen, INT_SOLID, TRUE);
    fill_color(screen, 0.8, 0.2, 0.2);
    rectangle(screen, (float)W->x1, (float)W->y1, (float)W->x2, (float)W->y2);
    make_picture_current(screen);
    break;
  case (MIDI_OFF):
    perimeter_color(screen, 0.0, 0.0, 0.0);
    interior_style(screen, INT_SOLID, TRUE);
    fill_color(screen, 0.2, 0.2, 0.2);
    rectangle(screen, (float)W->x1, (float)W->y1, (float)W->x2, (float)W->y2);
    make_picture_current(screen);
    break;
  case (DO):
    pick = (struct pickstruct *)data;
    button = pick->button;

    TestNote.note = 60;
    TestNote.channel = 0;	
    TestNote.velocity = 100;

    switch (button) {
    case (JUSTDOWN):  
      SendMessage(W, KEYDOWN, (char *)&TestNote);
      break;
    case (JUSTUP):  
      SendMessage(W, KEYUP, (char *)&TestNote);
      break;
    }
    break;
  case (PITCHBEND):
    SendMessage(W, MIDI_ON, NULL);
    Note = (struct MidiStruct *)data;

    byte = (unsigned char) (0xe0 + Note->channel);
    write(fildes, (char *) &byte, 1);
    byte = 0x00;
    write(fildes, (char *) &byte, 1);
    byte = (unsigned char) Note->velocity;
    write(fildes, (char *) &byte, 1);
/*    fflush (fp_write);*/

    SendMessage(W, MIDI_OFF, NULL);
    break;
  case (KEYDOWN):
    SendMessage(W, MIDI_ON, NULL);
    Note = (struct MidiStruct *)data;

    byte = 0x90 + Note->channel;
    write(fildes, (char *) &byte, 1);
    byte = Note->note;
    write(fildes, (char *) &byte, 1);
    byte = Note->velocity;
    write(fildes, (char *) &byte, 1);
/*    fflush (fp_write);*/

    SendMessage(W, MIDI_OFF, NULL);
    break;
  case (KEYUP):
    SendMessage(W, MIDI_ON, NULL);
    Note = (struct MidiStruct *)data;

    byte = 0x80 + Note->channel;
    write(fildes, (char *) &byte, 1);
    byte = Note->note;
    write(fildes, (char *) &byte, 1);
    byte = Note->velocity;
    write(fildes, (char *) &byte, 1);
/*    fflush (fp_write);*/

    SendMessage(W, MIDI_OFF, NULL);
    break;
  case (SENDBYTE):
    SendMessage(W, MIDI_ON, NULL);

    i =  ((int) data);
    printf("Sending %d\n", i);
    byte = (unsigned char) i;
    write(fildes, (char *) &byte, 1);

    SendMessage(W, MIDI_OFF, NULL);
    break;
  case (READNOTE):
    printf("Reding currently unsuppported ");
    /****************
    c = getc(fp_read);
    if ((CurrentMidiWindow)&&(c != -1)) {
      SendMessage(W, MIDI_ON, NULL);
      SendMessage(CurrentMidiWindow, MIDI_RECEIVE, &c);
      SendMessage(W, MIDI_OFF, NULL);
    }
    *********************/
    break;
  case (DIE):
/*    close_akai(fildes);   */
    break;
  case (ECHO):
    break;
  case (NOECHO):
    break;
  default:
    printf("Midi: Unknown message: %d\n", id);
    break;
  }
  return(0);
}



/*--------------------------------------------------------------------*/
/*                                                                    */
/*    MakeKeyboard makes a keyboard at location x,y, with an          */
/*    initial channel                                                 */
/*                                                                    */
/*--------------------------------------------------------------------*/



MakeKeyboard(Root,echo,chl,x,y)
struct Window *Root;
struct curstruct *echo;
int chl,x,y;
{
   struct Window *Keyboard, *label, *channel;
   struct Window *DragBar, *CloserWin, *SizerWin;
   struct OutDev *display;
   struct InDev  *locator;
   int    akai_fildes;
   int    *channelptr;

   channelptr = (int *)malloc(sizeof(int));
   *channelptr = chl;

   display = Root->display;
   locator = Root->input;

   /* Create EditPane with child windows */
   Keyboard = MakeWindow(display,locator,Root,x,y,580,150,"Keyboard");
   RoundRectify(Keyboard,100,100,94,0,0,0,0,.01);
   EchoWrap(Keyboard,echo,1,0,0);
   InstallWin(Keyboard);

   label = (struct Window *)MakeWindow(Root->display,Root->input,Keyboard, 10,25,100,18,"label");
   EchoWrap(label,echo,1,0,0);
   StringWin(label,"KEYBOARD",0,0,"arch",24,8,10,10,10,0);
   InstallWin(label);

   channel = (struct Window *)MakeWindow(Root->display,Root->input,Keyboard, 345,25,185,18,"channel");
   EchoWrap(channel,echo,1,0,0);
   RoundRectify(channel,100,100,105,0,0,0,0,.01);
   StringWin(channel,"MIDI  CHANNEL:",0,0,"arch",24,8,10,10,10,0);
   Register(channel, 1, (char *)channelptr, INT, 150, 0, "arch", 24, 8, 0, 0, 0, 0);
   InstallWin(channel);

   /*** Sound Windows ****/
   MakeOctave(display, locator, Keyboard, echo, 5, 50, 48, channelptr);
   MakeOctave(display, locator, Keyboard, echo, 180, 50, 60, channelptr);
   MakeOctave(display, locator, Keyboard, echo, 355, 50, 72, channelptr);

   /* default windows */
   DragBar = MakeWindow(display,locator,Keyboard,0,0,20,20,"DragBar");
   RoundRectify(DragBar,DRAG_BAR_COLOR,0,0,0,0,.01);
   MoverII(DragBar, Keyboard, 0);
   EchoWrap(DragBar,echo,1,0,0);
   Maker(DragBar, 5, 5, -25, 20);
   InstallWin(DragBar);

   CloserWin = MakeWindow(display,locator,Keyboard,0,0,20,20,"CloserWin");
   RoundRectify(CloserWin,CLOSER_COLOR,0,0,0,0,.01);
   StringWin(CloserWin, "X", 2, -2, "arch", 24, 8, 0, 0, 0, 0);
   Closer(CloserWin, Keyboard);
   EchoWrap(CloserWin,echo,JUSTUP,0,0);
   Maker(CloserWin, -20, 5, -5, 20);
   InstallWin(CloserWin);

   SizerWin = MakeWindow(display,locator,Keyboard,0,0,15,15,"SizerWin");
   MakeIcon(SizerWin, "icons/resize_icon",8,82);
   ResizerPlus(SizerWin, Keyboard, 80,80,1300,1100);
   EchoWrap(SizerWin,echo,7,0,0);
   Maker(SizerWin, -20, -20, -5, -5);
   InstallWin(SizerWin);
   
   /*****  S U C K E R   W I N D O W S  *****/
   UpdateSizes(Keyboard);

 }

MakeOctave(display, locator, Keyboard, echo, x, y, c, channel)
   struct OutDev *display;
   struct InDev  *locator;
   struct Window *Keyboard;
   struct curstruct *echo;
   int x, y, c, *channel;
{
   struct Window *W;
   int    note, i, xoff;

   /*** White keys ***********************/
   for (i = 0; i < 7; i++) {
     switch (i) {
     case (0):
       note = c;
       break;
     case (1):
       note = c+2;
       break;
     case (2):
       note = c+4;
       break;
     case (3):
       note = c+5;
       break;
     case (4):
       note = c+7;
       break;
     case (5):
       note = c+9;
       break;
     case (6):
       note = c+11;
       break;
     }

     W = MakeWindow(display,locator,Keyboard,0,0,15,15,"W");
     Maker(W, x + i*25 + 4, y, x + i*25 + 26, y + 90);
     RoundRectify(W,IVORY,0,NULL,NULL,NULL,0.05);
     PianoKeyify(W, channel, note);
     EchoWrap(W, echo, 7, 0, 0);
     InstallWin(W);
   }

   /*** Black keys **********************/
   for (i = 0; i < 5; i++) {
     switch (i) {
     case (0):
       note = c+1, xoff = 16;
       break;
     case (1):
       note = c+3, xoff = 44;
       break;
     case (2):
       note = c+6, xoff = 90;
       break;
     case (3):
       note = c+8, xoff = 118;
       break;
     case (4):
       note = c+10, xoff = 146;
       break;
     }

     W = MakeWindow(display,locator,Keyboard,0,0,15,15,"W");
     Maker(W, x + xoff, y, x + xoff + 20, y + 60);
     Rectify(W,EBONY,0,NULL,NULL,NULL,0);
     PianoKeyify(W, channel, note);
     EchoWrap(W, echo, 7, 0, 0);
     InstallWin(W);
   }
}



