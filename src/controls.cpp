// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                            MapEd v.2.01                             ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                           Control module                            ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


#include "maped.h"
#include "gui.h"
#include "minimap.h"
#include "vdriver.h"
#include "mouse.h"
#include "log.h"
#include "timer.h"
#include "config.h"
#include "render.h"
#include "tilesel.h"
#include "controls.h"
#include "ops.h"
#include "vcc.h"

#include <vector>

extern char scrollmode;

struct TileLocation {
	int x,y;
	int value;
	TileLocation(int x, int y, int value): x(x), y(y), value(value) {}
};

static std::vector<TileLocation*> tentative_undo;

namespace undo {

int UndoCount(void) {
	return tentative_undo.size();
}

};

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CHANGELOG:
// <zero 5.7.99>
// + fixed ScreenShot() to properly save screenshots with
//   the sequential filenames.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// ================================= Data ====================================

// ================================= Code ====================================

//#include "allegro.h"
//#include <stdio.h>
//#define unsigned char unsigned char

int CheckMouseTabs()
{ int i;

  for (i=0; i<10; i++)
  {
    if (i<6 && i>=numlayers) continue;
    if (mouse_b==1 && mouse_x>1+(i*10) && mouse_x<11+(i*10) && mouse_y>(sy-7) && mouse_y<sy)
    {   layertoggle[i]=(unsigned char)( layertoggle[i] ^ 1 ); WaitRelease(); return 1; }
    if (mouse_b==2 && mouse_x>1+(i*10) && mouse_x<11+(i*10) && mouse_y>(sy-7) && mouse_y<sy)
    {   el=(char)i; WaitRelease(); return 1; }
  }
  return 0;
}

void HAL()
{ int i;

  Message("I'm sorry dave, I can't do that...",600);
  i=systemtime+400;
  while ((int)systemtime<i)
  {
    RenderMap();
    RenderGUI();
    ShowPage();
  }
  Message("Just kidding! :)",200);
}

void OutputVSPpcx() {
	//Log("Outputting VSP as PCX...");
	PCX* pcx=new PCX();
	int w=18*17+1;
	int h=((numtiles+17)/18)*17+1;

	//Log("Dumping tiles...");
    BITMAP* bitmap = create_bitmap(w, h);
	rectfill(bitmap, 0, 0, w, h, 255);
	for (int index=0; index<numtiles; ++index) {
		int x=index%18;
		int y=index/18;
		blit(vsp[index],bitmap,0,0,(x*17)+1,(y*17)+1,16,16);
	}
	//Log("Capturing...");
	pcx->capture(bitmap);

	//Log("Creating filename...");
	char fname[1024];
	char* dot=strchr(vspname,'.');
	int dot_index=dot?(dot-vspname):0;
	strncpy(fname,vspname,dot_index);
	strncpy(fname+dot_index,".PCX",5);
	//Log("Filename is %s", fname);

	pcx->save(fname);
	//Log("PCX written!");
	delete pcx;
	//Log("PCX object deleted.");

	//Log("Destroying temp bitmap...");
	//destroy_bitmap(bitmap);
}

//extern void ScreenShot();

/*
void ScreenShot()
{
  unsigned char b1;
  unsigned short int w1;
  int i,n; //zero 5.7.99
  char fnamestr[13];
  static int ss=0;
  FILE *pcxf;

//--- zero 5.7.99
  n=0;
  do
  {
    sprintf(fnamestr,"%d.pcx",n);
    pcxf=fopen(fnamestr,"r");
    i=(int)pcxf;
    if(pcxf) fclose(pcxf);
    n++;
  } while(i);
  n--;

  // Takes a snapshot of the current screen.

   sprintf(fnamestr,"%d.pcx",n);
//---

   pcxf=fopen(fnamestr,"wb");
   ss++;

// Write PCX header

   b1=10; fwrite(&b1, 1, 1, pcxf); // manufacturer always = 10
   b1=5; fwrite(&b1, 1, 1, pcxf);  // version = 3.0, >16 colors
   b1=1; fwrite(&b1, 1, 1, pcxf);  // encoding always = 1
   b1=8; fwrite(&b1, 1, 1, pcxf);  // 8 bits per pixel, for 256 colors
   w1=0; fwrite(&w1, 1, 2, pcxf);  // xmin = 0;
   w1=0; fwrite(&w1, 1, 2, pcxf);  // ymin = 0;
 w1=(unsigned short)( sx-1 ); fwrite(&w1, 1, 2, pcxf);  // xmax = 319;
 w1=(unsigned short)( sy-1 ); fwrite(&w1, 1, 2, pcxf);  // ymax = 199;
 w1=(unsigned short)sx; fwrite(&w1, 1, 2, pcxf);  // hres = 320;
 w1=(unsigned short)sy; fwrite(&w1, 1, 2, pcxf);  // vres = 200;

 char junk[48];
 fwrite(junk,1,48,pcxf);

 b1=0; fwrite(&b1, 1, 1, pcxf);   // reserved always = 0.
 b1=1; fwrite(&b1, 1, 1, pcxf);   // number of color planes. Just 1 for 8bit.
 w1=(unsigned short)sx; fwrite(&w1, 1, 2, pcxf); // number of bytes per line

 w1=0; fwrite(&w1, 1, 1, pcxf);
 char junk2[59];
 fwrite(junk2, 1, 59, pcxf);          // filler

 // *** TODO ***
 //for (w1=0; w1<sy; w1++)
 //    WritePCXLine(screen+((w1+16)*tsx)+16, sx, pcxf);

 WritePalette(pcxf);
 fclose(pcxf);
 timer_count=0;
}
*/

