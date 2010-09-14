// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                            MapEd v.2.01                             ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                        Tile Selector module                         ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "config.h"
#include "keyboard.h"
#include "maped.h"
#include "mouse.h"
#include "render.h"
#include "tileed.h"
#include "timer.h"
#include "vdriver.h"

#include "smalfont.h"
#include "guicomp.h"
#include "gui.h"
#include "tilesel.h"
#include "pcx.h"
#include "allegro.h"
#include "error.h"
#include "compress.h"
#include "ops.h"
#include "vcc.h"

extern unsigned char TileCmp(BITMAP *one, BITMAP *two); //zero

// ================================= Data ====================================

int selofs=0;
//char ttile[256], ttile2[256];
BITMAP* ttile = 0;
BITMAP* ttile2 = 0;
int curastd=0;
int curmode=0;

// ================================= Code ====================================

int curtileptr(int row) {
	int zoom=sx/320;
	int tile_inc=(16+pad)*zoom;
	int tile_count_x=sx/tile_inc;
	int x=(mouse_x)/tile_inc;
	int y=(mouse_y)/tile_inc;
	return row+(y*tile_count_x)+x;
}

void NormalButton(int x1, int y1, int x2, int y2, char *str)
{
  FilledBox(x1, y1, x2, y2, winbg);
  HLine(x1, y1, x2, black);
  VLine(x1, y1, y2, black);
  HLine(x1+1, y1+y2-1, x2-1, black);
  VLine(x1+x2-1, y1+1, y2-1, black);

  HLine(x1+1, y1+1, x2-2, brightw);
  VLine(x1+1, y1+1, y2-2, brightw);
  HLine(x1+2, y1+y2-2, x2-3, darkw);
  VLine(x1+x2-2, y1+2, y2-3, darkw);
  GotoXY(x1+((x2-pixels(str))/2), y1+3);
  printstring(str);
}

void PressdButton(int x1, int y1, int x2, int y2, char *str)
{
  FilledBox(x1, y1, x2, y2, winbg);
  HLine(x1, y1, x2, black);
  VLine(x1, y1, y2, black);
  HLine(x1+1, y1+y2-1, x2-1, black);
  VLine(x1+x2-1, y1+1, y2-1, black);

  HLine(x1+1, y1+1, x2-2, darkw);
  VLine(x1+1, y1+1, y2-2, darkw);
  GotoXY(x1+((x2-pixels(str))/2)+1, y1+4);
  printstring(str);
}

static int get_selector_page_height() {
	return (sy==200)?200:240;
}

void DrawHighlight(int x, int y, int width, int height, int thickness) {
	for (int index=0; index<thickness; index++) {
		HLine(x, y+index, width, white);
		HLine(x, y+height-1-index, width, white);
		VLine(x+index, y, height, white);
		VLine(x+width-1-index, y, height, white);
	}
}

static void UpDownButton(int x, int y, int w, int h, char* text, bool down) {
	if (down) PressdButton(x, y, w, h, text);
	else NormalButton(x, y, w, h, text);
}

void RenderTileSelector(BITMAP** the_vsp, int the_num_tiles,int selector_offset) {
	int zoom=sx/320;
	int tskip=(16+pad);
	int twide=320/tskip;
	int tdeep=((get_selector_page_height()+15)&~15)/tskip;
	tskip*=zoom;

	ClearScreen();

	for (int y=0; y<tdeep; ++y) {
		for (int x=0; x<twide; ++x) {
			int offset=(y*twide)+x+selector_offset;
			CopySprite((pad*zoom)+(x*tskip),(pad*zoom)+(y*tskip),16*zoom,16*zoom,the_vsp[(offset>=the_num_tiles)?0:offset]);
		}
	}

	if (th) {
		if (mouse_y<(sy-40) && mouse_x<(twide*tskip)) {
			DrawHighlight(
				(pad*zoom)+((mouse_x/tskip)*tskip)-(1*zoom),
				(pad*zoom)+((mouse_y/tskip)*tskip)-(1*zoom), (16+2)*zoom, (16+2)*zoom, zoom);
		}
	}
}
	
void RenderSelector() {
	RenderTileSelector(vsp,numtiles,selofs);

	int z=sy-40;
	sprintf(strbuf,"MapEd %s Tile Selector - Tile %d/%d",ME2_VERSION,curtileptr(selofs),numtiles);
	Window(16-16,z,336,z+40,strbuf);
	HLine(278-16, z+11, 53, darkw);
	VLine(278-16, z+11, 25, darkw);
	VLine(330-16, z+12, 24, brightw);
	HLine(279-16, z+35, 51, brightw);

	UpDownButton(290-16, z+12, 40, 12, "Select", (curmode==0));
	UpDownButton(290-16, z+23, 40, 12, "Edit", (curmode==1));
	UpDownButton(279-16, z+12, 12, 12, "H", th!=0);
	UpDownButton(279-16, z+23, 12, 12, "P", pad!=0);

	HLine(219-16, z+11, 52, darkw);
	VLine(219-16, z+11, 25, darkw);
	VLine(270-16, z+12, 24, brightw);
	HLine(220-16, z+35, 50, brightw);
	NormalButton(220-16, z+12, 50, 12, "Import VSP");
	NormalButton(220-16, z+23, 50, 12, "Import PCX");

	HLine(160-16, z+11, 52, darkw);
	VLine(160-16, z+11, 25, darkw);
	VLine(211-16, z+12, 24, brightw);
	HLine(161-16, z+35, 50, brightw);
	NormalButton(161-16, z+12, 50, 12, "Add Tiles");
	NormalButton(161-16, z+23, 50, 12, "Animation");

	HLine(26-16, z+10, 20, darkw);
	VLine(26-16, z+10, 20, darkw);
	HLine(27-16, z+29, 19, brightw);
	VLine(45-16, z+11, 19, brightw);
	FilledBox(27-16, z+11, 18, 18, black);
	CopySprite(28-16, z+12, 16, 16, ttile2);
	GotoXY(20-16, z+32); printstring("Clipboard");
}

