#include <starbase.c.h>
#include <stdio.h>
#include <structures.h>
#include <windows.c.h>

#define LEFTEND   33
#define BACKCHAR  34
#define RIGHTEND  35

#define OFFSET 4

#define DEBUG 0

float fgrd_hue, fgrd_light, fgrd_sat;
float bgrd_hue, bgrd_light, bgrd_sat;

float fgrd_H, fgrd_V, fgrd_C;
float bgrd_H, bgrd_V, bgrd_C;







AdaptiveStringWin(W, contrast, string, x, y, font, size, bits, red, green, blue, t, munsellflag)     /*DB 26june90 */
     struct Window *W;
     float contrast;
     char *string, *font;
     int size, bits, red, green, blue, t, x, y, munsellflag;
{
  struct adaptive_stringstruct *params;
  int Adaptive_DrawAString();

  /*printf("\nW %s   backgrdwin %s  string %s  x %d  y %d   font %s   size %d   bits %d   red %d   green %d   blue %d   t %d",
            W->tag, W->parent->tag, string, x, y, font, size, bits, red, green, blue, t); */

  params = (struct adaptive_stringstruct *) malloc(sizeof(struct adaptive_stringstruct));
  params->parms = (struct stringstruct *) malloc (sizeof (struct stringstruct));

  params->parms->font = font;

  params->parms->string = string;

  params->parms->size = size;
  params->parms->bits = bits;
  params->parms->x = x;
  params->parms->y = y;
  params->parms->red = red;
  params->parms->green = green;
  params->parms->blue = blue;
  params->parms->t = t;
  params->parms->char_space = 0;
  params->parms->word_space = 0;

  params->contrast = contrast;
  params->munsellflag = munsellflag;

  AttachCommand(W, DRAW, Adaptive_DrawAString, (char *) params);

}




Adaptive_DrawAString(W, id, data, stuff)  
     struct Window *W;
     int id;
     char *data, *stuff;
{
  struct adaptive_stringstruct *mstruct, *newstruct;
  struct clipstruct *cstruct;
  int screen, cx1, cx2, cy1, cy2;
  int *colors, *trans;
  int *temp;

  if (id==DRAW)
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      cstruct = (struct clipstruct *)data;

      /*printf ("\nIn Adaptive_DrawAString, clip coords: x1 %d, y1 %d, x2 %d, y2 %d", cstruct->x1, cstruct->y1, cstruct->x2, cstruct->y2);*/

      set_font(mstruct->parms->font, mstruct->parms->size, mstruct->parms->bits);
      set_monitor(W->display->bits, W->display->fildes);
      set_text_clip_rectangle(cstruct->x1, cstruct->y1, cstruct->x2, cstruct->y2);
      set_char_space(mstruct->parms->char_space);
      set_word_space(mstruct->parms->word_space);
      if (mstruct->parms->string)
	{
	  if (mstruct->parms->x == CENTERED)
	    {
	      int xx;
	      xx = W->x1+W->width/2 - string_width(mstruct->parms->string)/2;
	      display_adaptive_string(W,mstruct, xx, W->y1+mstruct->parms->y);
	    }
	  else if (mstruct->parms->x < 0)
	    {
	      int xx;
	      xx = string_width(mstruct->parms->string);
	      display_adaptive_string(W, mstruct, W->x2 - xx + mstruct->parms->x, W->y1+mstruct->parms->y);
	    }
	  else display_adaptive_string(W, mstruct, W->x1+mstruct->parms->x, W->y1+mstruct->parms->y);
	}
      set_char_space( 0);	/* *DKY-18Mar90-17:29* */
      set_word_space( 0);	/* *DKY-18Mar90-17:29* */
    }

  else if (id == ATTRIBUTES)
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      mstruct->parms->string = data;
    }

  else if (id == CHANGESTRING)
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      mstruct->parms->string = data;
    }

  else if (id == CHANGECOLOR)
    {
      CloseWindow(W);
      mstruct = (struct adaptive_stringstruct *)stuff;
      colors = (int *)data;	/* I changed this to take an *int, 12/5/89, dsmall */
      mstruct->parms->red = (colors[0]);
      mstruct->parms->green = (colors[1]);
      mstruct->parms->blue = (colors[2]);
      OpenWindow(W);
    }

  else if (id == CHANGETRANS)
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      trans = (int *)data;
      mstruct->parms->t = *trans;
    }

  else if (id == SETCHARSPACE)
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      temp = (int *)data;
      mstruct->parms->char_space = *temp;
    }

  else if (id == SETWORDSPACE)
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      temp = (int *)data;
      mstruct->parms->word_space = *temp;
    }

  else if (id == CHANGEFONT)
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      newstruct = (struct adaptive_stringstruct *)data;
      mstruct->parms->font = newstruct->parms->font;
      mstruct->parms->size = newstruct->parms->size;
      mstruct->parms->bits = newstruct->parms->bits;
    }

  else if (id == CHANGE_X_POS)	/* *DKY-13May90* */
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      temp = (int *)data;
      mstruct->parms->x = *temp;
    }

  else if (id == CHANGE_Y_POS)	/* *DKY-13May90* */
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      temp = (int *)data;
      mstruct->parms->y = *temp;
    }

  else if (id == SET_SIZE)	/* *DKY-13May90* */
    {
      mstruct = (struct adaptive_stringstruct *)stuff;
      temp = (int *)data;
      mstruct->parms->size = *temp;
    }

  else if (id == GET_STRINGWIN_COLOR) {  	/* *DKY-15May90* */
    int **colors;
    /* data is a pointer to an array of 3 integers */
    colors = (int **)data;
    mstruct = (struct adaptive_stringstruct *)stuff;

    *colors = (int *)&(mstruct->parms->red);
    *(colors+1) = (int *)&(mstruct->parms->green);
    *(colors+2) = (int *)&(mstruct->parms->blue);
  }

}