void EntireMapShot()
{
	int save_sx, save_sy, save_tsx, save_tsy;
	int save_xwin, save_ywin;
	int save_tx, save_ty;

// save info
	save_sx = sx; save_sy = sy;
	save_tx=tx;
	save_ty=ty;
	save_tsx=tsx; // was typoed as save_tsy!!!
	save_tsy=tsy;
	save_xwin = xwin; save_ywin = ywin;

	int most_x=1;
	int most_y=1;
	for (int index=0; index<numlayers; index++) {
		if (layertoggle[index]==0) continue;
		if (layer[index].sizex>most_x) most_x=layer[index].sizex;
		if (layer[index].sizey>most_y) most_y=layer[index].sizex;
	}
// sneakily alter screen info so we can hand off to ScreenShot()
	tx = most_x;
	ty = most_y;
	sx = tx*16;
	sy = ty*16;
	tsx = sx;
	tsy = sy;
	xwin = ywin = 0;
	BITMAP* temp_dest=create_bitmap(tsx,tsy);//screen = (unsigned char*)malloc(tsx*tsy);
	rectfill(temp_dest,0,0,tsx,tsy,0);
	SetRenderDest(temp_dest);

	Log("Capturing entire map (%dx%d) at %dx%d pixels.",tx,ty,sx,sy);
	//Log("xwin=%d ywin=%d", xwin, ywin);
	//Log("width=%d height=%d", sx, sy);
	//Log("tx=%d ty=%d", tx, ty);

	RenderMap();

	//if (screen)
	ScreenShot();

// restore info
	sx = save_sx;
	sy = save_sy;
	tx = save_tx;
	ty = save_ty;
	tsx = save_tsx;
	tsy = save_tsy;
	xwin = save_xwin;
	ywin = save_ywin;
	//if (screen)
	//	free(screen);
	RestoreRenderDest();
	destroy_bitmap(temp_dest);
}

// aen -- put this out here because the PollMovement() routine i added
//        needs access to them.

int ms_right=0;
int ms_down=0;
int ms_left=0;
int ms_up=0;

bool nudging=false;

/**
 * <aen> Revised March 31, 2001.
 */
void PollMovement() {
	int xmax, ymax;

// d'oh! :p
	ms_right=
	ms_down	=
	ms_left	=
	ms_up	=0;

	// hack for nudging command
	if (key[KEY_G] || nudging) return;

	xmax = (layer[0].sizex * 16); if (xmax>sx) xmax-=sx;
	ymax = (layer[0].sizey * 16); if (ymax>sy) ymax-=sy;

	if (mouse_scroll) {
		if (mouse_y < map_scroll_y)		ms_up	=1;
		if (mouse_y > sy - map_scroll_y)ms_down	=1;
		if (mouse_x > sx - map_scroll_x)ms_right=1;
		if (mouse_x < map_scroll_x)		ms_left	=1;
	}
	if (key[KEY_UP])	ms_up	= 1;
	if (key[KEY_DOWN])	ms_down	= 1;
	if (key[KEY_LEFT])	ms_left	= 1;
	if (key[KEY_RIGHT])ms_right= 1;

	int tiles_to_scroll = tile_scroll_timer/16;
	tile_scroll_timer -= 16*tiles_to_scroll;
	int scroll_x = pixel_scroll_timer;
	int scroll_y = pixel_scroll_timer;
	pixel_scroll_timer = 0;
	if (!scrollmode) {
		scroll_x = 16*tiles_to_scroll;
		scroll_y = 16*tiles_to_scroll;
	}
	ywin += (ms_down - ms_up)	*scroll_x;
	xwin += (ms_right- ms_left)	*scroll_y;

	if (ywin < 0) ywin = 0;
	if (xwin < 0) xwin = 0;
	if (ywin > ymax) ywin = ymax;
	if (xwin > xmax) xwin = xmax;
}

void _setTile(int L, int X, int Y, int index) {
	if (L < 0 || L > 5) { return; }
	if (X < 0 || X >= layer[L].sizex || Y < 0 || Y >= layer[L].sizey) { return; }
	layers[L][layer[L].sizex*Y + X] = (unsigned short)index;
}