void FlipX(unsigned short edtile)
{ int i,j;

  if (!ttile) ttile = create_bitmap(16,16);

  key[KEY_X]=0;
  if (edtile>=numtiles) return;
  //memcpy(ttile,&vsp[edtile*256],256);
  blit(vsp[edtile], ttile, 0, 0, 0, 0, 16, 16);
  modified=1;

  for (i=0;i<16;i++)
      for (j=0;j<16;j++)
          //vsp[((edtile)*256)+(j*16)+i]=ttile[(j*16)+(15-i)];
		  putpixel(vsp[edtile], i, j, getpixel(ttile, (15-i), j));
}

void FlipY(unsigned short edtile)
{ int i,j;

  if (!ttile) ttile = create_bitmap(16,16);

  key[KEY_Y]=0;
  if (edtile>=numtiles) return;
  //memcpy(ttile,&vsp[edtile*256],256);
  blit(vsp[edtile], ttile, 0, 0, 0, 0, 16, 16);
  modified=1;

  for (i=0;i<16;i++)
      for (j=0;j<16;j++)
          //vsp[((edtile)*256)+(j*16)+i]=ttile[((15-j)*16)+i];
		  putpixel(vsp[edtile], i, j, getpixel(ttile, i, (15-j)));
}

void FlipZ(unsigned short edtile)
{ int i,j;

  if (!ttile) ttile = create_bitmap(16,16);

  key[KEY_Z]=0;
  if (edtile>=numtiles) return;
  //memcpy(ttile,&vsp[edtile*256],256);
  blit(vsp[edtile], ttile, 0, 0, 0, 0, 16, 16);
  modified=1;

  for (i=0;i<16;i++)
      for (j=0;j<16;j++)
          //vsp[((edtile)*256)+(j*16)+i]=ttile[(i*16)+j];
		  putpixel(vsp[edtile], i, j, getpixel(ttile, j, i));
}

void DrawHighlight(int x, int y)
{
  //if (x<10 || y<10 || x>330 || y>sy+10) return;
  HLine(x, y, 18, white);
  HLine(x, y+17, 18, white);
  VLine(x, y, 18, white);
  VLine(x+17, y, 18, white);
}


void DrawBigHighlight(int x, int y) {
  HLine(x,y, 18*2, white);
  HLine(x,y+1,18*2,white);
  
  HLine(x,y+(17*2), 18*2, white);
  HLine(x,y+(17*2)+1, 18*2,white);
  
  VLine(x, y, 18*2, white);
  VLine(x+1,y,18*2,white);
  
  VLine(x+(17*2), y, 18*2, white);
  VLine(x+(17*2)+1,y,18*2, white);
}