display_adaptive_string (W, stringstuff, x, y)
struct Window *W;
struct adaptive_stringstruct *stringstuff;
int x, y;
{
  int stwidth, stheight, string_width(), *draw_adapt_box(), *control_foreground_contrast(), 
  *b_rgb, *new_f_rgb, *old_f_rgb;
  int *munsell_control_foreground_contrast();

  old_f_rgb = (int *) malloc (sizeof (int) * 3);
  *old_f_rgb = stringstuff->parms->red;  
  *(old_f_rgb+1) = stringstuff->parms->green;  
  *(old_f_rgb+2) = stringstuff->parms->blue;

  if ((set_font (stringstuff->parms->font, stringstuff->parms->size, stringstuff->parms->bits)) == 0) 
    printf("\n ERROR set_font");
  stwidth = (int) string_width (stringstuff->parms->string);
  stheight = stringstuff->parms->size;

  
  b_rgb = (int *) draw_adapt_box (W, stringstuff, stwidth, stheight, x, y + OFFSET);


  if (stringstuff->munsellflag) 
    new_f_rgb = (int *) munsell_control_foreground_contrast (b_rgb, old_f_rgb, stringstuff->contrast);
  else 
    new_f_rgb = (int *) control_foreground_contrast (b_rgb, old_f_rgb, stringstuff->contrast);
  
  free (b_rgb);

  set_font_color (*new_f_rgb, *(new_f_rgb+1), *(new_f_rgb+2), stringstuff->parms->t);
  /*printf("\nForeground R %d  G %d  B %d", *new_f_rgb, *(new_f_rgb+1), *(new_f_rgb+2));*/

  free (new_f_rgb);

  if ((set_font (stringstuff->parms->font, stringstuff->parms->size, stringstuff->parms->bits)) == 0) 
    printf("\n ERROR set_font");

  clip_rectangle (W->display->fildes, (float) x, (float) (x + stwidth + OFFSET * 2), (float) y, 
		  (float) (y + stheight));

  display_string (stringstuff->parms->string, x + OFFSET/2, y + OFFSET);

/*  rectangle (W->display->fildes, (float) x, (float) y, (float) (x + stwidth + OFFSET * 2), 
			   (float) (y + stheight));*/

  clip_rectangle (W->display->fildes, (float) 0.0, (float)((W->display)->width-1), (float) 0.0, 
		  (float)((W->display)->height-1));


}


