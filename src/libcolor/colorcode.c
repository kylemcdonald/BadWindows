/*******************************************************************************************/
/*                                                                                         */
/*  This is colorcode.c. Following are the utilities to create a contrast lookup table     */
/*  for a multi_color code. This look up table is used to perform lightness contrast       */
/*  adaptation in dynamic mapping environments                                             */
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

#define NUM_COLORS 6     /* The number of colors to be in the colorcode */
#define NUM_CONTRASTS 15  /* The number of contrast value per colors to be stored in the table */

#define HUE_NUM 9
#define SAT_NUM 3
#define LITE_NUM 6

extern struct Window *currenttest;

extern int CurrentRed, CurrentGreen, CurrentBlue;
extern float CurrentHue, CurrentSat, CurrentVal;

extern struct colorcodestruct *Current_a_table;



float *make_contrast_array ()
{
  int count = 0, x, y, i, get_window_xy(), get_array_location();
  float *contrast_array, get_contrast();
  struct List *L, *L2;



  CopyList(currenttest->port, &L);

  contrast_array = (float *) malloc (sizeof(float)*15);

  
  for (L2 = L->Front ; L2 != L ; L2 = L2->Front) 
    if ( strcmp(L2->id->tag, "Z") == 0 ) 
      {
	count++;
	get_window_xy (L2->id, &x, &y);
	i = (int) get_array_location(y);
	*(contrast_array + i) = (float) get_contrast(x);
      }
  

  printf ("\n Hight Five! %d", count);
  free (L);
  
  return ((float *) contrast_array);
}



float **make_contrast_table(parent)
struct Window *parent;
{
  static struct namestruct *color_names = NULL;
  static int *colors = NULL;
  static float **table = NULL;
  static int color_names_index = 0, color_index = 0, contrast_array_index = 0, num_colors = 0;
  char *input_string, path[80];
  struct colorcodestruct *read_color_code_table(), *T;
  int t = 0;

  if (!currenttest) {
    printf ("\nAn HLS color test must be performed before using this function");
    return (0);
  }

  if (num_colors == 0) {
    input_string = (char *) malloc (sizeof (char) * 10);
    GetString (parent, 10, 885, "How many colors in the color code?", input_string, 50);
    sscanf (input_string, "%d", &num_colors);
    printf ("\nstarting a color an adaptive color code with %d colors\n", num_colors);
    free (input_string);
  }

  if (!color_names) {
    if ((color_names = (struct namestruct *) malloc (sizeof (struct namestruct) * num_colors)) == 0)
      printf("\ncolor_names malloc failed");
    printf("\nYow! color_names");
  }

  if (!colors) {
    if ((colors = (int *) malloc (sizeof (int) * num_colors * 3)) == 0)
      printf("\ncolors malloc failed");
    printf("\nYOW! colors");
  }

  if (!table) {
    if ((table = (float **) malloc (sizeof (float *) * num_colors)) == 0)
      printf("\ntable malloc failed");
    printf("\nYOW! table");
  }


  if (color_names_index < num_colors) {
    input_string = (char *) malloc (sizeof (char) * 50);
    GetString (parent, 10, 885, "Enter a name for this color", input_string, 60);
    (color_names + color_names_index)->size = strlen (input_string) + 1;
    (color_names + color_names_index)->name  = (char *) malloc (sizeof(char) * (strlen (input_string)));
    strcpy ((color_names + color_names_index)->name , input_string);
    printf ("\nYou entered the following name: %s\n", (color_names + color_names_index)->name);
    free (input_string);
    printf("\nINPUT STRING %s  SIZE %d", (color_names + color_names_index)->name, (color_names + color_names_index)->size);
    color_names_index++;
  }
  else {
    printf("\nYou have requested an adaptive color code with %d colors", num_colors);
    return (0);
  }


  if (color_index < (num_colors * 3)) {
    *(colors + color_index) = CurrentRed;
    *(colors + color_index + 1) = CurrentGreen;
    *(colors + color_index + 2) = CurrentBlue;
    
    color_index += 3;
  }
  else {
    printf("\nYou have requested an adaptive color code with %d colors", num_colors);
    return (0);
  }


  if (contrast_array_index < num_colors) {
    *(table + contrast_array_index) = (float *) make_contrast_array();
    print_float_array (*(table + contrast_array_index), 15);
    contrast_array_index++;
  }
  else {
    printf("\nYou have requested an adaptive color code with %d colors", num_colors);
    return (0);
  }


  if (contrast_array_index == num_colors) {
    if ( !(save_color_table (colors, table, num_colors, color_names, parent)))
      {
	printf("\nYes! unfortunately its true make_color_table() colorcode.c"); 
	return(0);
      }
    printf("\nTable color saved.");

    input_string = (char *) malloc (sizeof (char) * 50);
    GetString (parent, 10, 885, "Inpput full path name of the table you wish to load.", input_string, 60);
    sscanf (input_string, "%s", input_string);
    strcpy (path, input_string);
    free(input_string);
    printf ("\nAttempting to read table : %s\n", path);
    
    T = (struct colorcodestruct *) read_color_code_table (path);

/*    print_color_code_table (T); */

    free (input_string);
  }
}

    


save_color_table (colors, table, num_colors, color_names, parent)
int *colors, num_colors;
struct namestruct *color_names;
float **table;
struct Window *parent;
{
  FILE *fp;
  char path[80], *input_string;
  int i, size;

  printf("\nSIZEOF CHAR: %d", sizeof (char));
  printf("\nSIZEOF INT: %d", sizeof (int));
  printf("\nSIZEOF FLOAT: %d", sizeof (float));

  input_string = (char *) malloc (sizeof (char) * 60);

