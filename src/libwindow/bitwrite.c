/*** Block Transfer all of source buffer to dx, dy in destination buffer ***/



block_write24(control, sbuf, sbufw, sbufh, dbuf, dbufw, dbufh, dx, dy, x1, y1, x2, y2)     /* v 1.0b */
     unsigned char *sbuf, *dbuf, *control;
     int sbufw, sbufh, dbufw, dbufh, dx, dy, x1, y1, x2, y2;
{
  register unsigned char *dpos, *spos, *done;
  register int xcount, xgap, counter, sgap, scount, scounter;
  int xcut, ycut, xoff=0, yoff=0;

  if ((dx>x2) || (dy>y2) || (dx+sbufw <= x1) || (dy+sbufh <= y1)) return;

  xcut = sbufw;
  ycut = sbufh;
  if (x1-dx > 0) xoff = x1-dx;
  if (y1-dy > 0) yoff = y1-dy;
  if (dx+sbufw-1 - x2 >0) xcut = x2-dx+1;
  if (dy+sbufh-1 - y2 >0) ycut = y2-dy+1;
  xcut -= xoff;
  ycut -= yoff;

  dpos = dbuf+dx+(dy+yoff)*dbufw+xoff;
  spos = sbuf+xoff*3+(yoff*sbufw*3);

  done = sbuf+(xoff+xcut)*3+(yoff+ycut-1)*(sbufw*3);
  xgap = dbufw-xcut;
  sgap = sbufw-xcut;
  counter = xcount = xcut;

  control[0x0003] = 0x04;

  if (counter)
    while (spos<done)
      {
	control[0x40bf] = 4;
	*dpos   = *spos++;
	control[0x40bf] = 2;
	*dpos   = *spos++;
	control[0x40bf] = 1;
	*dpos++ = *spos++;

	if (!(--xcount)) {dpos += xgap; spos += sgap*3; xcount = counter;}
      }

  control[0x40bf] = 7;
}


shape_write24(control, sbuf, sbufw, sbufh, dbuf, dbufw, dbufh, dx, dy, clear, x1, y1, x2, y2)     /* v 1.0b */
     unsigned char *sbuf, *dbuf, *control;
     int sbufw, sbufh, dbufw, dbufh, dx, dy, x1, y1, x2, y2, clear;
{
  register unsigned char *dpos, *spos, *done;
  register int xcount, xgap, counter, sgap, scount, scounter;
  int xcut, ycut, xoff=0, yoff=0;

  if ((dx>x2) || (dy>y2) || (dx+sbufw <= x1) || (dy+sbufh <= y1)) return;

  xcut = sbufw;
  ycut = sbufh;
  if (x1-dx > 0) xoff = x1-dx;
  if (y1-dy > 0) yoff = y1-dy;
  if (dx+sbufw-1 - x2 >0) xcut = x2-dx+1;
  if (dy+sbufh-1 - y2 >0) ycut = y2-dy+1;
  xcut -= xoff;
  ycut -= yoff;

  dpos = dbuf+dx+(dy+yoff)*dbufw+xoff;
  spos = sbuf+xoff*3+(yoff*sbufw*3);

  done = sbuf+(xcut+xoff)*3+(yoff+ycut-1)*(sbufw*3);
  xgap = dbufw-xcut;
  sgap = sbufw-xcut;
  counter = xcount = xcut;

  control[0x0003] = 0x04;

  if (counter)
    while (spos<done)
      {
	if (*spos != clear) {
	  control[0x40bf] = 4;
	  *dpos = *spos;
	  control[0x40bf] = 2;
	  *dpos = *(spos+1);
	  control[0x40bf] = 1;
	  *dpos = *(spos+2);
        }
	dpos++;
	spos += 3;

	if (!(--xcount)) {dpos += xgap; spos += sgap*3; xcount = counter;}
      }

  control[0x40bf] = 7;
}