int *draw_adapt_box(W, stringstuff, width, height, x, y)
struct Window *W;
struct adaptive_stringstruct *stringstuff;
int width, height, x, y;
{
  int i, count = 0, *rgb, *find_average_bkgrd_color();
  char *adaptst, tempstring[100];

  /*printf ("\ndraw_adapt_box  W %s  W->width %d  W->height %d  W->relx %d  W->rely %d  width %d  height %d  x %d  y %d", W->tag, 
	  W->width, W->height, W->relx, W->rely, width, height, x, y);*/
  adaptst = (char *) malloc (sizeof(char)*width+3);

  adaptst[0] = (char) LEFTEND;

  for (i = 1 ; i <= width ; i++)  {
    adaptst[i] = (char) BACKCHAR;
    count++;
  }

  adaptst[count+1] = (char) RIGHTEND;
  adaptst[count+2] = '\0';

  rgb = (int *) find_average_bkgrd_color (W, x, y, height, width);

  strcpy(tempstring, stringstuff->parms->font);
  strcat(tempstring, "_background"); 


  /*printf("\n draw_adapt_box font %s  size %d  bits %d", tempstring, stringstuff->parms->size, stringstuff->parms->bits);*/


  if (set_font (tempstring, stringstuff->parms->size, stringstuff->parms->bits) == 0) 
      printf("\n ERROR set_font, draw_adapt_box(), adaptive.c in windowsource\n"); 

  /*rectangle (W->display->fildes, (float) x, (float) y-OFFSET, (float) (x+width+OFFSET*2), (float) (y+height+OFFSET));*/

  clip_rectangle (W->display->fildes, (float) x, (float) y-OFFSET, (float) (x+width+OFFSET*2), (float) (y+height+OFFSET));
  set_font_color ((int) *rgb, (int) *(rgb+1), (int) *(rgb+2), 0);
  display_string ((char *)adaptst, x, y);


  free (adaptst);

  return(rgb);
}