void TileSelector()
{ int z=0, ct=0, k=0,j=0,m=0;
  //char *s;
  //char *d;

	if (!ttile) {
		ttile = create_bitmap(16,16);
		rectfill(ttile,0,0,16,16,0);
	}
	if (!ttile2) {
		ttile2 = create_bitmap(16,16);
		rectfill(ttile2,0,0,16,16,0);
	}

  int ms_down=0; // aen
  int ms_up=0;
  int zoom=sx/320;

  while (!key[KEY_ESC])
  {
     RenderSelector();
     RenderMessages();
     DrawMouse();
     ShowPage();

     ms_down=0;
     ms_up=0;
     if (mouse_scroll)
     {
       if (mouse_y>sy-2) ms_down=1;
       if (mouse_y<2) ms_up=1;
     }

     if ((key[KEY_DOWN] || (ms_down && (!(systemtime%10)))) && selofs<65319)
     {
		int tiles_per_row=sx/((16+pad)*zoom);
		selofs+=tiles_per_row;
		int rows=(numtiles+(tiles_per_row-1))/tiles_per_row;
		if (selofs>(rows-1)*tiles_per_row)
			selofs=(rows-1)*tiles_per_row;

       key[KEY_DOWN]=0;
       systemtime++;
     }
     if ((key[KEY_UP] || (ms_up && (!(systemtime%10)))) && selofs>=sx/((16+pad)*zoom))
     {
		 int tiles_per_row=sx/((16+pad)*zoom);
		 selofs-=tiles_per_row;
		 if (selofs<0)
			 selofs=0;

       key[KEY_UP]=0;
       systemtime++;
     }
     if (key[KEY_P])
     {
        key[KEY_P]=0;
        pad=pad ^ 1;
        selofs=0;
        if (pad) Message("Tile padding enabled.",100);
            else Message("Tile padding disabled.",100);
     }
     if (key[KEY_H])
     {
       key[KEY_H]=0;
       th=th ^ 1;
       if (th) Message("Tile Highlight enabled.",100);
          else Message("Tile Highlight disabled.",100);
     }

     if (key[KEY_X]) FlipX((unsigned short)curtileptr(selofs));
     if (key[KEY_Y]) FlipY((unsigned short)curtileptr(selofs));
     if (key[KEY_Z]) FlipZ((unsigned short)curtileptr(selofs));
     if (key[KEY_V]) VSPAnimDialog();
     if ((key_shifts & KB_CTRL_FLAG) && key[KEY_C])
     {
       //memcpy(ttile2, (char *) (vsp+(256*curtileptr(selofs))), 256);
	   blit(vsp[curtileptr(selofs)], ttile2, 0, 0, 0, 0, 16, 16);
       key[KEY_C]=0;
     }
     if ((key_shifts & KB_CTRL_FLAG) && key[KEY_S])
     {
       if (curtileptr(selofs) >= numtiles) return;
       //memcpy((char *) (vsp+(256*curtileptr(selofs))), ttile2, 256);
	   blit(ttile2, vsp[curtileptr(selofs)], 0, 0, 0, 0, 16, 16);
       modified=1;
       key[KEY_S]=0;
     }
     if ((key_shifts & KB_CTRL_FLAG) && key[KEY_T])
     {
       if (curtileptr(selofs) >= numtiles) return;
       modified=1;
	   masked_blit(ttile2, vsp[curtileptr(selofs)], 0, 0, 0, 0, 16, 16);
       //s=ttile2;
       //d=(char *) (vsp+(256*curtileptr(selofs)));
       //for (z=0; z<256; z++)
       //{
       //  if (*s) *d=*s;
       //  d++;
       //  s++;
       //}
       key[KEY_T]=0;
     }
     if ((key_shifts & KB_CTRL_FLAG) && key[KEY_D] && !(key_shifts & KB_ALT_FLAG))
     {
        key[KEY_D]=0;
        modified=1;
        z=curtileptr(selofs);
        if (z<numtiles)
        {
			DeleteTiles(z,1);
        }
     }
// zero: Super Delete Mode!
// wimpy delete irks me.
     if ((key_shifts & KB_CTRL_FLAG) && key[KEY_D] && (key_shifts & KB_ALT_FLAG))
     {
       key[KEY_D]=0;
       modified=1;
       z=curtileptr(selofs);
       //if(ct<numtiles)
       //{
         if(z<numtiles)
         {
			 DeleteTiles(z,1);
           for(k=0;k<numlayers;k++)
           {
             for(j=0; j<layer[k].sizey; j++)
             {
               for(m=0; m<layer[k].sizex; m++)
               {
                 if(layers[k][j*layer[k].sizex+m]==z) layers[k][j*layer[k].sizex+m]=0;
                 if(layers[k][j*layer[k].sizex+m]>z) layers[k][j*layer[k].sizex+m]--;
               }
             }
           }
         }
       //}
     }
// zero: Super Insert Mode!
// wimpy insert irks me.
     if ((key_shifts & KB_CTRL_FLAG) && key[KEY_I] && (key_shifts & KB_ALT_FLAG))
     {
       key[KEY_I]=0;
       ct=curtileptr(selofs);
       if (ct <= numtiles)
       {
         modified=1;
		 InsertTiles(ct,1);
       }
       for(k=0;k<numlayers;k++)
       {
         for(j=0; j<layer[k].sizey; j++)
         {
           for(m=0; m<layer[k].sizex; m++)
           {
             if(layers[k][j*layer[k].sizex+m]>=ct) layers[k][j*layer[k].sizex+m]++;
           }
         }
       }
     }

     if ((key_shifts & KB_CTRL_FLAG) && key[KEY_I] && !(key_shifts & KB_ALT_FLAG))
     {
       key[KEY_I]=0;
       ct=curtileptr(selofs);
       if (ct <= numtiles)
       {
         modified=1;
		 InsertTiles(ct,1);
       }
     }
     if (key[KEY_E])
     {
       modified=1;
       TEtile=curtileptr(selofs); // middle tile initially
       if (TEtile<numtiles)
       {
         TEtype=TETILE;
         TEsource=vsp[TEtile];//vsp+(256*TEtile);
         TExsize=16;
         TEysize=16;
         TileEdit();
       }
     }

     if (mouse_b==1 && mouse_y<(tsy-40) && !curmode)
     {
        if (curtileptr(selofs)<numtiles)
        {
          lt=curtileptr(selofs);
          key[KEY_ESC]=1;
        }
        WaitRelease();
      }
     if (mouse_b==2 && mouse_y<(tsy-40) && !curmode)
     {
        if (curtileptr(selofs)<numtiles)
        {
          rt=curtileptr(selofs);
          key[KEY_ESC]=1;
        }
        WaitRelease();
     }
     if (mouse_b && mouse_y<(tsy-40) && curmode)
     {
       WaitRelease();
       TEtile=curtileptr(selofs); // middle tile initially
       if (TEtile<numtiles)
       {
         modified=1;
         TEtype=TETILE;
         TEsource=vsp[TEtile];//vsp+(256*TEtile);
         TExsize=16;
         TEysize=16;
         TileEdit();
       }
     }
     z=tsy-40;
     if (mouse_b && mouse_x>290-16 && mouse_x<330-16 && mouse_y>z+12 && mouse_y<z+23)
     {
       curmode=0;
       WaitRelease();
       Message("Tile select mode.",100);
     }
     if (mouse_b && mouse_x>290-16 && mouse_x<330-16 && mouse_y>z+23 && mouse_y<z+47)
     {
       curmode=1;
       WaitRelease();
       Message("Tile edit mode.",100);
     }
     if (mouse_b && mouse_x>278-16 && mouse_x<289-16 && mouse_y>z+12 && mouse_y<z+23)
     {
       WaitRelease();
       th=th ^ 1;
       if (th) Message("Tile Highlight enabled.",100);
          else Message("Tile Highlight disabled.",100);
        mouse_b=0;
     }
     if (mouse_b && mouse_x>278-16 && mouse_x<289-16 && mouse_y>z+23 && mouse_y<z+47)
     {
       WaitRelease();
       selofs=0;
       pad=pad ^ 1;
       if (pad) Message("Tile padding enabled.",100);
           else Message("Tile padding disabled.",100);
        mouse_b=0;
     }
     if (mouse_b && mouse_x>161-16 && mouse_x<210-16 && mouse_y>z+12 && mouse_y<z+23)
     {
        modified=1;
        PressdButton(161-16, z+12, 50, 12, "Add Tiles");
        while (mouse_b)
        {
          ReadMouse();
          ShowPage();
        }
        WaitRelease();
        AddTilesDialog();
        mouse_b=0;
     }
     if (mouse_b && mouse_x>161-16 && mouse_x<210-16 && mouse_y>z+23 && mouse_y<z+47)
     {
        PressdButton(161-16, z+23, 50, 12, "Animation");
        while (mouse_b)
        {
          ReadMouse();
          ShowPage();
        }
        WaitRelease();
        VSPAnimDialog();
        mouse_b=0;
     }
     if (mouse_b && mouse_x>220-16 && mouse_x<270-16 && mouse_y>z+12 && mouse_y<z+24)
     {
        PressdButton(220-16, z+12, 50, 12, "Import VSP");
        while (mouse_b)
        {
          ReadMouse();
          ShowPage();
        }
        WaitRelease();
        ImportVSP();
        modified=1;
        mouse_b=0;
     }
     if (mouse_b && mouse_x>220-16 && mouse_x<270-16 && mouse_y>z+23 && mouse_y<z+47)
     {
        PressdButton(220-16, z+23, 50, 12, "Import PCX");
        while (mouse_b)
        {
          ReadMouse();
          ShowPage();
        }
        WaitRelease();
        ImportPCX();
        modified=1;
        mouse_b=0;
     }
     if (key[KEY_F10])
     {
      key[KEY_F10]=0;
      SaveMAP(mapname);
      SaveVSP(vspname);
	  CompileMap_forMaped(mapname);
      Message("MAP/VSP saved.",100);
      modified=0;
    }
  }
  key[KEY_ESC]=0;
}