int _getTile(int L, int X, int Y) {
	if (L < 0 || L > 5) { return 0; }
	if (X < 0 || X >= layer[L].sizex || Y < 0 || Y >= layer[L].sizey) { return 0; }
	return layers[L][layer[L].sizex*Y + X];
}

void _setTileArea(int L, int X, int Y, int X2, int Y2, int index) {
	if (L < 0 || L > 5) {
		return;
	}
	int temp = 0;
	if (X > X2) {
		temp = X;
		X = X2;
		X2 = temp;
	}
	if (Y > Y2) {
		temp = Y;
		Y = Y2;
		Y2 = temp;
	}
	int width = X2 - X + 1;
	int height= Y2 - Y + 1;
	if (width < 1 || height < 1) {
		return;
	}
	for (int yloop = 0; yloop < height; yloop++) {
		for (int xloop = 0; xloop < width; xloop++) {
			_setTile(L, X + xloop, Y + yloop, index);
		}
	}
}

// <aen, june 5>
// + helpers for filling/copy routines. performs clipping

void set_layer(int l, int x, int y, int v)
  {
    if (x<0||x>=layer[l].sizex||y<0||y>=layer[l].sizey)
      return;

    layers[l][(y*layer[l].sizex)+x]=(unsigned short)v;
  }

int get_layer(int l, int x, int y)
  {
    if (x<0||x>=layer[l].sizex||y<0||y>=layer[l].sizey)
      return 0;

    return layers[l][(y*layer[l].sizex)+x];
  }

/////////////////////////////////////
// nb: this implementation assumes the region to be filled is not bounded
// by the edge of the screen, but is bounded on all sides by pixels of
// the specified boundary color.  This exercise is left to the student...

static int tiling_wrap(int value, int limit) {
	if (limit < 1) {
		return 0;
	}
	value = value % limit;
	if (value < 0) {
		value += limit;
	}
	return value;
}

std::stack<FloodFill_Coord> FloodFill_coords;
static unsigned short* temp_layer=0;

// pixel setter and getter
void FloodFill_SetPixel(int layer, int x, int y, int value) {
	set_layer(layer,x,y,value);
}
int FloodFill_GetPixel(int l,int x, int y) {
    if (x<0||x>=layer[l].sizex||y<0||y>=layer[l].sizey)
      return -1;

    return layers[l][(y*layer[l].sizex)+x];
}

// this algorithm uses a global stack of pixel coordinates
void FloodFill_pushSeed(int x, int y) {
	FloodFill_Coord c;
	c.x=x;
	c.y=y;
	FloodFill_coords.push(c);
}
// returns false iff stack was empty
bool FloodFill_popSeed(int* x, int* y) {
	if (!FloodFill_coords.empty()) {
		FloodFill_Coord c=FloodFill_coords.top();
		FloodFill_coords.pop();
		*x=c.x;
		*y=c.y;
		return true;
	} else {
		return false;
	}
}

int FloodFill_value=-1;
// the set it up and kick it off routine.
void FloodFill(int layer, int x, int y, int fill)
{
	FloodFill_value=FloodFill_GetPixel(layer,x,y);
	// we assume the stack is created empty, and that no other
	// routines or threads are using this stack for anything
	FloodFill_pushSeed(x, y);
	FloodFill_FillSeedsOnStack(layer, fill);
}

void FloodFillPaste(int lay, int x, int y) {
	// fill with some unlikely value
	FloodFill(lay,x,y,-2);

	int x_offset=(x%copybuf_wide);
	int y_offset=(y%copybuf_deep);
	// now just spread the past buffer over the unlikely values
	for (y=0; y<layer[lay].sizey; ++y) {
		for (x=0; x<layer[lay].sizex; ++x) {
			if ((signed short)get_layer(lay,x,y)==-2) {
				int wrapped_x=tiling_wrap(x-x_offset,copybuf_wide);
				int wrapped_y=tiling_wrap(y-y_offset,copybuf_deep);
				int wrapped_offset=(wrapped_y*copybuf_wide)+wrapped_x;
				set_layer(lay,x,y,copybuf[wrapped_offset]);
			}
		}
	}
}