int *find_average_bkgrd_color (W, x, y, ht, wd)
struct Window *W;
int x, y, ht, wd;
{
  int i, j, count = 0, *RGB;
  unsigned char *R, *G, *B;

  if (ht == 0) { printf ("\nfind_avarage_bkgrd_color() in adaptive.c  OOooops! height = 0 can't do it"); exit(1); }
  else if (wd == 0) { printf ("\nfind_avarage_bkgrd_color() in adaptive.c  OOooops! width = 0 can't do it"); exit(1); }
  else {
    /*printf ("\nfind_avarage_color W %s  x %d  y %d  ht %d  wd %d", W->tag, x, y, ht, wd);*/
  
    if ((R = (unsigned char *) malloc (sizeof (unsigned char)*ht*wd)) == NULL) 
      printf("\nERROR find_average_bkgrd_color malloc failed R, adaptive.c");
    if ((G = (unsigned char *) malloc (sizeof (unsigned char)*ht*wd)) == NULL) 
      printf("\nERROR find_average_bkgrd_color malloc failed G, adaptive.c");
    if ((B = (unsigned char *) malloc (sizeof (unsigned char)*ht*wd)) == NULL) 
      printf("\nERROR find_average_bkgrd_color malloc failed B, adaptive.c");

    if ((RGB = (int *) malloc (sizeof (int)*3)) == NULL) 
      printf("\nERROR find_average_bkgrd_color malloc failed RGB, adaptive.c");

    *RGB = 0;  *(RGB+1) = 0;  *(RGB+2) = 0;

    bank_switch (W->display->fildes, 2, 0);
    dcblock_read (W->display->fildes, x, y, wd, ht, R, FALSE);
    bank_switch (W->display->fildes, 1, 0);
    dcblock_read (W->display->fildes, x, y, wd, ht, G, FALSE);
    bank_switch (W->display->fildes, 0, 0);
    dcblock_read (W->display->fildes, x, y, wd, ht, B, FALSE);
    
    W->display->control[0x40bf] = 7;
    
    for (i = 0, j = 0 ; j < wd ; j += 4) {
      /*printf("\nARRAY value J %d  %d  %d", j, (int) *(R+i+j), (int) *(R+(ht-1)*wd+j));*/
      *RGB +=(int) *(R+i+j);
      *RGB +=(int) *(R+(ht-1)*wd+j);
      count += 2;
    }
    /*printf("\nG %d  Count %d", *RGB, count);*/
    *RGB = *RGB / count;
    /*printf("\nRvalue %d", *RGB);*/
    
    count = 0;
    for (i = 0, j = 0 ; j < wd ; j += 4) {
      /*printf("\nARRAY value J %d  %d  %d", j, (int) *(G+i+j), (int) *(G+(ht-1)*wd+j));*/
      *(RGB+1) +=(int) *(G+i+j);
      *(RGB+1) +=(int) *(G+(ht-1)*wd+j);
      count += 2;
    }
    /*printf("\nG %d  Count %d", *(RGB+1), count);*/
    *(RGB+1) = *(RGB+1) / count;
    /*printf("\nGvalue %d", *(RGB+1));*/
    
    count = 0;
    for (i = 0, j = 0 ; j < wd ; j += 4) {
      /*printf("\nARRAY value J %d  %d  %d", j, (int) *(B+i+j), (int) *(B+(ht-1)*wd+j));*/
      *(RGB+2) +=(int) *(B+i+j);
      *(RGB+2) +=(int) *(B+(ht-1)*wd+j);
      count += 2;
    }
    /*printf("\nB %d Count %d", *(RGB+2), count);*/
    *(RGB+2) = *(RGB+2) / count;
    /*printf("\nBvalue %d", *(RGB+2));*/
    
    /*printf("\nIn find_average_background_color R %d   G %d   B %d", *RGB, *(RGB+1), *(RGB+2));*/
    /*printf ("\nExiting find_average_bckgrd_color");*/
    
    free (R);  free (G);  free (B);
    
    return (RGB);
  }
}



  
int *control_foreground_contrast (rgb_back, rgb_front, contrast)
int *rgb_back, *rgb_front;
float contrast;
{
  float *hls_back, *hls_front, old_contrast, upper, lower, maxsat(), *rgb_to_hls();
  int *hls_to_rgb();

  hls_back = (float *) rgb_to_hls (rgb_back);
  hls_front = (float *) rgb_to_hls (rgb_front);

  bgrd_hue = *hls_back;   bgrd_light = *(hls_back+1);   bgrd_sat = *(hls_back+2);



  upper = *(hls_back+1) + contrast;  /* Compute light and dark values of foreground lightness */
  if (upper > 1.0)  upper = 1.0; 

  lower = *(hls_back+1) - contrast;
  if (lower < 0.0)  lower = 0.0; 

  old_contrast = *(hls_back+1) - *(hls_front+1);
  /*printf("old_contrast: %f\n", old_contrast);*/

  if (old_contrast < 0)     /* Light on Dark */ 

    if (upper > 1.0)
      if (maxsat(lower) >= *(hls_front+2)) {
	/*printf ("\nL on D, lower, no adjustment");*/
	*(hls_front+1) = lower;
	fgrd_hue = *hls_front;   fgrd_light = *(hls_front+1);   fgrd_sat = *(hls_front+2);
	return ((int *) hls_to_rgb(hls_front));
      }
      else {
	/*printf("\nL on D, lower, adjustment");*/
	*(hls_front+1) = lower;
	*(hls_front+2) = maxsat(lower);
	if (*(hls_front+2) == 0) *hls_front = -1.0;
	fgrd_hue = *hls_front;   fgrd_light = *(hls_front+1);   fgrd_sat = *(hls_front+2);
	return ((int *) hls_to_rgb(hls_front));
      }
    
    else 
      
      if (maxsat(upper) > *(hls_front+2)) {
	/*printf("\nL on D, upper, no adjustment");*/
	*(hls_front+1) = upper;
	fgrd_hue = *hls_front;   fgrd_light = *(hls_front+1);   fgrd_sat = *(hls_front+2);
	return ((int *) hls_to_rgb (hls_front));
      }
      else {
	/*printf("\nL on D, upper, adjustment");*/
	*(hls_front+1) = upper;
	*(hls_front+2) = maxsat(upper);
	if (*(hls_front+2) == 0) *hls_front = -1.0;
	fgrd_hue = *hls_front;   fgrd_light = *(hls_front+1);   fgrd_sat = *(hls_front+2);
	return ((int *) hls_to_rgb (hls_front));
      }
  

  else    /* Dark on Light */
    
    if (lower < 0.0)
      if (maxsat(upper) > *(hls_front+2)) {
	/*printf ("\nD on L, upper, no adjustment");*/
	*(hls_front+1) = upper;
	fgrd_hue = *hls_front;   fgrd_light = *(hls_front+1);   fgrd_sat = *(hls_front+2);
	return ((int *) hls_to_rgb (hls_front));
      }
      else {
	/*printf ("\nD on L, upper, adjustment");*/
	*(hls_front+1) = upper;
	*(hls_front+2) = maxsat(upper);
	if (*(hls_front+2) == 0) *hls_front = -1.0;
	fgrd_hue = *hls_front;   fgrd_light = *(hls_front+1);   fgrd_sat = *(hls_front+2);
	return ((int *) hls_to_rgb (hls_front));
      }
    else 
      if (maxsat(lower) > *(hls_front+2)) {
	/*printf ("\nD on L, lower, no adjustment");*/
	*(hls_front+1) = lower;
	fgrd_hue = *hls_front;   fgrd_light = *(hls_front+1);   fgrd_sat = *(hls_front+2);
	return ((int *) hls_to_rgb (hls_front));
      }
      else {
      /*printf ("\nD on L, lower, adjustment");*/
	*(hls_front+1) = lower;
	*(hls_front+2) = maxsat(lower);
	if (*(hls_front+2) == 0) *hls_front = -1.0;
	fgrd_hue = *hls_front;   fgrd_light = *(hls_front+1);   fgrd_sat = *(hls_front+2);
	return ((int *) hls_to_rgb (hls_front));
      }
}