int vaxsize=150, vaysize=66;
int vaxofs=20, vayofs=20;

void VSPAnimDialog()
{ int done=0, moving=0, i=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;
  int bleh;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80];

  bleh=th;
  th=0;
  i=curastd;
  sprintf(nstr1, "%d", vspanim[i].start);
  sprintf(nstr2, "%d", vspanim[i].finish);
  sprintf(nstr3, "%d", vspanim[i].delay);
  sprintf(nstr4, "%d", vspanim[i].mode);
  last_pressed=0;
  do
  {
    RenderSelector();
    RenderMessages();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      vaxofs = (mouse_x-mxo);
      vayofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (vaxofs<16) vaxofs = 16;
      if (vayofs<16) vayofs = 16;
      if (vaxofs+vaxsize>335) vaxofs = 336-vaxsize;
      if (vayofs+vaysize>sy+16) vayofs = (sy+16)-vaysize;
    }

    Window(vaxofs, vayofs, vaxofs+vaxsize, vayofs+vaysize, "VSP Animation Editor");
    CopySprite(vaxofs+110, vayofs+16, 16, 16, vsp[tileidx[vspanim[i].start]]/*(char *) (vsp+(256*tileidx[vspanim[i].start]))*/);
    GotoXY(vaxofs+4, vayofs+12); printstring("Start");
    TextField(vaxofs+25, vayofs+11, 60, nstr1, ctf == 1 ? cb : 0);
    GotoXY(vaxofs+11, vayofs+22); printstring("End");
    TextField(vaxofs+25, vayofs+21, 60, nstr2, ctf == 2 ? cb : 0);
    GotoXY(vaxofs+4, vayofs+32); printstring("Delay");
    TextField(vaxofs+25, vayofs+31, 60, nstr3, ctf == 3 ? cb : 0);
    GotoXY(vaxofs+6, vayofs+42); printstring("Mode");
    TextField(vaxofs+25, vayofs+41, 60, nstr4, ctf == 4 ? cb : 0);
    GotoXY(vaxofs+2, vayofs+56); printstring("Strand");
    GotoXY(vaxofs+104, vayofs+33); printstring("Preview");
    HLine(vaxofs+1, vayofs+51, 148, white);
    Button(vaxofs+85, vayofs+54, "OK");
    Button(vaxofs+117, vayofs+54, "Update");
    GotoXY(vaxofs+30, vayofs+56);
    sprintf(strbuf, "%d", i); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>vaxofs+25 && mouse_x<vaxofs+60+25 && mouse_y>vayofs+11 && mouse_y<vayofs+9+11 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>vaxofs+25 && mouse_x<vaxofs+60+25 && mouse_y>vayofs+21 && mouse_y<vayofs+9+21 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>vaxofs+25 && mouse_x<vaxofs+60+25 && mouse_y>vayofs+31 && mouse_y<vayofs+9+31 && !moving)
    {
      ctf=3; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>vaxofs+25 && mouse_x<vaxofs+60+25 && mouse_y>vayofs+41 && mouse_y<vayofs+9+41 && !moving)
    {
      ctf=4; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mouse_b==1 && mouse_x>vaxofs+85 && mouse_x<vaxofs+30+85 && mouse_y>vayofs+54 && mouse_y<vayofs+10+54 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(vaxofs+85, vayofs+54, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>vaxofs+117 && mouse_x<vaxofs+30+117 && mouse_y>vayofs+54 && mouse_y<vayofs+10+54 && !moving)
    {
      // insert code for button "Update" here
      vspanim[i].start=(unsigned short)atoi(nstr1);
      vspanim[i].finish=(unsigned short)atoi(nstr2);
      vspanim[i].delay=(unsigned short)atoi(nstr3);
      vspanim[i].mode=(unsigned short)atoi(nstr4);
      InitTileIDX();
      ButtonPressed(vaxofs+117, vayofs+54, "Update");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(vaxofs+vaxsize-9) && (mouse_x<vaxofs+vaxsize-2)
    && mouse_y>(vayofs+1) && (mouse_y<vayofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>vaxofs && mouse_x<vaxofs+vaxsize && mouse_y>vayofs && mouse_y<(vayofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-vaxofs;
      myo=mouse_y-vayofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=last_pressed & 0xff;//key_shift_tbl[last_pressed];
      //else c=(char)(last_pressed&0xff);//key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_UP)
      {
        vspanim[i].start=(unsigned short)atoi(nstr1);
        vspanim[i].finish=(unsigned short)atoi(nstr2);
        vspanim[i].delay=(unsigned short)atoi(nstr3);
        vspanim[i].mode=(unsigned short)atoi(nstr4);
        if (i<99) i++; else i=0;
        last_pressed=0; key[KEY_UP]=0;
        sprintf(nstr1, "%d", vspanim[i].start);
        sprintf(nstr2, "%d", vspanim[i].finish);
        sprintf(nstr3, "%d", vspanim[i].delay);
        sprintf(nstr4, "%d", vspanim[i].mode);
        continue;
      }
      if ((last_pressed>>8)==KEY_DOWN)
      {
        vspanim[i].start=(unsigned short)atoi(nstr1);
        vspanim[i].finish=(unsigned short)atoi(nstr2);
        vspanim[i].delay=(unsigned short)atoi(nstr3);
        vspanim[i].mode=(unsigned short)atoi(nstr4);
        if (i) i--; else i=99;
        last_pressed=0; key[KEY_DOWN]=0;
        sprintf(nstr1, "%d", vspanim[i].start);
        sprintf(nstr2, "%d", vspanim[i].finish);
        sprintf(nstr3, "%d", vspanim[i].delay);
        sprintf(nstr4, "%d", vspanim[i].mode);
        continue;
      }
      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=3; str=nstr3; break;
          case 3: ctf=4; str=nstr4; break;
          case 4: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

	  if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=4; str=nstr4; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
          case 3: ctf=2; str=nstr2; break;
          case 4: ctf=3; str=nstr3; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
//  memset(&vadelay, 0, 100);
//  for (i=0; i<numtiles; i++)
//    tileidx[i]=i;
  vspanim[i].start=(unsigned short)atoi(nstr1);
  vspanim[i].finish=(unsigned short)atoi(nstr2);
  vspanim[i].delay=(unsigned short)atoi(nstr3);
  vspanim[i].mode=(unsigned short)atoi(nstr4);
  curastd=i;
  th=bleh;
}

int atxsize=150, atysize=39;
int atxofs=80, atyofs=50;

void AddTilesDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink, at=0; //, i;
  int ctf=0, cb=0, c, t;
  char *str;
  int bleh;
  char nstr1[80];

  bleh=th;
  th=0;
  sprintf(nstr1, "%d", at);
  last_pressed=0;
  do
  {
    RenderSelector();
    RenderMessages();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      atxofs = (mouse_x-mxo);
      atyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (atxofs<16) atxofs = 16;
      if (atyofs<16) atyofs = 16;
      if (atxofs+atxsize>335) atxofs = 336-atxsize;
      if (atyofs+atysize>sy+16) atyofs = (sy+16)-atysize;
    }

    Window(atxofs, atyofs, atxofs+atxsize, atyofs+atysize, "Allocate Additional Tiles");
    GotoXY(atxofs+12, atyofs+15); printstring("Alloc Tiles:");
    TextField(atxofs+56, atyofs+13, 80, nstr1, ctf == 1 ? cb : 0);
    Button(atxofs+85, atyofs+26, "OK");
    Button(atxofs+117, atyofs+26, "Cancel");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>atxofs+56 && mouse_x<atxofs+80+56 && mouse_y>atyofs+13 && mouse_y<atyofs+9+13 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mouse_b==1 && mouse_x>atxofs+85 && mouse_x<atxofs+30+85 && mouse_y>atyofs+26 && mouse_y<atyofs+10+26 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      at=atoi(nstr1);
	  InsertTiles(numtiles,at);

	  ButtonPressed(atxofs+85, atyofs+26, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>atxofs+117 && mouse_x<atxofs+30+117 && mouse_y>atyofs+26 && mouse_y<atyofs+10+26 && !moving)
    {
      // insert code for button "Cancel" here
      done=1;
      ButtonPressed(atxofs+117, atyofs+26, "Cancel");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(atxofs+atxsize-9) && (mouse_x<atxofs+atxsize-2)
    && mouse_y>(atyofs+1) && (mouse_y<atyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>atxofs && mouse_x<atxofs+atxsize && mouse_y>atyofs && mouse_y<(atyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-atxofs;
      myo=mouse_y-atyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c = (char)(last_pressed & 0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
  at=atoi(nstr1);
  th=bleh;
//  memset(&vadelay, 0, 100);
//  for (i=0; i<numtiles; i++)
//    tileidx[i]=i;
}

// ========================= Import *** Data/Code ============================

BITMAP** ovsp = 0;//char *ovsp;
int op, oh, openas;
int ontiles;
int sofs=0;
int tilesrippedthissession;
unsigned char *implist; //zero

void RenderImport() {
	RenderTileSelector(ovsp,ontiles,sofs);
	
	int z=sy-40;
	char buffer[1024];
	sprintf(buffer,"Import Tiles %d/%d",curtileptr(sofs),ontiles);
	Window(16,z,336,z+40,buffer);
	printstring(176-(pixels(strbuf)/2),z+20,"Tiles imported this session: %d",tilesrippedthissession);
}

void ImportTiles()
{ //int tp;
//  char *str;
  int bloop1,bloop2;
  int zoom=sx/320;
  int inc=320/17;//(16+pad);//*zoom);
  int rows=(ontiles+(inc-1))/inc;
  //inc*=zoom;
  sofs=0;

  op=pad; oh=th;
  pad=1; th=1;
  tilesrippedthissession=0;
  implist=(unsigned char *)malloc(ontiles); //zero
  memset(implist,0,ontiles); //zero
  while (!key[KEY_ESC])
  {
    RenderImport();
    RenderMessages();
    DrawMouse();
    ShowPage();

     if (key[KEY_DOWN] && sofs<ontiles-inc)
     {
		 sofs+=inc;
       key[KEY_DOWN]=0;
     }
     if (key[KEY_UP] && sofs>=sx/((16+pad)*zoom))
     {
		 sofs-=inc;
       key[KEY_UP]=0;
     }
/*all by zero here in this scan_b thing*/
     if (key[KEY_B])
     {
       key[KEY_B]=0;
       Message("Building duplicate import list",200);
       for(bloop1=0; bloop1<ontiles; bloop1++)
       {
         for(bloop2=0; bloop2<numtiles; bloop2++)
         {
           if(TileCmp(vsp[bloop2]/*(unsigned char*)vsp+bloop2*256*/,ovsp[bloop1]/*(unsigned char*)ovsp+bloop1*256*/))
           {
             implist[bloop1]=1;
             bloop2=numtiles;
           }
         }
       }
     }
//all by zero here in this scan_a thing
     if (key[KEY_A])
     {
       Message("Importing all tiles",200);
       key[KEY_A]=0;
	   int start=numtiles;
	   InsertTiles(numtiles,ontiles);
   //    for(bloop1=0; bloop1<ontiles; bloop1++)
   //    {
   //      //str=(char *) valloc((numtiles+1)*256,"vsp data",0);
		 //BITMAP** tmp_bmp = new BITMAP*[numtiles+1];
		 //for (int index=0; index<numtiles; index++)
		 //  tmp_bmp[index]=vsp[index];
   //      //memcpy(str, vsp, (numtiles*256));
   //      //vfree(vsp); vsp=(unsigned char*)str; str=0;
		 ////delete[] vsp;
		 //vsp = tmp_bmp;
   //      //memcpy(vsp+(numtiles*256), ovsp+(bloop1*256), 256);
		 //blit(ovsp[bloop1], vsp[numtiles], 0, 0, 0, 0, 16, 16);
   //      numtiles+=1;
   //    }
	   for (bloop1=0; bloop1<ontiles; ++bloop1)
		   blit(ovsp[bloop1],vsp[start++],0,0,0,0,16,16);
       goto DIE;
     }

//all by zero here in this scan_n thing
     if (key[KEY_N])
     {
       Message("Importing all new tiles",200);
       key[KEY_N]=0;
       for(bloop1=0; bloop1<ontiles; bloop1++)
       {
         for(bloop2=0; bloop2<numtiles; bloop2++)
         {
           if(TileCmp(vsp[bloop2]/*(unsigned char*)vsp+bloop2*256*/,ovsp[bloop1]/*(unsigned char*)ovsp+bloop1*256*/))
           {
             implist[bloop1]=1;
             bloop2=numtiles;
           }
         }
       }
       for(bloop1=0; bloop1<ontiles; bloop1++)
       {
         if(!implist[bloop1])
         {
			 InsertTiles(numtiles,1);
     //      //str=(char *) valloc((numtiles+1)*256,"vsp data",0);
		   //BITMAP** tmp_bmp = new BITMAP*[numtiles+1];
		   //for (int index=0; index<numtiles; index++)
			  // tmp_bmp[index]=vsp[index];
     //      //memcpy(str, vsp, (numtiles*256));
     //      //vfree(vsp); vsp=(unsigned char*)str; str=0;
		   ////delete[] vsp;
		   //vsp = tmp_bmp;
		   //vsp[numtiles]=create_bitmap(16,16);
           //memcpy(vsp+(numtiles*256), ovsp+(bloop1*256), 256);
		   blit(ovsp[bloop1],vsp[numtiles-1],0,0,0,0, 16,16);
           //numtiles+=1;
         }
       }
       goto DIE;
     }

     if (mouse_b==1 && mouse_y<(tsy-40) && curtileptr(sofs)<ontiles)
     {
        lt=curtileptr(sofs);
        implist[lt]=1; //zero
		InsertTiles(numtiles,1);
  //      //str=(char *) valloc((numtiles+1)*256,"vsp data",0);
		//BITMAP** tmp_bmp = new BITMAP*[numtiles+1];
		//for (int index=0; index<numtiles; index++)
		//	tmp_bmp[index]=vsp[index];
  //      //memcpy(str, vsp, (numtiles*256));
  //      //vfree(vsp); vsp=(unsigned char*)str; str=0;
		//delete[] vsp;
		//vsp = tmp_bmp;
		//vsp[numtiles]=create_bitmap(16,16);
        //memcpy(vsp+(numtiles*256), ovsp+(lt*256), 256);
		blit(ovsp[lt], vsp[numtiles-1], 0,0,0,0, 16,16);
        //numtiles+=1;
        tilesrippedthissession++;
        WaitRelease();
        sprintf(strbuf,"Tile %d imported.",lt);
        Message(strbuf, 200);
     }
  }
  DIE: //zero
  pad=op; th=oh;
  vfree(implist); //zero
}

void ImportVSP()
{ FILE *f;
  unsigned short ver;
  char fname[60];
  int bsize;

  memset(fname, 0, 60);
  bmode=1; oh=th; th=0;
  Browse(1,"*.vsp","","","",fname);
  if (strlen(fname)<3) return;
  f=fopen(fname,"rb");
  if (!f)
     err("VSP file %s not found.",fname);
  WaitRelease(); th=oh; bmode=0;

  fread(&ver, 1, 2, f);
  if (ver!=2 && ver!=3)
     err("VSP file %s is not the correct VSP format.",fname);
  unsigned char pal[3*256];
  fread(pal, 1, 768, f);
  fread(&ontiles, 1, 2, f);
  if (ovsp!=0) {
	  for (int index=0; index<ontiles; index++)
		  destroy_bitmap(ovsp[index]);
	  delete[] ovsp;
  }
  ovsp=new BITMAP*[ontiles];//(char *) valloc(256*ontiles,"ovsp",0);
  for (int index=0; index<ontiles; index++) {
	  ovsp[index]=create_bitmap(16,16);
	  rectfill(ovsp[index],0,0,16,16,0);
  }

  if (ver==2) {
	  ovsp=new BITMAP*[ontiles];
	  for (int index=0; index<ontiles; index++)
		  ovsp[index]=create_bitmap(16,16);
	  unsigned char* data = new unsigned char[256*ontiles];
      fread(data, 256, ontiles, f);
	  for (int index=0; index<ontiles; index++)
		  for (int y=0; y<16; y++)
			  for (int x=0; x<16; x++)
				  putpixel(ovsp[index],x,y, data[256*index + y*16+x]);
	  delete[] data;
  }
  if (ver==3)
  {
    fread(&bsize, 1, 4, f);
	unsigned char* data = new unsigned char[256*ontiles];
    ReadCompressedLayer1(data, 256*ontiles, f);
	for (int index=0; index<ontiles; index++)
		for (int y=0; y<16; y++)
			for (int x=0; x<16; x++)
				putpixel(ovsp[index],x,y, data[256*index + y*16+x]);
	delete[] data;
  }

  fclose(f);

  ImportTiles();

  //vfree(ovsp);
  for (int index=0; index<ontiles; index++)
	  destroy_bitmap(ovsp[index]);
  delete[] ovsp; ovsp=0;
  key[KEY_ESC]=0;

}

int OPxsize=100, OPysize=40;
int OPxofs=100, OPyofs=70;

void OpenPCXas()
{ int done=0, moving=0;
  int mxo, myo, cursorblink=0;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  do
  {
    RenderSelector();
    RenderMessages();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      OPxofs = (mouse_x-mxo);
      OPyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (OPxofs<16) OPxofs = 16;
      if (OPyofs<16) OPyofs = 16;
      if (OPxofs+OPxsize>335) OPxofs = 336-OPxsize;
      if (OPyofs+OPysize>sy+16) OPyofs = (sy+16)-OPysize;
    }

    Window(OPxofs, OPyofs, OPxofs+OPxsize, OPyofs+OPysize, "Open PCX As");
    Button(OPxofs+21, OPyofs+18, "Tileset");
    Button(OPxofs+50, OPyofs+18, "Image");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>OPxofs+21 && mouse_x<OPxofs+30+21 && mouse_y>OPyofs+18 && mouse_y<OPyofs+10+18 && !moving)
    {
      // insert code for button "Tileset" here
      ButtonPressed(OPxofs+21, OPyofs+18, "Tileset");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      openas=0; done=1;
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>OPxofs+50 && mouse_x<OPxofs+30+50 && mouse_y>OPyofs+18 && mouse_y<OPyofs+10+18 && !moving)
    {
      // insert code for button "Image" here
      ButtonPressed(OPxofs+50, OPyofs+18, "Image");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      openas=1; done=1;
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(OPxofs+OPxsize-9) && (mouse_x<OPxofs+OPxsize-2)
    && mouse_y>(OPyofs+1) && (mouse_y<OPyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>OPxofs && mouse_x<OPxofs+OPxsize && mouse_y>OPyofs && mouse_y<(OPyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-OPxofs;
      myo=mouse_y-OPyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
}

//extern char manufacturer;
//extern char version;
//extern char encoding;
//extern char bits_per_pixel;
//extern short xmin,ymin;
//extern short xmax,ymax;
//extern short hres;
//extern short vres;
//extern char palette[48];
//extern char reserved;
//extern char color_planes;
//extern short bytes_per_line;
//extern short palette_type;
//extern char filler[58];
//char *t;
//
//int width,depth;
//int bytes;
//int vidoffset;
//FILE *pcxf;
//
//void ReadPCXLine(unsigned char *dest)
//{ int i, n;
//  int c, run;
//
//  n=0;
//  do {
//    c=fgetc(pcxf) & 0xff;
//
//    if ((c & 0xc0)==0xc0)
//    { run=c & 0x3f;
//      c=fgetc(pcxf);
//      for (i=0; i<run; i++)
//        dest[vidoffset+n+i]=(unsigned char)c;
//      n+=run;
//    }
//    else {
//      dest[vidoffset+n]=(unsigned char)c;
//      n++;
//    }
//  } while (n<bytes);
//}
//
//void LoadPCXHeader(char *fname)
//{
//  int i;
//
//  pcxf=fopen(fname,"rb");
//  if (!pcxf) err("Could not open specified PCX file.");
//  fread(&manufacturer,1,1,pcxf);
//  fread(&version,1,1,pcxf);
//  fread(&encoding,1,1,pcxf);
//  fread(&bits_per_pixel,1,1,pcxf);
//  fread(&xmin,1,2,pcxf);
//  fread(&ymin,1,2,pcxf);
//  fread(&xmax,1,2,pcxf);
//  fread(&ymax,1,2,pcxf);
//  fread(&hres,1,2,pcxf);
//  fread(&vres,1,2,pcxf);
//  fread(palette,1,48,pcxf);
//  fread(&reserved,1,1,pcxf);
//  fread(&color_planes,1,1,pcxf);
//  fread(&bytes_per_line,1,2,pcxf);
//  fread(&palette_type,1,2,pcxf);
//  fread(filler,1,58,pcxf);
//  fseek(pcxf,-768L,SEEK_END);
//  fread(pal,1,768,pcxf);
//  fseek(pcxf,128L,SEEK_SET);
//  width=xmax-xmin+1;
//  depth=ymax-ymin+1;
//  bytes=bytes_per_line;
//
//  for (i=0; i<256; i++) {
//    pal[i].r=pal[i].r>>2;
//	pal[i].g=pal[i].g>>2;
//	pal[i].b=pal[i].b>>2;
//  }
//  set_intensity(63);
//}
//
//void LoadPCXHeaderNP(char *fname)
//{
//  pcxf=fopen(fname,"rb");
//  if (!pcxf) err("Could not open specified PCX file.");
//  fread(&manufacturer,1,1,pcxf);
//  fread(&version,1,1,pcxf);
//  fread(&encoding,1,1,pcxf);
//  fread(&bits_per_pixel,1,1,pcxf);
//  fread(&xmin,1,2,pcxf);
//  fread(&ymin,1,2,pcxf);
//  fread(&xmax,1,2,pcxf);
//  fread(&ymax,1,2,pcxf);
//  fread(&hres,1,2,pcxf);
//  fread(&vres,1,2,pcxf);
//  fread(palette,1,48,pcxf);
//  fread(&reserved,1,1,pcxf);
//  fread(&color_planes,1,1,pcxf);
//  fread(&bytes_per_line,1,2,pcxf);
//  fread(&palette_type,1,2,pcxf);
//  fread(filler,1,58,pcxf);
//  width=xmax-xmin+1;
//  depth=ymax-ymin+1;
//  bytes=bytes_per_line;
//}


// grabs a tile from the ovsp buffer (transit tile bay)
int GrabTileAt(PCX* pcx, int grab_x, int grab_y, int z, int check_dup) {

  BITMAP* cutout=create_bitmap(16,16);
  for (int y=0; y<16; y++) {
	  for (int x=0; x<16; x++) {
		  putpixel(cutout,x,y,pcx->data[(grab_y+y)*pcx->width+(grab_x+x)]);
	  }
  }

  if (check_dup) {
	  for (int index=0; index<z; ++index) {
		  if (TileCmp(cutout,ovsp[index])) {
			  return 0;
		  }
	  }
  }

  if (ovsp[z]) {
	  destroy_bitmap(ovsp[z]);
  }
  ovsp[z]=cutout;//memcpy(ovsp+(z*256), tvsp, 256);
  return 1;
}

void ImportPCX()
{ char fname[60];
  int i,j,z;
  int wide,deep; // grabbable x&y tiles
  int add,jumper; // related to grab coords

  bmode=1; oh=th; th=0;
  Browse(1,"*.pcx","","","",fname);
  WaitRelease();
  if (strlen(fname)<3) return;
  OpenPCXas();
  th=oh; bmode=0;

  add=!openas;
  jumper=16+add;

  PCX* pcx=new PCX();
  pcx->load(fname);

  deep=((pcx->height&~15)+add)/jumper;
  wide=(pcx->width+add)/jumper;

  ontiles=wide*deep;
  ovsp=new BITMAP*[ontiles];//(char *) valloc(ontiles*256,"ovsp",0); // temp tilebay
  for (int index=0; index<ontiles; index++) {
	  ovsp[index]=create_bitmap(16,16);
	  rectfill(ovsp[index],0,0,16,16,0);
  }

  z=0;
  for (i=0; i<deep; i++) // grab all tiles
    for (j=0; j<wide; j++)
    {
      z+=GrabTileAt(pcx,(j*jumper)+add, (i*jumper)+add, z, openas);
    }

  ontiles = z; // tiles successfully grabbed
  //vfree(t);
  delete pcx;

  ImportTiles();
  //vfree(ovsp);
  for (int index=0; index<ontiles; index++)
	  destroy_bitmap(ovsp[index]);
  delete[] ovsp; ovsp=0;
  key[KEY_ESC]=0;
}
