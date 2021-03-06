/*
 * printmunsell.c
 * Joel Wachman
 * Monday, November 14, 1988 5:48:02 PM
 */

#include <stdio.h>
#include <munsell.h>
#include <cie.h>


static float   munciedata [NUMV] [NUMH] [NUMC] [2];


printmunsell()
{
  register hh,vv,cc;
  float xyY[3];
  
  for (vv=0;vv<NUMV;vv++)
    {
      munaccess( vv,0,0, xyY );
      for (hh=0;hh<NUMH;hh++)
	{
	  for (cc=0;cc<NUMC;cc++)
	    {
	      printf("\r%2d %2d %2d",vv,hh,cc);
	      if(munciedata[vv][hh][cc][0]<0.0 || munciedata[vv][hh][cc][0]>1.0)
		printf("munsel2cie[%d][%d][%d] = {%1.2f %1.2f}\n",
		       vv,hh,cc,munciedata[vv][hh][cc][0],munciedata[vv][hh][cc][1]);
	    }
	}
    }
  printf("\n");
}

OKAlert(fmt,longa,longb,longc,longd)
     char *fmt;
     long longa,longb,longc,longd;
{
  printf(fmt,longa,longb,longc,longd);
}