/*-------------------------------------------------------Auxillary function for control_foreground_contrast().
                                                         Returns the maximum saturation value for a given lightness value. */
float maxsat (lightness)
float lightness;
{
  if (lightness == 0.500)
    return (1.0);

  if (lightness > 0.500)
    return (-2.0 * lightness + 2.0);

  if (lightness < 0.500)
    return (2 * lightness);
}


	

/*--------------------------------------------------------------------Converts RGB values into HLS */

float *rgb_to_hls (RGB)
int *RGB;
{
  float max, min, *hls, *rgb, rc, gc, bc, maxi(), mini();

  rgb = (float *) malloc (sizeof (float) * 3);
  hls = (float *) malloc (sizeof (float) * 3);

  /*printf("\nrgb_to_hls input: %d  %d  %d\n", *RGB, *(RGB+1), *(RGB+2));*/
  
  *rgb     = (float) (*RGB/255.0);
  *(rgb+1) = (float) (*(RGB+1)/255.0);
  *(rgb+2) = (float) (*(RGB+2)/255.0);

  /*printf("\n in func rgb: %f  %f  %f\n", *rgb, *(rgb+1), *(rgb+2));*/

  max = maxi(rgb);
  min = mini(rgb);  /*printf("max: %f   min: %f\n", max, min);*/

  /* Lightness */
  *(hls+1) = (max+min)/2;   /*printf("in funcs Lightness: %f\n", *(hls+1));*/

  /* Saturation */
  if (max == min) { /* a gray */
    *(hls+2) = 0;                     /*printf("infunc gray Saturation: %f  Hue: %f\n", *(hls+2), *hls);*/
    *hls = -1;
  }
  else  {  /* not a gray */
    if (*(hls+1) <= 0.5) {
      *(hls+2) = (max-min)/(max+min);      /*printf("infunc not a gray, L <= 0.5,  Saturation: %f\n", *(hls+2));*/ }
    else {
      *(hls+2) = (max-min)/(2-max-min);    /*printf("infunc not a gray, L > 0.5,  Saturation: %f\n", *(hls+2));*/ }

    /* Hue */
    rc = (max - *rgb)/(max-min);
    gc = (max - *(rgb+1))/(max-min);
    bc = (max - *(rgb+2))/(max-min);
    
    if (*rgb == max) *hls = bc-gc;
    else if (*(rgb+1) == max) *hls = 2+rc-bc;
    else if (*(rgb+2) == max) *hls = 4+gc-rc;
    
    *hls = *hls * 60;    
    
    if (*hls < 0.0) *hls += 360;

    /*printf("\nrgb_to_hls output: %f %f %f\n", *hls, *(hls+1), *(hls+2));*/
  }
  free( rgb);			/* *DKY-10Apr91* */
  return (hls);
}
  

