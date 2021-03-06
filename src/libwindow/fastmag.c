#include <starbase.c.h>
#include <stdio.h>



FastMag(screen, x1, y1, width, height, x2, y2, pixelsize, linestat, r, g, b)     /* v 1.0b */
     int screen;
     int x1, y1, width, height, x2, y2, pixelsize, linestat, r, g, b;
{
  int starty;
  int x, xindex, xx;
  int y, yindex, yy;
  int offset, bigwide, bigtall;

  bigwide = pixelsize*width;
  bigtall = pixelsize*height;
  line_color(screen, (float)r/255.0, (float)g/255.0, (float)b/255.0);
  move2d(screen, (float)(x2+bigwide), (float)(y2));
  draw2d(screen, (float)(x2+bigwide), (float)(y2+bigtall));
  draw2d(screen, (float)(x2), (float)(y2+bigtall));

  x = x2+bigwide-1;
  for (xindex = width-1; xindex >=0 ; xindex--)
    {
      for (xx = 0; xx< pixelsize-linestat; xx++)
	dcblock_move(screen, x1+xindex, y1, 1, height+1, x--, y2);
      if (linestat)
	dcblock_move(screen, x2+bigwide, y2, 1, height+1, x--, y2);
    }

  y = y2+bigtall-1;;
  for (yindex = height-1; yindex >=0 ; yindex--)
    {
      for (yy = 0; yy< pixelsize-linestat; yy++)
	dcblock_move(screen, x2, y2+yindex, bigwide, 1, x2, y--);
      if (linestat)
	dcblock_move(screen, x2, y2+bigtall, bigwide, 1, x2, y--);
    }
}


/* This version works for the munsell palette. BS 20Sep90*/
DamagedFastMag(screen, x1, y1, width, height, x2, y2, pixelsize, linestat, r, g, b)     /* v 1.0b */
     int screen;
     int x1, y1, width, height, x2, y2, pixelsize, linestat, r, g, b;
{
  int starty;
  int x, xindex, xx;
  int y, yindex, yy;
  int offset, bigwide, bigtall;

  bigwide = pixelsize*width;
  bigtall = pixelsize*height;
  line_color(screen, (float)r/255.0, (float)g/255.0, (float)b/255.0);
  move2d(screen, (float)(x2+bigwide), (float)(y2));
  draw2d(screen, (float)(x2+bigwide), (float)(y2+bigtall));
  draw2d(screen, (float)(x2), (float)(y2+bigtall));

  x = x1+bigwide-1;
  for (xindex = width-1; xindex >=0 ; xindex--)
    {
      for (xx = 0; xx< pixelsize-linestat; xx++)
	dcblock_move(screen, x1+xindex, y1, 1, height+1, x--, y1);
      if (linestat)
	dcblock_move(screen, x2+bigwide, y2, 1, height+1, x--, y1);
    }

  y = y2+bigtall-1;;
  for (yindex = height-1; yindex >=0 ; yindex--)
    {
      for (yy = 0; yy< pixelsize-linestat; yy++)
	dcblock_move(screen, x1, y1+yindex, bigwide, 1, x2, y--);
      if (linestat)
	dcblock_move(screen, x2, y2+bigtall, bigwide, 1, x2, y--);
    }
}


/* This version works for the CRX munsell palette. BS 26May92*/
/* it does not use block moves.  instead it takes as input a one-d buffer that is as large as the palette's longest
   dimension.  that buffer is used to block reads and writes */

