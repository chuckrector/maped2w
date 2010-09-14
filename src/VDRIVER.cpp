// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                   The VERGE Engine version 2.01                     ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                    Video Driver Manager module                      ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#include <conio.h>

#include "allegro.h"
#include "maped.h"
#include "error.h"
#include "log.h"

// ================================= Data ====================================

// Driver function-pointers.
int (*ShutdownVideo)();
int (*ShowPage)();
int (*CopySprite)(int x, int y, int width, int height, BITMAP *src);
int (*CCopySprite)(int x, int y, int width, int height, BITMAP* src);
int (*TCCopySprite)(int x, int y, int width, int height, BITMAP* src);
int (*TCopySprite)(int x, int y, int width, int height, BITMAP* src);
int (*CopyTile)(int x, int y, BITMAP* src);
int (*FilledBox)(int x, int y, int width, int height, int c);
int (*VLine)(int x, int y, int height, int color);
int (*HLine)(int x, int y, int width, int color);
int (*ColorGrid)(int x, int y, int c);
int (*ColorGridSize)(int x, int y, int w, int h, int c);
int (*ClearScreen)(void);
int (*TCopySpriteLucent)(int x, int y, int width, int height, BITMAP* src);

PALETTE pal;
PALETTE pal2;

int sx=0;
int sy=0;
int tx=0;
int ty=0;
int tsx=0;
int tsy=0;
int map_scroll_x=1;
int map_scroll_y=1;

static BITMAP* offscreen[3];
static BITMAP* double_buffer;
static int page_count=0;
static int current_page=0;
static bool got_video=false;

// ================================= Code ====================================

static BITMAP* render_dest=0;

void SetRenderDest(BITMAP* dest) {
	render_dest=dest;
}

void RestoreRenderDest() {
	render_dest=0;
}

BITMAP* get_offscreen() {
//	if (current_page<0||current_page>=page_count)
//		current_page=0;
	if (render_dest!=0) {
		return render_dest;
	}
	return double_buffer;//offscreen[current_page%page_count];
}

int AllegroShutdown() {
	for (int index=0; index<page_count; index++)
		destroy_bitmap(offscreen[index]);
	destroy_bitmap(double_buffer);
	return 0;
}

int AllegroShowPage() {
	CheckTimerStuff();
	if (got_video) {
		BITMAP* dest = offscreen[current_page%page_count];
		acquire_bitmap(dest);
		stretch_blit(get_offscreen(), dest, 0, 0, sx, sy, 0, 0, SCREEN_W, SCREEN_H);
		release_bitmap(dest);
		show_video_bitmap(dest);
		current_page++;
	} else {
		acquire_bitmap(screen);
		stretch_blit(get_offscreen(), screen, 0, 0, sx, sy, 0, 0, SCREEN_W, SCREEN_H);
		release_bitmap(screen);
	}
	return 0;
}

int AllegroPutPixel(int x, int y, int color) {
	putpixel(get_offscreen(), x, y, color);
	return 0;
}

int AllegroGetPixel(int x, int y) {
	return getpixel(get_offscreen(), x, y);
}

int AllegroCopySprite(int x, int y, int width, int height, BITMAP *src) {
	stretch_blit(src, get_offscreen(), 0, 0, src->w, src->h, x, y, width, height);
	return 0;
}

int AllegroTCopySprite(int x, int y, int width, int height, BITMAP *src) {
	masked_stretch_blit(src, get_offscreen(), 0, 0, src->w, src->h, x, y, width, height);
	return 0;
}

int AllegroVLine(int x, int y, int length, int color) {
	vline(get_offscreen(), x, y, y + length - 1, color);
	return 0;
}

int AllegroHLine(int x, int y, int width, int color) {
	hline(get_offscreen(), x, y, x + width - 1, color);
	return 0;
}

int AllegroFilledBox(int x, int y, int width, int height, int color) {
	rectfill(get_offscreen(),x,y,x+width-1,y+height-1,color);
	return 0;
}