/*---------------------------------------------------------------------Converts HLS values to RGB */
int *hls_to_rgb (HLS)
float *HLS;
{
  int *rgb;
  float m1, m2, rf, gf, bf, value();

  rgb = (int *) malloc (sizeof(int) * 3);



  if (*(HLS+1) <= 0.5)  m2 = *(HLS+1) * (1 + *(HLS+2));
  else                  m2 = *(HLS+1) + *(HLS+2) - ((*(HLS+1)) * (*(HLS+2)));

  m1 = 2 * (*(HLS+1)) - m2;

  if (*(HLS+2) == 0.0)
    if (*HLS == -1.0) 
      rf = gf = bf = (float) *(HLS+1);
    else
      {
	printf("ERROR hls_to_rgb, When saturation = 0 Hue should be set to -1 (undefined)");
	*rgb =  255;    *(rgb+1) = 255;    *(rgb+2) = 255;  
	return(rgb);
      }
  else
    {
      rf = (float) value(m1, m2, *HLS + 120);
      
      gf = (float) value (m1, m2, *HLS);

      bf = (float) value (m1, m2, *HLS - 120);
    }
  *rgb = (int) (rf*255);    *(rgb+1) = (int) (gf*255);    *(rgb+2) = (int) (bf*255);  
#if (0)
  if ( *rgb > 255 || *(rgb+1) > 255  ||  *(rgb+2) > 255 ) {
    printf ("\n--------------------------------------------------------");
    printf ("\nhls_to_rgb input: %f   %f  %f\n\n", *HLS, *(HLS+1), *(HLS+2));
    printf ("\nhls_to_rgb output: %d  %d  %d\n", *rgb, *(rgb+1), *(rgb+2));
    printf ("\n--------------------------------------------------------");
  }
#endif
  return (rgb);
}

      
  





/*---------------------------------------------------------------------Auxiliary function for hls_to_rgb */
float value (n1, n2, hue)
float n1, n2, hue;
{
  if (hue > 360)  hue -= 360;
  if (hue < 0)    hue += 360;

  if (hue < 60)  {
    /*printf("\nIn value, returned: %f\n",(float) (n1 + (n2 - n1) * hue / 60));*/
    return ((float) (n1 + (n2 - n1) * hue / 60));
  }
  else if (hue < 180) {
    /*printf("\nIn value, returned: %f\n",(float)n2);*/
    return ((float)n2);
  }
  else if (hue < 240) {
    /*printf("\nIn value, returned: %f\n",(float)(n1 + (n2 - n1) * (240 - hue) / 60));*/
    return ((float)(n1 + (n2 - n1) * (240 - hue) / 60));
  }
  else {
    /*printf("\nIn value, returned: %f\n",(float)n1);*/
    return ((float)n1);
  }
}







/*---------------------------------------------------------------------Returns the max value of an array of 3 float */

float maxi (A)
float *A;
{
  int i;
  float max;

  max = *A;

  for (i=1 ; i<3 ; i++)
    if (max < *(A+i)) max = *(A+i);

  return(max);
}



/*--------------------------------------------------------------------Returns the minimum value of an array of three float */

float mini (A)
float *A;
{
  int i;
  float min;

  min = *A;

  for (i=1 ; i<3 ; i++)
    if (min > *(A+i)) min = *(A+i);

  return(min);
}





