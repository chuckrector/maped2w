// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                            MapEd v.2.01                             ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                         Screen Saver module                         ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#include <math.h>
#include <malloc.h>
#include <string.h>

#include "maped.h"
#include "timer.h"
#include "vdriver.h"

#include "mouse.h"
#include "log.h"

// ================================= Data ====================================

int *xlut = NULL;
int *ylut = NULL;
unsigned char *dscr = NULL;
//static int recalc_distort = 1;
//static long build = 0;

BITMAP* distort_screen=NULL;

// ================================= Code ====================================

static void init_distort(void)
{
	// *** TODO ***
  int n=0;

  xz=0;
  yz=0;
  xzd=2;
  yzd=1;

  if (xlut) { vfree(xlut); xlut=NULL; }
  if (ylut) { vfree(ylut); ylut=NULL; }
  //if (dscr) { vfree(dscr); dscr=NULL; }

  // allocate distort lookups
  xlut=(int *)valloc(sx*4,"ssaver X",0);
  ylut=(int *)valloc(sy*4,"ssaver Y",0);
  if (distort_screen!=NULL) destroy_bitmap(distort_screen);

  // allocate distort image buffer
  //dscr=(unsigned char *) valloc(sx*sy,"ssaver scrn",0);
  //save_screen=screen;
  //screen=(unsigned char *)valloc(tsx*tsy+(tsx*4), "ssaver screen",0);
  //screen+=(tsx*2);
  distort_screen=create_bitmap(sx,sy);
  //memcpy(screen,save_screen,tsx*tsy);

  //// copy viewport into distort buffer
  //memcpy(dscr, screen, tsx*tsy);
  blit(screen,distort_screen,0,0,0,0,sx,sy);

  //// lookups
  for (n=0; n<sy; n++)
    ylut[n]=cos(n*(360.0/sy)*(3.14/180))
    * ((sin((float)(sy-1-n)*(360.0/(float)sy)*(3.14/180))/2)*360/sy)
    *80;
  for (n=0; n<sx; n++)
    xlut[n]=sin(n*(360.0/sx)*(3.14/180))*((ylut[n/2]*360)/sx); //20;
}

void do_distort(void)
  {
	  // *** TODO ***
  static int xt=0;
  static int yt=0;
  //unsigned char *v=NULL;
  int x=0,y=0;
  int xx=0,yy=0;
  //int ytemp=0;

  BITMAP* offscreen=get_offscreen();
  //v=dscr; //screen; //dscr;
  bmp_select(offscreen);
  for (y=0; y<sy; y++)
  {
    yy=y+yz;
    while (yy<0) yy+=sy;
    while (yy>=sy) yy-=sy;
    yy=ylut[yy];

    for (x=0; x<sx; x++)
    {
      yt=y,xt=x;

      xx=x+xz*sx/sy;
      while (xx<0) xx+=sx;
      while (xx>=sx) xx-=sx;

      xt+=yy;
      yt+=xlut[xx];

      while (xt<0) xt+=sx;
      while (xt>=sx) xt-=sx;
      while (yt<0) yt+=sy;
      while (yt>=sy) yt-=sy;

      //*v++=dscr[xt+yt*tsx];
      //ytemp=(xt+yt*sx);
      //screen[ytemp]=
      //screen[ytemp-tsx]=
      //screen[ytemp+tsx]=*v++;
	  
	  //int color=getpixel(distort_screen,x,y);
	  //putpixel(get_offscreen(),xt,yt-1,color);
	  //putpixel(get_offscreen(),xt,yt,color);
	  //putpixel(get_offscreen(),xt,yt+1,color);

	  //_putpixel(get_offscreen(),x,y,_getpixel(distort_screen,xt,yt));

      bmp_write8((unsigned long)offscreen->line[y]+x, distort_screen->line[yt][xt]);
    }
  }
}

void ScreenSaver()
{
  //unsigned char *s=NULL;

  init_distort();
  ssaver_enabled=1;
  while (idlect > 100*60*3)
  {
    ReadMouse();
    do_distort();
    ShowPage();
  }
  ssaver_enabled=0;
  //s=screen-(tsx*2);
  //vfree(s);
  //screen=save_screen;
  timer_count=0;
  tick=0;
}