// the main routine
void FloodFill_FillSeedsOnStack(int layer, int fill)
{
   int col1, col2;
   int x, y;              // current seed pixel
   int xLeft, xRight;     // current span boundary locations
   int i;

   while (FloodFill_popSeed(&x, &y)) {
      if (FloodFill_GetPixel(layer,x, y) == FloodFill_value) {
         FloodFill_FillContiguousSpan(layer,x, y, fill, &xLeft, &xRight);

         // single pixel spans handled as a special case in the else clause
         if (xLeft != xRight) {
            // handle the row above you
            y++;
            for(i=xLeft+1; i<=xRight; i++) {
               col1 = FloodFill_GetPixel(layer,i-1, y);
               col2 = FloodFill_GetPixel(layer,i, y);
               if (col1 == FloodFill_value && col1 != fill && col2 != FloodFill_value)
                  FloodFill_pushSeed(i-1, y);
            }
            if (col2 == FloodFill_value && col2 != fill)
               FloodFill_pushSeed(xRight, y);

            // handle the row below you
            y -= 2;
            for(i=xLeft+1; i<=xRight; i++) {
               col1 = FloodFill_GetPixel(layer,i-1, y);
               col2 = FloodFill_GetPixel(layer,i, y);
               if (col1 == FloodFill_value && col1 != fill && col2 != FloodFill_value)
                  FloodFill_pushSeed(i-1, y);
            }
            if (col2 == FloodFill_value && col2 != fill)
               FloodFill_pushSeed(xRight, y);
         } else {
            col1 = FloodFill_GetPixel(layer,xLeft, y+1);
            col2 = FloodFill_GetPixel(layer,xLeft, y-1);
            if (col1 != fill)
               FloodFill_pushSeed(xLeft, y+1);
            if (col2 != fill)
               FloodFill_pushSeed(xLeft, y-1);
         }

      } // end if (GetPixel)
	  //else break;
   }  // end while (popSeed)
}


// fill pixels to the left and right of the seed pixel until you hit
// boundary pixels.  Return the locations of the leftmost and rightmost
// filled pixels.
void FloodFill_FillContiguousSpan(int lay,int x, int y, int fill, int *xLeft, int *xRight)
{
   int col;
   int i;

   // fill pixels to the right until you reach a boundary pixel
   i = x;
   col = FloodFill_GetPixel(lay,i, y);
   while(col == FloodFill_value) {
      FloodFill_SetPixel(lay,i, y, fill);
      i++;
      col = FloodFill_GetPixel(lay,i, y);
   }
   *xRight = i-1;

   // fill pixels to the left until you reach a boundary pixel
   i = x-1;
   col = FloodFill_GetPixel(lay,i, y);
   while(col == FloodFill_value) {
      FloodFill_SetPixel(lay,i, y, fill);
      i--;
      col = FloodFill_GetPixel(lay,i, y);
   }
   *xLeft = i+1;
}

void NudgeLayer(int layer_index, int nudge_x, int nudge_y) {
	layer_r* lay=&layer[layer_index];
	int sizex=lay->sizex;
	int sizey=lay->sizey;

	if (nudge_x==-1) {
		unsigned short* left_strip=new unsigned short[sizey];
		for (int y=0; y<sizey; y++) {
			left_strip[y]=layers[layer_index][y*sizex];
		}
		for (int y=0; y<sizey; y++) {
			for (int x=1; x<sizex; x++) {
				int index=y*sizex+x;
				layers[layer_index][index-1]=layers[layer_index][index];
			}
		}
		for (int y=0; y<sizey; y++) {
			layers[layer_index][y*sizex+(sizex-1)]=left_strip[y];
		}
		delete[] left_strip;
	} else if (nudge_x==+1) {
		unsigned short* right_strip=new unsigned short[sizey];
		for (int y=0; y<sizey; y++) {
			right_strip[y]=layers[layer_index][y*sizex+(sizex-1)];
		}
		for (int y=0; y<sizey; y++) {
			for (int x=sizex-1; x>=1; x--) {
				int index=y*sizex+x;
				layers[layer_index][index]=layers[layer_index][index-1];
			}
		}
		for (int y=0; y<sizey; y++) {
			layers[layer_index][y*sizex]=right_strip[y];
		}
		delete[] right_strip;
	}

	if (nudge_y==-1) {
		unsigned short* top_strip=new unsigned short[sizex];
		memcpy(top_strip,&layers[layer_index][0],sizex*2);
		for (int y=1; y<sizey; y++) {
			memcpy(&layers[layer_index][(y-1)*sizex],&layers[layer_index][y*sizex],sizex*2);
		}
		memcpy(&layers[layer_index][(sizey-1)*sizex],top_strip,sizex*2);
		delete[] top_strip;
	} else if (nudge_y==+1) {
		unsigned short* bottom_strip=new unsigned short[sizex];
		memcpy(bottom_strip,&layers[layer_index][(sizey-1)*sizex],sizex*2);
		for (int y=sizey-1; y>=1; y--) {
			memcpy(&layers[layer_index][y*sizex],&layers[layer_index][(y-1)*sizex],sizex*2);
		}
		memcpy(&layers[layer_index][0],bottom_strip,sizex*2);
		delete[] bottom_strip;
	}
}
void NudgeObsZone(unsigned char* data, int nudge_x, int nudge_y) {
	int sizex=layer[0].sizex;
	int sizey=layer[0].sizey;

	if (nudge_x==-1) {
		unsigned char* left_strip=new unsigned char[sizey];
		for (int y=0; y<sizey; y++) {
			left_strip[y]=data[y*sizex];
		}
		for (int y=0; y<sizey; y++) {
			for (int x=1; x<sizex; x++) {
				int index=y*sizex+x;
				data[index-1]=data[index];
			}
		}
		for (int y=0; y<sizey; y++) {
			data[y*sizex+(sizex-1)]=left_strip[y];
		}
		delete[] left_strip;
	} else if (nudge_x==+1) {
		unsigned char* right_strip=new unsigned char[sizey];
		for (int y=0; y<sizey; y++) {
			right_strip[y]=data[y*sizex+(sizex-1)];
		}
		for (int y=0; y<sizey; y++) {
			for (int x=sizex-1; x>=1; x--) {
				int index=y*sizex+x;
				data[index]=data[index-1];
			}
		}
		for (int y=0; y<sizey; y++) {
			data[y*sizex]=right_strip[y];
		}
		delete[] right_strip;
	}

	if (nudge_y==-1) {
		unsigned char* top_strip=new unsigned char[sizex];
		memcpy(top_strip,data,sizex);
		for (int y=1; y<sizey; y++) {
			memcpy(data+(y-1)*sizex,data+y*sizex,sizex);
		}
		memcpy(data+(sizey-1)*sizex,top_strip,sizex);
		delete[] top_strip;
	} else if (nudge_y==+1) {
		unsigned char* bottom_strip=new unsigned char[sizex];
		memcpy(bottom_strip,data+(sizey-1)*sizex,sizex);
		for (int y=sizey-1; y>=1; y--) {
			memcpy(data+y*sizex,data+(y-1)*sizex,sizex);
		}
		memcpy(data,bottom_strip,sizex);
		delete[] bottom_strip;
	}
}