/*-------------------------You are entering MUNSELL territories------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------*/
/*                                                                                                             */
/*     THE FOLLOWING CODE PERFORMS VALUE CONTRAST ADAPTATION USING THE MUNSELL COLOR SPECIFICATION SYSTEM      */
/*                                                                                                             */
/*-------------------------------------------------------------------------------------------------------------*/


  
int *munsell_control_foreground_contrast (rgb_back, rgb_front, contrast)
int *rgb_back, *rgb_front;
float contrast;
{
  float *mun_back, *mun_front, old_contrast, upper, lower, *rgb_to_mun();
  int *mun_to_rgb();


#if DEBUG
  printf("\nIn munsell_control_foreground_contrast() rb %d  gb %d  bb %d", *rgb_back, *(rgb_back+1), *(rgb_back+2));
  printf("\nIn munsell_control_foreground_contrast() rf %d  gf %d  bf %d", *rgb_front, *(rgb_front+1), *(rgb_front+2));
#endif

  mun_back = (float *) rgb_to_mun (rgb_back);
  mun_front = (float *) rgb_to_mun (rgb_front);

#if DEBUG
  printf("\nIn munsell_control_foreground_contrast() hb %.3f  vb %.3f  cb %.3f", *mun_back, *(mun_back+1), *(mun_back+2));
  printf("\nIn munsell_control_foreground_contrast() hf %.3f  vf %.3f  cf %.3f", *mun_front, *(mun_front+1), *(mun_front+2));
#endif


  bgrd_H = *mun_back;   bgrd_V = *(mun_back+1);   bgrd_C = *(mun_back+2);


  upper = *(mun_back+1) + contrast;  /* Compute light and dark values of foreground lightness */

  lower = *(mun_back+1) - contrast;

  old_contrast = *(mun_back+1) - *(mun_front+1);
  /*printf("old_contrast: %f\n", old_contrast);*/



  if (old_contrast < 0)     /* Light on Dark */ 

    if (upper > 9.0) {         /* Munsell V greater than upper bound */
	*(mun_front+1) = lower;
	fgrd_H = *mun_front;   fgrd_V = *(mun_front+1);   fgrd_C = *(mun_front+2);
	return ((int *) mun_to_rgb(mun_front));
      }
    
    else {
	*(mun_front+1) = upper;   /* Munsell V within upper bound */
	fgrd_H = *mun_front;   fgrd_V = *(mun_front+1);   fgrd_C = *(mun_front+2);
	return ((int *) mun_to_rgb (mun_front));
      }
  

  else    /* Dark on Light */
    
    if (lower < 0.10) {           /* Munsell V smaller than lower bound */
	*(mun_front+1) = upper;
	fgrd_H = *mun_front;   fgrd_V = *(mun_front+1);   fgrd_C = *(mun_front+2);
	return ((int *) mun_to_rgb (mun_front));
      }
    else {
	*(mun_front+1) = lower;      /* Munsell V within lower bound */
	fgrd_H = *mun_front;   fgrd_V = *(mun_front+1);   fgrd_C = *(mun_front+2);
	return ((int *) mun_to_rgb (mun_front));
      }
}




/*----------------------The 2 following routine convert color specs from rgb to munsell and munsell to rgb_____*/

float *rgb_to_mun (RGB)
int *RGB;
{
  float *HVC;

#if DEBUG
  printf("\nIn rgb_to_mun() *RGB %d  *(RGB+1) %d  *(RGB+2) %d", *RGB, *(RGB+1), *(RGB+2));
#endif

  HVC = (float *) malloc (sizeof (float) * 3);

  if (rgb2mun (*RGB, *(RGB + 1), *(RGB + 2), HVC, HVC+1, HVC+2) == -1) {
    *HVC = 0.0;
    *(HVC+1) = 0.0;
    *(HVC+2) = 0.0;

#if DEBUG
  printf("\nIn rgb_to_mun() rgb2mun() returned -1  *HVC %.3f  *(HVC+1) %.3f  *(HVC+2) %.3f", *HVC, *(HVC+1), *(HVC+2));
#endif

    return ((float *)HVC);
  }

#if DEBUG
  printf("\nIn rgb_to_mun()  *HVC %.3f  *(HVC+1) %.3f  *(HVC+2) %.3f", *HVC, *(HVC+1), *(HVC+2));
#endif

  return((float*)HVC);
}