BrainDamagedFastMag(screen, lbuf, x1, y1, width, height, x2, y2, pixelsize, linestat, r, g, b)
     int screen;
     int x1, y1, width, height, x2, y2, pixelsize, linestat, r, g, b;
     unsigned char *lbuf;
{
  int starty;
  int x, xindex, xx;
  int y, yindex, yy;
  int offset, bigwide, bigtall;

  bigwide = pixelsize*width;
  bigtall = pixelsize*height;
  line_color(screen, (float)r/255.0, (float)g/255.0, (float)b/255.0);
  move2d(screen, (float)(x2+bigwide), (float)(y2));
  draw2d(screen, (float)(x2+bigwide), (float)(y2+bigtall));
  draw2d(screen, (float)(x2), (float)(y2+bigtall));

  x = x1+bigwide-1;
  for (xindex = width-1; xindex >=0 ; xindex--)
    {
      for (xx = 0; xx< pixelsize-linestat; xx++) {
	dcblock_read(screen,x1+xindex,y1,1,height+1,lbuf,FALSE);
	dcblock_write(screen,x--,y1,1,height+1,lbuf,FALSE);
      }

      if (linestat) {
/*	dcmove(screen, x, y2);
	dcdraw(screen, x--, y2+bigtall); */

	dcblock_read(screen,x2+bigwide,y2,1,height+1,lbuf,FALSE);
	dcblock_write(screen,x--,y1,1,height+1,lbuf,FALSE);

      }
    }

  y = y2+bigtall-1;;
  for (yindex = height-1; yindex >=0 ; yindex--)
    {
      for (yy = 0; yy< pixelsize-linestat; yy++) {
	dcblock_read(screen,x1,y1+yindex,bigwide,1,lbuf,FALSE);
	dcblock_write(screen,x2,y--,bigwide,1,lbuf,FALSE);
      }
      if (linestat) {
/*	dcmove(screen, x2, y);
	dcdraw(screen, x2+bigwide, y--); */
	dcblock_read(screen,x2,y2+bigtall,bigwide,1,lbuf,FALSE);
	dcblock_write(screen,x2,y--,bigwide,1,lbuf,FALSE);
      }
    }

}




FastScale(screen,orgx,orgy,tempx,tempy,tempx2,tempy2,width,height,finalwidth,finalheight)
int screen, orgx,orgy,tempx,tempy,tempx2,tempy2,width,height,finalwidth,finalheight;
{
  float r,tr;
  int x,y, offset;

  /*  do yscale */
  r = (float)width/(float)finalwidth;
  tr = 0.0;
  for (x = 0; x< finalwidth; x++)
    {
      tr += r;
      dcblock_move(screen, orgx+(int)tr, orgy, 1, height, tempx+x,tempy);
    }

  r = (float)height/(float)finalheight;
  tr = 0.0;
  for (y = 0; y< finalheight; y++)
    {
      tr += r;
      dcblock_move(screen, tempx,tempy+(int)tr, finalwidth, 1, tempx2,tempy2+y);
    }
}




FastOverlapScale(screen,orgx,orgy,tempx,tempy,tempx2,tempy2,width,height,finalwidth,finalheight)
int screen, orgx,orgy,tempx,tempy,tempx2,tempy2,width,height,finalwidth,finalheight;
{
  float r,tr;
  int x,y, offset;

  /*  do yscale */
  r = (float)width/(float)finalwidth;
  tr = (finalwidth-1)*r;
  for (x = finalwidth-1; x >= 0; x--)
    {
      tr -= r;
      dcblock_move(screen, orgx+(int)tr, orgy, 1, height, tempx+x,tempy);
    }

  r = (float)height/(float)finalheight;
  tr = 0.0;
  for (y = 0; y< finalheight; y++)
    {
      tr += r;
      dcblock_move(screen, tempx,tempy+(int)tr, finalwidth, 1, tempx2,tempy2+y);
    }
}




FastVertScale(screen,orgx,orgy,tempx,tempy,tempx2,tempy2,width,height,finalwidth,finalheight)
int screen, orgx,orgy,tempx,tempy,tempx2,tempy2,width,height,finalwidth,finalheight;
{
  float r,tr;
  int x,y, offset;

  /*  do yscale */
  r = (float)width/(float)finalwidth;
  tr = 0.0;
  for (y = 0; y< finalheight; y++)
    {
      tr += r;
      dcblock_move(screen, orgx,orgy+(int)tr, width, 1, tempx,tempy+y);
    }

  r = (float)height/(float)finalheight;
  tr = 0.0;
  for (x = 0; x< finalwidth; x++)
    {
      tr += r;
      dcblock_move(screen, tempx+(int)tr, tempy, 1, finalheight, tempx2+x,tempy2);
    }
}


