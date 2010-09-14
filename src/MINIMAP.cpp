// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                            MapEd v.2.01                             ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                     Mini-MAP (zoom out) module                      ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "config.h"
#include "keyboard.h"
#include "maped.h"
#include "mouse.h"
#include "vdriver.h"
#include "log.h"
// ============================ Data ============================

unsigned char mvsp[10000];         // mini-VSP dominant color
unsigned char solid_count[10000];
int xoff=0, yoff=0;               // x-offset, y-offset

void FindDominantColor(int tile_index) {
	BITMAP* src=vsp[tile_index];

	int r=0;
	int g=0;
	int b=0;

	GetPalette();
	solid_count[tile_index]=0;
	for (int y=0; y<16; ++y) {
		for (int x=0; x<16; ++x) {
			int color=getpixel(src,x,y);
			if (color!=0) {
				++solid_count[tile_index];
			}
			r+=pal[color].r;
			g+=pal[color].g;
			b+=pal[color].b;
		}
	}

	if (tile_index<0) {
		Log("tile_index<0: %d", tile_index);
	} else if (tile_index>=numtiles) {
		Log("tile_index>=%d: %d", numtiles, tile_index);
	} else {
		mvsp[tile_index]=(unsigned char)makecol(r/64,g/64,b/64);
		if (mvsp[tile_index]==0) {
			mvsp[tile_index]=makecol(0,0,0);
		}
	}
}

void GenerateMiniVSP() {
	//Log("generating minivsp: ptr=%d,numtiles=%d", mvsp,numtiles);
	//if (!mvsp) {
	//	Log("!mvsp!!!");
	//	return;
	//}
	for (int index=0; index<numtiles; index++) {
		FindDominantColor(index);
	}
}

void MiniMAP() {
	//mvsp=new unsigned char[numtiles];
	//if (!mvsp) {
	//	Log("!mvsp");
	//	return;
	//}
	GenerateMiniVSP();

	BITMAP* minimap=create_bitmap(layer[0].sizex,layer[0].sizey);
	rectfill(minimap,0,0,minimap->w,minimap->h,0);

	// Now we "draw" the tiles into the buffer.
  for (char* src=rstring; *src; src++) {
	  int layer_index=0;
	  switch (*src) {
		  case '1': case '2': case '3': case '4': case '5': case '6':
			  layer_index=(int)*src-(int)'1';
			  break;
		  default: continue;
	  }

		if (!layertoggle[layer_index]) {
			continue;
		}
		//Log("layer[%d].sizex=%d",layer_index,layer[layer_index].sizex);
		//Log("layer[%d].sizey=%d",layer_index,layer[layer_index].sizey);
		if (!layers[layer_index]) {
			Log("no layer data at index %d", layer_index);
			continue;
		}
		for (int y=0; y<layer[layer_index].sizey; y++) {
			for (int x=0; x<layer[layer_index].sizex; x++) {
				int offset=y*layer[layer_index].sizex+x;
				if (offset<0) {
					Log("offset<0: %d", offset);
				} else if (offset>=layer[layer_index].sizex*layer[layer_index].sizey) {
					Log("offset>=%d: %d", layer[layer_index].sizex*layer[layer_index].sizey, offset);
				} else {
					int index=layers[layer_index][offset];
					if (index<0) {
						Log("index<0: %d", index);
					} else if (index>=numtiles) {
						Log("index>=%d: %d", numtiles, index);
					} else {
						if (index) {
							unsigned char c=(unsigned char)mvsp[index];
							putpixel(minimap,x,y,c);
						}
					}
				}
			}
		}
	}

	int tile_w=(tsx+15)/16;
	int tile_h=(tsy+16)/16;
	int gray=makecol(128,128,128);

	while (keypressed())
		readkey();

	bool done = false;
	while (!done) {
		rectfill(get_offscreen(),0,0,sx,sy,gray);
		CopySprite(-xoff,-yoff,layer[0].sizex,layer[0].sizey,minimap);
		HLine(mouse_x,mouse_y,tile_w,white);
		VLine(mouse_x,mouse_y,tile_h,white);
		VLine(mouse_x+tile_w,mouse_y,tile_h+1,white);
		HLine(mouse_x,mouse_y+tile_h,tile_w,white);
		ShowPage();

		if (keypressed()) {
			int scan=readkey()>>8;
			switch (scan) {
				case KEY_ESC: done=true; break;
				case KEY_LEFT: xoff-=16; break;
				case KEY_UP: yoff-=16; break;
				case KEY_DOWN: yoff+=16; break;
				case KEY_RIGHT: xoff+=16; break;
			}
			if (xoff<0) xoff=0;
			else if (xoff>layer[0].sizex-tile_w) {
				xoff=layer[0].sizex-tile_w;
			}
			if (yoff<0) yoff=0;
			else if (yoff>layer[0].sizey-tile_h) {
				yoff=layer[0].sizey-tile_h;
			}
		}

		ReadMouse();
		if (mouse_b) {
			xwin=(xoff+mouse_x)*16;
			ywin=(yoff+mouse_y)*16;
			if (xwin >= (layer[0].sizex*16)-sx) {
				xwin=(layer[0].sizex*16)-sx;
			}
			if (ywin >= (layer[0].sizey*16)-sy) {
				ywin=(layer[0].sizey*16)-sy;
			}
			WaitRelease();
			done=true;
		}
	}

	while (keypressed())
		readkey();

	destroy_bitmap(minimap);
	//delete[] mvsp;
}