void NudgeMap(int nudge_x, int nudge_y) {
	if (nudge_x==0 && nudge_y==0) {
		return;
	}
	for (int index=0; index<numlayers; index++) {
		NudgeLayer(index,nudge_x,nudge_y);
	}
	NudgeObsZone((unsigned char*)obstruct,nudge_x,nudge_y);
	NudgeObsZone((unsigned char*)zone,nudge_x,nudge_y);
}

static int last_mouse_b = 0;
static int last_tile_x = -1;
static int last_tile_y = -1;

static int _parallax_x(int layer_index, int x) { return x*layer[layer_index].pmultx/layer[layer_index].pdivx; }
static int _parallax_y(int layer_index, int y) { return y*layer[layer_index].pmulty/layer[layer_index].pdivy; }

static void clear_tentative_undo() {
	// necessary?
	for (int index=0; index<(int)tentative_undo.size(); ++index) {
		delete tentative_undo[index];
	}

	tentative_undo.clear();
}

//		lt=layers[el][((((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16) *
//             layer[el].sizex)+(((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16)];
static void undoable_tile_plot(int layer_index, int pixel_x, int pixel_y, int value) {
	unsigned short* data=layers[layer_index];
	int x=(_parallax_x(layer_index,pixel_x)+mouse_x)/16;
	int y=(_parallax_y(layer_index,pixel_y)+mouse_y)/16;
	data[(layer->sizex*y)+x]=value;
	tentative_undo.push_back(new TileLocation(x,y,value));
}

