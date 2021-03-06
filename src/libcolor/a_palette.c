/*******************************************************************************************/
/*                                                                                         */
/*  Following is the code that creates an adaptive color palette                           */
/*                                                                                         */
/*  Didier Bardon, October 13, 1990                                                        */
/*                                                                                         */
/*******************************************************************************************/


#include <stdio.h>
#include <starbase.c.h>
#include <string.h>
#include <math.h>
#include "windows.c.h"
#include "structures.h"

#include "/u/didier/TYPO2/adapstructs.h"
#include "vlwfont.h"

#define MARGIN 16
#define LEADING 15

extern struct curstruct *pointer;

extern int CurrentRed, CurrentGreen, CurrentBlue;
extern float CurrentHue, CurrentSat, CurrentVal;

extern struct colorcodestruct *Current_a_table;
extern char *Current_a_color;



struct Window *make_adaptive_color_palette (x, y, name, r, g, b, parent, font, size, bits)
int x, y, size, bits, r, g, b;
char *name, *font;
struct Window *parent;
{
  struct colorcodestruct *T;
  char path[80];
  int width, hight, i, j, yy, temp, print_color_code_table(), set_current_a_color ();
  struct Window *base, *currentcolorwin, *palettewin;

  printf ("\nOOOUUUII!!");

  printf ("\n%s ", name);

  strcpy (path, name);

  printf ("\n%s ",path);

  if ( !(T = (struct colorcodestruct *) read_color_code_table (path))) {
    printf( "\nread_color_codde_table() failed,  bad file name?");
    return (0);
  }

  print_color_code_table (T);

  width = (int) find_palette_width (T, font, size, bits);

  hight = (T->num_colors * (size+LEADING)) + 60;

  set_font(font, size, bits);
  temp = string_width ("Color Code");
  if (temp > width)
    width = temp;

  /*printf("\nwidth %d  hight %d", width, hight);*/

  base = (struct Window *) MakeWindow (parent->display, parent->input, parent, x, y, width+MARGIN, hight, "base");
  RoundRectify (base, r, g, b, 0, 0, 0, 0, 0.005);
  StringWin (base, "Adaptive", (MARGIN / 2)-3, hight-35, font, size, bits, 180, 180, 180, 0);  
  StringWin (base, "Color Code", (MARGIN / 2)-3, hight-21, font, size, bits, 180, 180, 180, 0);    
  Mover (base, base, 0);
  EchoWrap (base, pointer, 7, 0, 0);
  InstallWin(base);


  for(i = 0, j = 0, yy = LEADING ; i < (T->num_colors * 3) ; i += 3, j++, yy += (size+LEADING)) {
    palettewin = (struct Window *) MakeWindow (parent->display, parent->input, base, (MARGIN/2)-3, yy, width+5, size+5, "palettewin");
    StringWin (palettewin, *(T->color_names + j), 2, 0, font, size, bits, *(T->colors+i), *(T->colors+i+1), *(T->colors+i+2), 0);  
    AttachCommand (palettewin, DO, set_current_a_color, (char *) size);
    EchoRect (palettewin, *(T->colors+i), *(T->colors+i+1), *(T->colors+i+2));
    EchoWrap (palettewin, pointer, 7, 0, 0);
    InstallWin(palettewin);
  }
  
  return ((struct Window *) base);
}



int find_palette_width (T, font, size, bits)
struct colorcodestruct *T;
char *font;
int size, bits;
{
  int i, width = 0, temp, string_width();

  /*printf("\nNUM %d", T->num_colors);*/

  set_font (font, size, bits);

  for (i = 0 ; i < T->num_colors ; i++) {
    printf("\n______%s", *(T->color_names + i));
    temp = string_width ( *(T->color_names + i) );
    if ( temp > width )
      width = temp;
  }
  return (width);
}


int (*a_palette_set_new_color)() = NULL;   /* this functon is called when a new color is set*/

set_current_a_color (W, id, data, stuff)
struct Window *W;
int id;
char *data, *stuff;
{ 
  struct pickstruct *p;
  int size, Y, index, rgbindex, done = 0;

  p = (struct pickstruct *) data;

  size = (int) stuff;

  switch (p->button)
    {
    case (JUSTDOWN):
      index = 0;
      rgbindex = 0;
      Y = LEADING;
      while ((!done) && (index < Current_a_table->num_colors)) {
	if (W->rely == Y) {	
	  Current_a_color = (char *) *(Current_a_table->color_names + index);
	  CurrentRed = (int) *(Current_a_table->colors + rgbindex);
	  CurrentGreen = (int) *(Current_a_table->colors + rgbindex + 1);
	  CurrentBlue = (int) *(Current_a_table->colors + rgbindex + 2);
	  /*printf("\n-------------- Index %d  Name %s  R %d  G %d  B %d", index, Current_a_color, CurrentRed, CurrentGreen, CurrentBlue);*/
	  done = 1;
	  if( a_palette_set_new_color) {
	    printf( "send color %s\n", Current_a_color);
	    (*a_palette_set_new_color)( Current_a_color);
	  }
	}
	else {
	  Y += size + LEADING;
	  index++;
	  rgbindex += 3;
	}
      }
      printf("\nCurrent_a_color is now : %s", Current_a_color);
      break;
    }
  return (0);
}