  GetString (parent, 10, 885, "Enter full file path name for the table", input_string, 80);
  sscanf (input_string, "%s", path);
  printf ("\nSaving the color table in file: %s\n", path);
  free (input_string);  

  if ((fp = fopen (path, "w")) == NULL) 
    { 
      printf("\nsave_color_table() colorcode.c ......fopen failed .....path: %s", path); 
      return(0);
    }  

  if(fwrite ((char *) &num_colors, sizeof (int), 1, fp) != 1) 
    {
      printf ("\nsave_color_table() colorcode.c .......1rst fwrite statement failed  .......path: %s", path); 
      return(0);
    }

  if (fwrite ((char *) colors, sizeof (int), num_colors * 3, fp) != (num_colors * 3)) 
    { 
      printf ("\nsave_color_table() colorcode.c .......2nd fwrite statement failed  .......path: %s", path); 
      return(0);
    }  


  for (i = 0 ; i < num_colors ; i++) {
    printf("\nWRITING SIZES %d", (color_names + i)->size);
    if (fwrite ((char *) &((color_names + i)->size), sizeof (int), 1, fp) != 1) 
      { 
	printf ("\nsave_color_table() colorcode.c .......3rd fwrite statement failed  .......path: %s", path); 
	return(0);
      }      
  }


  for (i = 0 ; i < num_colors ; i++) {
    printf("\nWRITING STRINGS string: %s  size: %d", (color_names + i)->name, (color_names + i)->size);
    size = (color_names + i)->size;
    if (fwrite ((char *) (color_names + i)->name, sizeof (char), size, fp) != (size)) 
      { 
 	printf ("\nsave_color_table() colorcode.c .......4th fwrite statement failed  .......path: %s", path); 
	return(0);
      }      
  }


  for (i = 0 ; i < num_colors ; i++) {
    printf( "\nSaving contrast value %d: %f", i, **(table + i));
    if (fwrite ((char *) *(table + i), sizeof (float), NUM_CONTRASTS, fp) != (NUM_CONTRASTS)) 
      { 
	printf ("\nsave_color_table() colorcode.c .......5th fwrite statement failed  .......path: %s", path); 
	return(0);
      }      
  }
  fclose (fp);

  return(1);
}



struct colorcodestruct *read_color_code_table(name)
char *name;
{
  FILE *fp;
  int i, t = 0, *stringsizes, print_color_code_table();
  char *path[80];
  struct colorcodestruct *color_code_table;

  
  color_code_table = (struct colorcodestruct *) malloc ( sizeof (struct colorcodestruct));

  strcpy (path, name);

  printf ("\nIN READ_COL... path: %s", path);

  if ((fp = fopen (path, "r")) == NULL) 
    { 
      printf("\nread_color_code() colorcode.c ....fopen failed ......path: %s", path); 
      return(0);
    }

  fread ((char*) &(color_code_table->num_colors), sizeof (int), 1, fp);

  color_code_table->colors = (int *) malloc (sizeof (int) * color_code_table->num_colors * 3);
  fread ((char *) color_code_table->colors, sizeof (int), color_code_table->num_colors * 3, fp);

  printf("\ncolor_code_table->num_colors: %d", color_code_table->num_colors);

  stringsizes = (int *) malloc (sizeof (int) * color_code_table->num_colors);
  color_code_table->color_names = (char **) malloc (sizeof (char *) * color_code_table->num_colors);

  for (i = 0 ; i < color_code_table->num_colors ; i++) 
    fread ((char *) (stringsizes + i), sizeof (int), 1, fp);



  for (i = 0 ; i < color_code_table->num_colors ; i++) {
    printf ("\nstringsize: %d", (*(stringsizes + i)));
    *(color_code_table->color_names + i) = (char *) malloc (sizeof (char) * (*(stringsizes + i)));
    fread ((char *) *(color_code_table->color_names + i), sizeof (char), *(stringsizes + i), fp);
  }

  free(stringsizes);

  for (i = 0 ; i < color_code_table->num_colors ; i++) 
    printf("\nSTRING: %s", *(color_code_table->color_names + i));

  color_code_table->table = (float **) malloc (sizeof (float *) * color_code_table->num_colors);

  for (i = 0 ; i < color_code_table->num_colors ; i++) {
    *(color_code_table->table + i) = (float *) malloc (sizeof (float) * NUM_CONTRASTS);
    fread ((char *) *(color_code_table->table + i), sizeof (float), NUM_CONTRASTS, fp);
  }

  fclose (fp);

  printf("\nDone Reading Table: %s\n", path);

  print_color_code_table( (struct colorcodestruct *) color_code_table);

  set_adapt_color_table ((struct colorcodestruct *) color_code_table);

  return ((struct colorcodestruct *) color_code_table);
}



print_color_code_table(table)
struct colorcodestruct *table;
{
  int i, count = 1;


  printf("\n\nAdaptive Color Table Values:\n");
  printf("\nNumber of colors: %d\n", table->num_colors);

  for(i = 0 ;i < table->num_colors ; i++) {
    printf ("\nColor %d name: %s", count, *(table->color_names + i));
    count++;
  }

  printf("\nRGB values of the %d colors:\n", table->num_colors);

  for(i = 0 ; i < (table->num_colors * 3) ; i += 3)
    printf ("\nr %d  g %d  b %d", *(table->colors + i), *(table->colors + i + 1), *(table->colors + i + 2));

  for(i = 0 ;i < table->num_colors ; i++)
    print_float_array (*(table->table + i), 15);

  return(0);
}





set_adapt_color_table (T)
struct colorcodestruct *T;
{
  Current_a_table = (struct colorcodestruct *) T;
}