int AllegroColorGridSize(int x, int y, int w, int h, int color) {
	for (int yloop = 0; yloop < h; yloop++) {
		for (int xloop = 0; xloop < w; xloop++) {
			if ((xloop + yloop)&1) {
				AllegroPutPixel(x + xloop, y + yloop, color);
			}
		}
	}
	return 0;
}

int AllegroColorGrid(int x, int y, int color) {
	return AllegroColorGridSize(x, y, 16, 16, color);
}

int AllegroClearScreen() {
	AllegroFilledBox(0,0,sx,sy,0);
	return 0;
}

int AllegroTCopySpriteLucent(int x, int y, int width, int height, BITMAP *src) {
	if (color_map!=NULL) {
		draw_trans_sprite(get_offscreen(),src,x,y);
	} else {
		AllegroTCopySprite(x,y,width,height,src);
	}
	return 0;
}

void InitVideo(int xres, int yres, int refresh_rate, bool windowed)
{
	::windowed=windowed;
	if (refresh_rate<60) refresh_rate=60;
	request_refresh_rate(refresh_rate);
	set_color_depth(8);
	sx	= xres;
	sy	= yres;
	int type;
	type=GFX_AUTODETECT;
	if (windowed)
		type=GFX_AUTODETECT_WINDOWED;
	if (set_gfx_mode(type, xres, yres, 0, 0)<0) {
		if (set_gfx_mode(type, 320, 200, 0, 0)<0) {
			err("Couldn't set 320x200x8!");
		}
		sx = 320;
		sy = 200;
	}

	tsx	= sx;
	tsy	= sy;
	tx	= tsx/16;
	ty	= tsy/16;

	page_count=0;
	got_video=false;

	offscreen[0]=create_video_bitmap(sx,sy);
	if (offscreen[0]) {
		// try and get second page
		offscreen[1]=create_video_bitmap(sx,sy);
		if (offscreen[1]) {
			got_video=true;
			page_count=2;
			// try and get third page
			offscreen[2]=create_video_bitmap(sx,sy);
			if (offscreen[2])
				page_count++;
		} else {
			destroy_bitmap(offscreen[page_count]);
		}
	// just get a normal bitmap
	} 
	double_buffer=create_bitmap(sx,sy);
	if (!got_video) {
		page_count=1;
	}
	Log("pages: %d",page_count);
	Log("page-flipping? %s",got_video?"yes":"no");

	// Mode successfuly set, now lets set up the driver.
	ShutdownVideo		= AllegroShutdown;
	ShowPage			= AllegroShowPage;
	CopySprite			= AllegroCopySprite;
	TCopySprite			= AllegroTCopySprite;
	FilledBox			= AllegroFilledBox;
	VLine				= AllegroVLine;
	HLine				= AllegroHLine;
	ColorGrid			= AllegroColorGrid;
	ColorGridSize		= AllegroColorGridSize;
	ClearScreen			= AllegroClearScreen;
	TCopySpriteLucent	= AllegroTCopySpriteLucent;

	map_scroll_x = 2;
	map_scroll_y = 2;
}

void SetPalette(PALETTE pall) {
	for (int index=0; index<256; index++) {
		pal[index].r=pall[index].r;
		pal[index].g=pall[index].g;
		pal[index].b=pall[index].b;
	}
	set_palette(pall);
}

void GetPalette() {
	get_palette(pal);
}

void set_intensity(unsigned int n) {
	for (int index=0; index<256; index++) {
		pal2[index].r = (pal[index].r*n)>>6;
		pal2[index].g = (pal[index].g*n)>>6;
		pal2[index].b = (pal[index].b*n)>>6;
	}
	SetPalette(pal2);
}

int cx1=16;
int cy1=16;
int cx2=336;
int cy2=256;

char stipple[256]={
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
};

BITMAP* bmpStipple = 0;
BITMAP* get_stipple() {
	if (!bmpStipple) {
		bmpStipple = create_bitmap(16,16);
		for (int y=0; y<16; y++)
			for (int x=0; x<16; x++)
				putpixel(bmpStipple,x,y,stipple[y*16+x]);
	}
	return bmpStipple;
}