/*------------------------------------------------This routine is specific to aqdaptive text, it transforms MUNSELL values into RGBs providing
                                                  for cases when the chroma value of the color it gets as argument happens to be oobound.
                                                  It reduces its munsell chroma iteratively by 5% until the color becomes valid. */ 
int *mun_to_rgb (HVC)
float *HVC;
{
  int *RGB, done = 0, count = 1;

  RGB = (int *) malloc (sizeof (int) * 3);

#if DEBUG   
  printf ("\nmun_to_rgb()  *HVC %.3f   *(HVC+1) %.3f   *(HVC+2) %.3f", *HVC, *(HVC+1), *(HVC+2));
#endif 

  if (*(HVC+1) < 1.0) *(HVC+1) = 1.0;

  while (!done) 
    if (mun2rgb (*HVC, *(HVC+1), *(HVC+2), RGB, RGB+1, RGB+2) == -1) {

#if DEBUG
      printf ("\nHVC %d    chroma = %f", count, *(HVC+2));
#endif

      *(HVC+2) = (*(HVC+2)/100)*95;
      count++;
    }
    else
      done = 1;

  fgrd_C = *(HVC+2);

  if (!done) {
    *RGB = 0; 
    *(RGB+1) = 0;
    *(RGB+2) = 0;
    printf ("\nmun_to_rgb() adaptive.c failed");
    return((int *)RGB);
  }

  return((int *)RGB);
}



/*------------------------------------------------------------------------------------------*/



int *draw_darpa_adapt_box(W, font, size, bits, width, height, x, y, r, g, b, flag)
struct Window *W;
char *font;
int width, height, x, y, r, g, b, flag;
{
  int i, count = 0, screen, w;
  char *adaptst, *single;

  echo(1);

  if (flag)
    w = width/3;
  else
    w = width;

  screen = W->display->fildes;

  /*printf ("\ndraw_adapt_box  W %s  width %d  height %d  x %d  y %d", W->tag, width, height, x, y);*/
  adaptst = (char *) malloc (sizeof(char)*width/3+3);
  single = (char *) malloc (sizeof (char)*2);

  single[0] = (char) BACKCHAR;
  single[1] = '\0';

  adaptst[0] = (char) LEFTEND;

  for (i = 1 ; i <= width/3 ; i++)  {
    adaptst[i] = (char) BACKCHAR;
    count++;
  }

  adaptst[count+1] = (char) RIGHTEND;
  adaptst[count+2] = '\0';

  printf("\n----%s",adaptst);
  printf("\n-----%s",single);
  printf("\nDraw_darpa_adapt_box font %s  size %d  bits %d", font, size, bits);

  if (set_font (font, size, bits) == 0) 
      printf("\n ERROR set_font, draw_adapt_box(), adaptive.c in colorsource\n"); 

  set_text_clip_rectangle (x-3, y-3, x+w+60, y+height+6);
  set_font_color ( r, g, b, 0);
  display_string ((char *)adaptst, x, y);

  if(flag) {
    display_string ((char *)single, x+w+12, y);
    display_string ((char *)single, x+w+14, y);
    display_string ((char *)single, x+w+17, y);

    
    line_color(screen, 0.0, 0.0, 0.6);
    move2d(screen, (float)(x-(w+30)), (float)(y+11));
    draw2d(screen, (float)(x+w*4), (float)(y+11));
    move2d(screen, (float)(x-(w+30)), (float)(y+24));
    draw2d(screen, (float)(x+w*4), (float)(y+24));
    
    make_picture_current(screen);
  }
  free(adaptst); free(single);

  echo(0);
  return(0);
}



draw_silly_box(W, width, height, x, y, r, g, b)
struct Window *W;
int width, height, x, y;
{
  int *rgb, *find_average_bkgrd_color(), screen;


  screen = W->display->fildes;
 
  DrawRect(screen, x, y, x+width, y+height, r, g, b, 0, 0,0,0, 0);
}