shape_write32(control, sbuf, sbufw, sbufh, dbuf, dbufw, dbufh, dx, dy, clear, x1, y1, x2, y2)     /* v 1.0b */
     unsigned char *sbuf, *control;
     int *dbuf, sbufw, sbufh, dbufw, dbufh, dx, dy, x1, y1, x2, y2, clear;
{
  register unsigned char *spos, *done;
  register int *dpos;
  register int xcount, xgap, counter, sgap, scount, scounter, color;
  int xcut, ycut, xoff=0, yoff=0;

  if ((dx>x2) || (dy>y2) || (dx+sbufw <= x1) || (dy+sbufh <= y1)) return;

  xcut = sbufw;
  ycut = sbufh;
  if (x1-dx > 0) xoff = x1-dx;
  if (y1-dy > 0) yoff = y1-dy;
  if (dx+sbufw-1 - x2 >0) xcut = x2-dx+1;
  if (dy+sbufh-1 - y2 >0) ycut = y2-dy+1;
  xcut -= xoff;
  ycut -= yoff;

  dpos = dbuf+dx+(dy+yoff)*dbufw+xoff;
  spos = sbuf+xoff*3+(yoff*sbufw*3);

  done = sbuf+(xcut+xoff)*3+(yoff+ycut-1)*(sbufw*3);
  xgap = dbufw-xcut;
  sgap = sbufw-xcut;
  counter = xcount = xcut;

  if (counter)
    while (spos<done)
      {
	if (*spos != clear) {
	  color = ((*spos)<<16) | ((*(spos+1))<<8) | (*(spos+2));
	  *dpos = color;
        }
	dpos++;
	spos += 3;

	if (!(--xcount)) {dpos += xgap; spos += sgap*3; xcount = counter;}
      }
}


shape_write24to4(sbuf, sbufw, sbufh, dbuf, dbufw, dbufh, dx, dy, clear, x1, y1, x2, y2)     /* v 1.0b */
     unsigned char *sbuf, *dbuf;
     int sbufw, sbufh, dbufw, dbufh, dx, dy, x1, y1, x2, y2, clear;
{
  register unsigned char *dpos, *spos, *done;
  register int xcount, xgap, counter, sgap, scount, scounter;
  int xcut, ycut, xoff=0, yoff=0;

  if ((dx>x2) || (dy>y2) || (dx+sbufw <= x1) || (dy+sbufh <= y1)) return;

  xcut = sbufw;
  ycut = sbufh;
  if (x1-dx > 0) xoff = x1-dx;
  if (y1-dy > 0) yoff = y1-dy;
  if (dx+sbufw-1 - x2 >0) xcut = x2-dx+1;
  if (dy+sbufh-1 - y2 >0) ycut = y2-dy+1;
  xcut -= xoff;
  ycut -= yoff;

  dpos = dbuf+dx+(dy+yoff)*dbufw+xoff;
  spos = sbuf+xoff*3+(yoff*sbufw*3);

  done = sbuf+(xoff+xcut)*3+(yoff+ycut-1)*(sbufw*3);
  xgap = dbufw-xcut;
  sgap = sbufw-xcut;
  counter = xcount = xcut;

  if (counter)
    while (spos<done)
      {
	if (*spos != clear)
/*	  *dpos = ((*spos * 77) + (*(spos+1) * 150) + (*(spos+2) * 28)) >> 12;  */
	  *dpos = ((((*spos)+1) >> 6)*25+
		   ((*(spos+1)+1) >> 6)*5+
		   ((*(spos+2)+1) >> 6)) + 16; 

	dpos++;
	spos += 3;

	if (!(--xcount)) {dpos += xgap; spos += sgap*3; xcount = counter;}
      }

}



/*** Replace one color with another ***/

SwapColors(sbuf, sbufw, sbufh, bcolor1, bcolor2, bcolor3, fcolor1, fcolor2, fcolor3)     /* v 1.0b */
     unsigned char *sbuf;
     int sbufw, sbufh;
     register unsigned char bcolor1, bcolor2, bcolor3,
                            fcolor1, fcolor2, fcolor3;
{
  register unsigned char *spos, *done;

  spos = sbuf;

  done = sbuf+sbufw*sbufh*3;

  while (spos<done)
    {
      if ((*spos == bcolor1)&&(*(spos+1) == bcolor2)&&(*(spos+2) == bcolor3))
	{
	  *spos = fcolor1;
	  *(spos+1) = fcolor2;
	  *(spos+2) = fcolor3;
	}
      spos+=3;
    }
}


/*** Some new functions, like the old shapewrites, but with transparency - dave small 9/91 ******/