static void ProcessControlsInner() {
  int xmax, ymax;

  xmax = (layer[0].sizex * 16) - sx;
  ymax = (layer[0].sizey * 16) - sy;

	if (!key[KEY_G]) {
		nudging=false;
	}
    if (key[KEY_G]) {
		nudging=true;
		int nudge_x=0;
		int nudge_y=0;
		if (key[KEY_RIGHT]) nudge_x++;
		if (key[KEY_LEFT]) nudge_x--;
		if (key[KEY_DOWN]) nudge_y++;
		if (key[KEY_UP]) nudge_y--;
		NudgeMap(nudge_x,nudge_y);
		//key[KEY_G]=0;
		key[KEY_UP]=0;
		key[KEY_DOWN]=0;
		key[KEY_LEFT]=0;
		key[KEY_RIGHT]=0;
		return;
	}

  if (key[KEY_F5]) { key[KEY_F5]=0; ShellMAP(); }
  if (key[KEY_F6]) { key[KEY_F6]=0; CompileAll(); }
  if (key[KEY_F8]) { key[KEY_F8]=0; ShellVERGE(); }
  //if ((key_shifts & KB_ALT_FLAG) && key[KEY_D]) { key[KEY_D]=0; ShellToDOS(); }
  if ((key_shifts & KB_ALT_FLAG) && key[KEY_L]) LoadMAPDialog();
  if ((key_shifts & KB_ALT_FLAG) && key[KEY_V]) ShellEditMAP();
  if ((key_shifts & KB_ALT_FLAG) && key[KEY_S]) ShellEditSystem();
  if ((key_shifts & KB_ALT_FLAG) && key[KEY_N]) NewMAP();
  if ((key_shifts & KB_ALT_FLAG) && key[KEY_P]) MPDialog();
  if (!(key_shifts & KB_ALT_FLAG) && key[KEY_P]) { pasting^=1; key[KEY_P]=0; } // aen

  if (key[KEY_A] && el<6)
  {
    lt++;
    if (lt==numtiles) lt=0;
    key[KEY_A]=0;
  }
  if (key[KEY_Z] && el<6)
  {
    if (lt) lt--;
    else lt=(short)( numtiles-1 );
    key[KEY_Z]=0;
  }
  if (key[KEY_A] && el==7)
  {
    curzone++;
    key[KEY_A]=0;
  }
  if (key[KEY_Z] && el==7)
  {
    curzone--;
    key[KEY_Z]=0;
  }
  if (key[KEY_S])
  {
    rt++;
    if (rt==numtiles) rt=0;
    key[KEY_S]=0;
  }
  if (key[KEY_X])
  {
    if (rt) rt--;
    else rt=(short)( numtiles-1 );
    key[KEY_X]=0;
  }
  if (key[KEY_F10])
  {
    key[KEY_F10]=0;
    if (rnd(0,1500)<1) HAL();
    SaveMAP(mapname);
    SaveVSP(vspname);
    CompileMap_forMaped(mapname);
    //CompileMAP();
    Message("MAP/VSP saved.",100);
    modified=0;
  }
  if (key[KEY_C])
  {
    key[KEY_C]=0;
    sprintf(strbuf,"Left: %d Right: %d", lt, rt);
    Message(strbuf, 300);
  }
  if (key[KEY_M])
  {
    key[KEY_M]=0;
    GenerateMiniVSP();
    MiniMAP();
  }
  if (key[KEY_H])
  {
    key[KEY_H]=0;
    mh=(char)( mh^1 );
    if (mh) Message("MAP Tile Highlight enabled.",100);
       else Message("MAP Tile Highlight disabled.",100);
  }

  // ***
  // movement code moved to PollMovement()
  // ***

  if (key[KEY_PGUP])
  {
    key[KEY_PGUP]=0;
    ywin -= sy;
    if (ywin < 0)
      ywin = 0;
  }
  if (key[KEY_HOME])
  {
    key[KEY_HOME]=0;
    xwin -= sx;
    if (xwin < 0)
      xwin = 0;
  }
  if (key[KEY_END])
  {
    key[KEY_END]=0;
    xwin += sx;
    if (xwin > xmax)
      xwin = xmax;
  }
  if (key[KEY_PGDN])
  {
    key[KEY_PGDN]=0;
    ywin += sy;
    if (ywin > ymax)
      ywin = ymax;
  }

  if ((key_shifts & KB_CTRL_FLAG) && el<6)
  {
    //key[SCAN_CTRL]=0;
	key[KEY_LCONTROL] = 0;
	key[KEY_RCONTROL] = 0;
    TileSelector();
  }
  if ((key_shifts & KB_CTRL_FLAG) && el==7)
  {
    //key[SCAN_CTRL]=0;
    key[KEY_LCONTROL] = 0;
	key[KEY_RCONTROL] = 0;
    ZoneEdDialog();
  }

  if (key[KEY_TAB])
  {
    key[KEY_TAB]=0;
    if (scrollmode)
    {
      scrollmode=0;
      xwin=xwin/16; xwin=xwin*16;
      ywin=ywin/16; ywin=ywin*16;
      Message("Tile scroll.",150);
    }
    else
    {
      scrollmode=1;
      Message("Pixel scroll.",150);
    }
  }

  if (key[KEY_1])
  {
     if ((key_shifts & KB_SHIFT_FLAG))
     {
       layertoggle[0]=0;
       return;
     }
     layertoggle[0]=1;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     el=0;
     key[KEY_1]=0;
  }
  if (key[KEY_2])
  {
     if ((key_shifts & KB_SHIFT_FLAG))
     {
       layertoggle[1]=0;
       return;
     }
     key[KEY_2]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>1)
     { layertoggle[1]=1;
       el=1; }
  }
  if (key[KEY_3])
  {
     if ((key_shifts & KB_SHIFT_FLAG))
     {
       layertoggle[2]=0;
       return;
     }
     key[KEY_3]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>2)
     { layertoggle[2]=1;
       el=2; }
  }
  if (key[KEY_4])
  {
     if ((key_shifts & KB_SHIFT_FLAG))
     {
       layertoggle[3]=0;
       return;
     }
     key[KEY_4]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>3)
     { layertoggle[3]=1;
       el=3; }
  }
  if (key[KEY_5])
  {
     if ((key_shifts & KB_SHIFT_FLAG))
     {
       layertoggle[4]=0;
       return;
     }
     key[KEY_5]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>4)
     { layertoggle[4]=1;
       el=4; }
  }
  if (key[KEY_6])
  {
     if ((key_shifts & KB_SHIFT_FLAG))
     {
       layertoggle[5]=0;
       return;
     }
     key[KEY_6]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>5)
     { layertoggle[5]=1;
       el=5; }
  }
  if (key[KEY_O])
  {
     key[KEY_O]=0;
     layertoggle[6]=1; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     el=6;
  }
  if (key[KEY_N])
  {
     key[KEY_N]=0;
     layertoggle[6]=0; layertoggle[7]=1;
     layertoggle[8]=0; layertoggle[9]=0;
     el=7;
  }
  if (key[KEY_E])
  {
     key[KEY_E]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=1; layertoggle[9]=0;
     el=8;
  }
  if (key[KEY_T])
  {
     key[KEY_T]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=1;
     el=9;
  }
  if (key[KEY_F9])
  { // aen
    key[KEY_F9]=0;
    mouse_scroll^=1;
    sprintf(strbuf,"Mouse scroll %sabled.", mouse_scroll ? "en" : "dis");
    Message(strbuf, 100);
  }
  if (key[KEY_F11])
  {
    key[KEY_F11]=0;
    ScreenShot();
    Message("Screen capture saved.",300);
  }
  if (key[KEY_F7])
  {
	  key[KEY_F7]=0;
	  EntireMapShot();
	  Message("Screen capture of entire map saved.",300);
  }
  if (key[KEY_F12])
  {
    key[KEY_F12]=0;
    OutputVSPpcx();
    Message("PCX file exported.",300);
  }
  if (key[KEY_OPENBRACE])
  {
      key[KEY_OPENBRACE]=0;
      ShutdownVideo();
      vm=0;
      InitVideo(320,240,120,(key_shifts & KB_ALT_FLAG)!=0);
      set_intensity(63);
      InitMouse();

      // aen -- gotta recalc this if you're in a lower res at the bottom of
      //        the map and jump to a higher res.
      xmax = (layer[0].sizex * 16) - sx;
      ymax = (layer[0].sizey * 16) - sy;
      if (xwin > xmax) xwin=xmax;
      if (ywin > ymax) ywin=ymax;
  }
  if (key[KEY_CLOSEBRACE])
  {
      key[KEY_CLOSEBRACE]=0;
      ShutdownVideo();
      vm=1;
      InitVideo(640,480,120,(key_shifts & KB_ALT_FLAG)!=0);
      set_intensity(63);
      InitMouse();

      // aen -- gotta recalc this if you're in a lower res at the bottom of
      //        the map and jump to a higher res.
      xmax = (layer[0].sizex * 16) - sx;
      ymax = (layer[0].sizey * 16) - sy;
      if (xwin > xmax) xwin=xmax;
      if (ywin > ymax) ywin=ymax;
  }

  if (CheckMouseTabs()) return;

  if (mouse_b>=3) newMainMenu(mouse_x-4,mouse_y-5);
  if (key[KEY_ESC]) DoMainMenu();

  if (mouse_b==1 && key[KEY_SLASH] && el<6 && layertoggle[el])
  {
		lt=layers[el][((((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16) *
             layer[el].sizex)+(((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16)];

		return;
  }

  if (mouse_b==2 && key[KEY_SLASH] && el<6 && layertoggle[el])
  {
		rt=layers[el][((((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16) *
             layer[el].sizex)+(((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16)];
		return;
  }

  // aen; these must come before the tile plotting code just below
  // to work correctly.
  if (mouse_b && el<6 && key[KEY_LSHIFT] && !shifted)
  {
    selx1=(((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16);
    sely1=(((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16);
    selx2=selx1;
    sely2=sely1;
    shifted=mouse_b;
    return;
  }
  if (mouse_b && el<6 && shifted)
  {
    selx2=(((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16);
    sely2=(((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16);
    return;
  }
  if (!mouse_b && el<6 && shifted)
  { int i,j;
    int x1,y1,x2,y2;

    x1=selx1;
    y1=sely1;
    x2=selx2;
    y2=sely2;

    if (x2<x1) x2^=x1,x1^=x2,x2^=x1;
    if (y2<y1) y2^=y1,y1^=y2,y2^=y1;

    copybuf_wide=x2-x1+1;
    copybuf_deep=y2-y1+1;

    if (shifted==2)
    {
      if (el<numlayers && layertoggle[el]) // safeguard
      {
        // block fill
        modified=1;
        for (j=0; j<copybuf_deep; j++)
        {
          for (i=0; i<copybuf_wide; i++)
            set_layer(el, x1+i,y1+j, lt);
            //layers[el][((y1+j)*layer[el].sizex)+(x1+i)]=lt;
        }
      }
    }

    if (shifted==1)
    {
      if (el<numlayers && layertoggle[el]) // safeguard
      {
        int n=0;

        modified=1;
        if (copybuf) vfree(copybuf);
        copybuf=(unsigned short *)valloc(copybuf_wide*copybuf_deep*2, "copybuf", 0);

        // copy
        for (j=0; j<copybuf_deep; j++)
        {
          for (i=0; i<copybuf_wide; i++,n++)
            copybuf[n]=(unsigned short) get_layer(el,x1+i,y1+j);
            //layers[el][((y1+j)*layer[el].sizex)+(x1+i)];
        }
        pasting=1;
      }
    }
    selx1=sely1=0;
    selx2=sely2=0;
    shifted=0;
  }
  if (mouse_b==1 && el<6 && !shifted && pasting && layertoggle[el])
  { int a,b,i,j;

    a=(((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16);
    b=(((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16);

	if (!key[KEY_F]) {
		// paste
		for (j=0; j<copybuf_deep; j++)
		{
			for (i=0; i<copybuf_wide; i++)
			{
				if (b+j<layer[el].sizey && a+i<layer[el].sizex)
				layers[el][((b+j)*layer[el].sizex)+(a+i)]=copybuf[(j*copybuf_wide)+i];
			}
		}
	} else {
		FloodFillPaste(el,a,b);
	}
  }

  if (mouse_b==1 && el<6 && !shifted && !pasting && layertoggle[el])
  {
	  if (last_mouse_b==0) {
		clear_tentative_undo();
	  }
    if (mouse_x>sx) mouse_x=sx;
	if (mouse_y>sy) mouse_y=sy;
    modified=1;
	int x=((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16;
	int y=((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16;
	if (!key[KEY_F]) {
	    if (last_tile_x!=x || last_tile_y!=y)
		undoable_tile_plot(el,xwin,ywin,lt);
    	//layers[el][(y*layer[el].sizex)+x]=lt;
	} else {
		FloodFill(el,x,y,lt);
	}
  }
  if (mouse_b==2 && el<6 && !shifted && layertoggle[el])
  {
    if (mouse_x>sx) mouse_x=sx;
	if (mouse_y>sy) mouse_y=sy;
    modified=1;
	int x=((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16;
	int y=((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16;
	if (!key[KEY_F]) {
	  if (last_mouse_b==0) {
		clear_tentative_undo();
	  }
	    if (last_tile_x!=x || last_tile_y!=y)
		undoable_tile_plot(el,xwin,ywin,rt);
    	//layers[el][(y*layer[el].sizex)+x]=rt;
	} else {
		FloodFill(el,x,y,rt);
	}
  }

  if (mouse_b==1 && el==6 && layertoggle[el])
  {
   modified=1;
   obstruct[((((ywin*layer[0].pmulty/layer[0].pdivy)+mouse_y)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mouse_x)
               /16)]=1;
  }
  if (mouse_b==2 && el==6 && layertoggle[el])
  {
   modified=1;
   obstruct[((((ywin*layer[0].pmulty/layer[0].pdivy)+mouse_y)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mouse_x)
               /16)]=0;
  }
  if (mouse_b==1 && el==7 && (key_shifts & KB_SHIFT_FLAG) && layertoggle[el])
  {
   curzone=zone[((((ywin*layer[0].pmulty/layer[0].pdivy)+mouse_y)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mouse_x)
               /16)];
   WaitRelease();
   ZoneEdDialog();
   return;
  }
  if (mouse_b==1 && el==7 && layertoggle[el])
  {
   modified=1;
   zone[((((ywin*layer[0].pmulty/layer[0].pdivy)+mouse_y)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mouse_x)
               /16)]=curzone;
  }
  if (mouse_b==2 && el==7 && layertoggle[el])
  {
   modified=1;
   zone[((((ywin*layer[0].pmulty/layer[0].pdivy)+mouse_y)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mouse_x)
               /16)]=0;
  }
  if (mouse_b==1 && el==8 && layertoggle[el])
  {
     WaitRelease();
     ProcessEntity((xwin+(mouse_x))/16,(ywin+(mouse_y))/16);
     modified=1;
  }
  if (el==8 && EntityThere((mouse_x+xwin)/16,(mouse_y+ywin)/16) &&
      key[KEY_DEL] && layertoggle[el])
  {
    WaitRelease();
    DeleteEntity((mouse_x+xwin)/16,(mouse_y+ywin)/16);
    modified=1;
  }
}

void ProcessControls()
{
	int x=-1;
	int y=-1;
	if (el<6 && mouse_b!=0) {
		x=((xwin*layer[el].pmultx/layer[el].pdivx)+mouse_x)/16;
		y=((ywin*layer[el].pmulty/layer[el].pdivy)+mouse_y)/16;
	}

	ProcessControlsInner();
	last_mouse_b = mouse_b;
	last_tile_x=x;
	last_tile_y=y;
}