shape_write_transp24(control, sbuf, sbufw, sbufh, dbuf, dbufw, dbufh, dx, dy, clear, x1, y1, x2, y2, t)     /* v 1.0b */
     unsigned char *sbuf, *dbuf, *control;
     int sbufw, sbufh, dbufw, dbufh, dx, dy, x1, y1, x2, y2, clear, t;
{
  register unsigned char *dpos, *spos, *done;
  register int xcount, xgap, counter, sgap, scount, scounter, inv_t;
  int xcut, ycut, xoff=0, yoff=0;

  if ((dx>x2) || (dy>y2) || (dx+sbufw <= x1) || (dy+sbufh <= y1)) return;

  inv_t = 255 - t;
  if (inv_t > 255) inv_t = 255;
  if (inv_t < 0) inv_t = 0;
  
  xcut = sbufw;
  ycut = sbufh;
  if (x1-dx > 0) xoff = x1-dx;
  if (y1-dy > 0) yoff = y1-dy;
  if (dx+sbufw-1 - x2 >0) xcut = x2-dx+1;
  if (dy+sbufh-1 - y2 >0) ycut = y2-dy+1;
  xcut -= xoff;
  ycut -= yoff;

  dpos = dbuf+dx+(dy+yoff)*dbufw+xoff;
  spos = sbuf+xoff*3+(yoff*sbufw*3);

  done = sbuf+(xcut+xoff)*3+(yoff+ycut-1)*(sbufw*3);
  xgap = dbufw-xcut;
  sgap = sbufw-xcut;
  counter = xcount = xcut;
  control[0x0003] = 4;

  if (counter)
    while (spos<done)
      {
	if (*spos != clear) {
	  if (t = 0) {
	    control[0x40bf] = 4;
	    *dpos = *spos;
	    control[0x40bf] = 2;
	    *dpos = *(spos+1);
	    control[0x40bf] = 1;
	    *dpos = *(spos+2);
	  }
	  else {
	    control[0x40bf] = 4;
	    *dpos += (*spos - *dpos) * inv_t >> 8;
	    control[0x40bf] = 2;
	    *dpos += (*(spos+1) - *dpos) * inv_t >> 8;
	    control[0x40bf] = 1;
	    *dpos += (*(spos+2) - *dpos) * inv_t >> 8;
	  }
	}
	dpos++;
	spos += 3;

	if (!(--xcount)) {dpos += xgap; spos += sgap*3; xcount = counter;}
      }

  control[0x40bf] = 7;
}


shape_write_transp32(control, sbuf, sbufw, sbufh, dbuf, dbufw, dbufh, dx, dy, clear, x1, y1, x2, y2, t)     /* v 1.0b */
     unsigned char *sbuf, *control;
     int *dbuf, sbufw, sbufh, dbufw, dbufh, dx, dy, x1, y1, x2, y2, clear, t;
{
  register unsigned char *spos, *done;
  register int *dpos;
  register int xcount, xgap, counter, sgap, scount, scounter, color, inv_t;
  register unsigned char red, green, blue;
  int xcut, ycut, xoff=0, yoff=0;

  if ((dx>x2) || (dy>y2) || (dx+sbufw <= x1) || (dy+sbufh <= y1)) return;

  inv_t = 255 - t;
  if (inv_t > 255) inv_t = 255;
  if (inv_t < 0) inv_t = 0;
  
  xcut = sbufw;
  ycut = sbufh;
  if (x1-dx > 0) xoff = x1-dx;
  if (y1-dy > 0) yoff = y1-dy;
  if (dx+sbufw-1 - x2 >0) xcut = x2-dx+1;
  if (dy+sbufh-1 - y2 >0) ycut = y2-dy+1;
  xcut -= xoff;
  ycut -= yoff;

  dpos = dbuf+dx+(dy+yoff)*dbufw+xoff;
  spos = sbuf+xoff*3+(yoff*sbufw*3);

  done = sbuf+(xcut+xoff)*3+(yoff+ycut-1)*(sbufw*3);
  xgap = dbufw-xcut;
  sgap = sbufw-xcut;
  counter = xcount = xcut;

  if (counter)
    while (spos<done)
      {
	if (*spos != clear) {
	  if ( t > 0) {
	    red = (unsigned char) ((*dpos & 0x00ff0000) >> 16);
	    green = (unsigned char) ((*dpos & 0x0000ff00) >> 8);
	    blue = (unsigned char) ((*dpos & 0x000000ff) >> 0);
	    
	    red += ((*spos - red ) * inv_t) >> 8;
	    green += (((*spos+1) - green ) * inv_t) >> 8;
	    blue += (((*spos+2) - blue ) * inv_t) >> 8;
	    
	    *dpos = ((int) ((red << 16) | (green << 8) | blue));
	  }
	  else {
	    color = ((*spos)<<16) | ((*(spos+1))<<8) | (*(spos+2));
	    *dpos = color;
	  }
        }
	dpos++;
	spos += 3;

	if (!(--xcount)) {dpos += xgap; spos += sgap*3; xcount = counter;}
      }
}

