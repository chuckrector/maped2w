// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                            MapEd v.2.01                             ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                         Map Render module                           ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "config.h"
#include "maped.h"
#include "vdriver.h"

#include "render.h" // MAXTILES
#include "compress.h"

// ================================= Data ====================================

extern char stipple[256];

int oxw=0, oyw=0;
int xofs=0, yofs=0;
int xtc=0, ytc=0;

char curlayer=0;
short tileidx[MAXTILES];
char flipped[MAXTILES];

// ------

chri chrs[100];

// ================================= Code ====================================

void DoCHR(int i) // hee hee!
{
  FILE *f;
  char b;
  int q;
  short numframes;

//  free(chrs[i].frame);
//  chrs[i].frame=0;
//  return;

  if (!strlen(chrlist[i].t))
  {
    if (chrs[i].frame) vfree(chrs[i].frame);
    chrs[i].frame=0;
    chrs[i].fx=chrs[i].fy=0;
    chrs[i].hx=chrs[i].hy=0;
    return;
  }
  f=fopen(chrlist[i].t,"rb");
  if (!f)
  {
    if (chrs[i].frame) vfree(chrs[i].frame);
    chrs[i].frame=0;
    chrs[i].fx=chrs[i].fy=0;
    chrs[i].hx=chrs[i].hy=0;
    return;
  }
  if (chrs[i].frame) vfree(chrs[i].frame);
  fread(&b, 1, 1, f);
  if (b!=2)
  {
    // This should really not work with V1 CHRs, but..
    chrs[i].frame=create_bitmap(16,32);//(char *) malloc(512);
    fseek(f, 0, 0);
	char buf[512];
    fread(buf/*chrs[i].frame*/, 1, 512, f);
	for (int y=0; y<32; y++)
		for (int x=0; x<16; x++)
			putpixel(chrs[i].frame,x,y,buf[y*16+x]);
    fclose(f);
    chrs[i].fx=16; chrs[i].fy=32;
    chrs[i].hx=0;  chrs[i].hy=16;
    return;
  }
  chrs[i].fx=0; fread(&chrs[i].fx, 1, 2, f);
  chrs[i].fy=0; fread(&chrs[i].fy, 1, 2, f);
  chrs[i].hx=0; fread(&chrs[i].hx, 1, 2, f);
  chrs[i].hy=0; fread(&chrs[i].hy, 1, 2, f);
  fread(strbuf, 2, 2, f);
  fread(&numframes, 1, 2, f);
  chrs[i].frame=create_bitmap(chrs[i].fx,chrs[i].fy*numframes);//(char *) valloc(chrs[i].fx*chrs[i].fy*numframes,"chr preview",i);
  fread(&q, 1, 4, f);

  unsigned char* data = new unsigned char[q];
  ReadCompressedLayer1(data/*(unsigned char*)chrs[i].frame*/, chrs[i].fx*chrs[i].fy*numframes,f);
  for (int y=0; y<chrs[i].fy*numframes; y++)
	  for (int x=0; x<chrs[i].fx; x++)
		  putpixel(chrs[i].frame,x,y, data[y*chrs[i].fx + x]);
  delete[] data;

  fclose(f);
}

void DoCHRdealy()
{
  int i;

  for (i=0; i<100; i++)
    DoCHR(i);
}

//
// This does NOT take parallaxing into account, and is intended
// to be multi-purpose. All params are assumed to be valid.
//
// The upper left tile location will be adjusted as needed to
// ensure that rendering begins at the upper left target pixel
// location and ends at the lower right pixel location. If there
// are not enough tiles to fill the entire target pixel region,
// what tiles are available will be rendered at the upper left
// target pixel location.
//
void render_layer_with_callback(int layer_index,
								int& pixel_x, int& pixel_y,
								int tile_ulx, int tile_uly,
								int pixel_end_x, int pixel_end_y,
								void (*callback)(int pixel_x, int pixel_y, int tile_x, int tile_y, int tile_index)) {

	unsigned short* layer_data=layers[layer_index];
	int left=pixel_x;
	int tile_left=tile_ulx;
	for (; pixel_y<pixel_end_y; pixel_y+=16,tile_uly++) {
		for (; pixel_x<pixel_end_x; pixel_x+=16,tile_ulx++) {
			int tile_index=layer_data[tile_uly*layer->sizex+tile_ulx];
			callback(pixel_x,pixel_y,tile_ulx,tile_uly,tile_index);
		}
		if (pixel_y+16<pixel_end_y) {
			pixel_x=left;
			tile_ulx=tile_left;
		}
	}
}

void render_layer_masked_with_callback(int layer_index,
								int pixel_x, int pixel_y,
								int tile_ulx, int tile_uly,
								int pixel_end_x, int pixel_end_y,
								void (*callback)(int pixel_x, int pixel_y, int tile_x, int tile_y, int tile_index)) {

	unsigned short* layer_data=layers[layer_index];
	int left=pixel_x;
	int tile_left=tile_ulx;
	for (; pixel_y<pixel_end_y; pixel_y+=16,tile_uly++) {
		for (; pixel_x<pixel_end_x; pixel_x+=16,tile_ulx++) {
			int tile_index=layer_data[tile_uly*layer->sizex+tile_ulx];
			if (tile_index>0 && tile_index<numtiles) {
				callback(pixel_x,pixel_y,tile_ulx,tile_uly,tile_index);
			}
		}
		pixel_x=left;
		tile_ulx=tile_left;
	}
}

void render_data_with_callback(unsigned char* data, int data_width,
  							   int pixel_x, int pixel_y,
							   int tile_ulx, int tile_uly,
							   int pixel_end_x, int pixel_end_y,
							   void (*callback)(int pixel_x, int pixel_y, int tile_x, int tile_y, int value)) {

	int left=pixel_x;
	int tile_left=tile_ulx;
	for (; pixel_y<pixel_end_y; pixel_y+=16,tile_uly++) {
		for (; pixel_x<pixel_end_x; pixel_x+=16,tile_ulx++) {
			int tile_index=data[tile_uly*data_width+tile_ulx];
			callback(pixel_x,pixel_y,tile_ulx,tile_uly,tile_index);
		}
		pixel_x=left;
		tile_ulx=tile_left;
	}
}

static void clip_render_params(int layer_index,
							   int pixel_x, int pixel_y,
							   int& tile_ulx, int& tile_uly,
							   int& pixel_end_x, int& pixel_end_y) {

	if (layer_index < 0 || layer_index > 5) return;

	layer_r* layer=&::layer[layer_index];
	// "clip"
	int tile_count_x=(pixel_end_x-pixel_x+15)/16;
	int tile_count_y=(pixel_end_y-pixel_y+15)/16;
	
	if (tile_ulx<0) {
		tile_count_x+=tile_ulx;
		pixel_end_x+=tile_ulx*16;
		tile_ulx=-tile_ulx;
	}
	if (tile_ulx >= layer->sizex) {
		tile_count_x=0;
		pixel_x=pixel_end_x=0;
		tile_count_y=0;
		pixel_y=pixel_end_y=0;
		return;
	} else if (tile_ulx+tile_count_x > layer->sizex) {
		tile_count_x=layer->sizex-tile_ulx;
		pixel_end_x=pixel_x+(tile_count_x*16);
	}

	if (tile_uly<0) {
		tile_count_y+=tile_uly;
		pixel_end_y+=tile_uly*16;
		tile_uly=-tile_uly;
	}
	if (tile_uly >= layer->sizey) {
		tile_count_y=0;
		pixel_y=pixel_end_y=0;
		tile_count_x=0;
		pixel_x=pixel_end_x=0;
		return;
	} else if (tile_uly+tile_count_y > layer->sizey) {
		tile_count_y=layer->sizey-tile_uly;
		pixel_end_y=pixel_y+(tile_count_y*16);
	}

}

static void BlitBackLayer_callback(int pixel_x, int pixel_y, int tile_x, int tile_y, int tile_index) {
	CopySprite(pixel_x, pixel_y, 16, 16, vsp[tileidx[tile_index]]);
}

void BlitBackLayer(char layer_index) {
	if (!layertoggle[layer_index]) return;

	int parallax_x=xwin*layer[layer_index].pmultx/layer[layer_index].pdivx;
	int parallax_y=ywin*layer[layer_index].pmulty/layer[layer_index].pdivy;
	int pixel_x=-(parallax_x&15);
	int pixel_y=-(parallax_y&15);
	int tile_ulx=parallax_x/16;
	int tile_uly=parallax_y/16;
	int pixel_end_x=sx;
	int pixel_end_y=sy;

	clip_render_params(layer_index,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y);
	render_layer_with_callback(layer_index,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y,BlitBackLayer_callback);

	if (pixel_x<sx) {
		rectfill(get_offscreen(),pixel_x,0,sx,pixel_y,0);
	}
	if (pixel_y<sy) {
		rectfill(get_offscreen(),0,pixel_y,sx,sy,0);
	}

	curlayer++;
}

static void LucentBlitLayer_callback(int pixel_x, int pixel_y, int tile_x, int tile_y, int tile_index) {
	TCopySpriteLucent(pixel_x, pixel_y, 16, 16, vsp[tileidx[tile_index]]);
}

void LucentBlitLayer(char layer_index) {
	if (!layertoggle[layer_index]) return;

	int parallax_x=xwin*layer[layer_index].pmultx/layer[layer_index].pdivx;
	int parallax_y=ywin*layer[layer_index].pmulty/layer[layer_index].pdivy;
	int pixel_x=-(parallax_x&15);
	int pixel_y=-(parallax_y&15);
	int tile_ulx=parallax_x/16;
	int tile_uly=parallax_y/16;
	int pixel_end_x=sx;
	int pixel_end_y=sy;

	clip_render_params(layer_index,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y);
	if (curlayer==0) {
		rectfill(get_offscreen(),0,0,SCREEN_W,SCREEN_H,0);
		render_layer_with_callback(layer_index,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y,LucentBlitLayer_callback);
	} else {
		render_layer_masked_with_callback(layer_index,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y,LucentBlitLayer_callback);
	}
	
	curlayer++;
}

static void TransBlitLayer_callback(int pixel_x, int pixel_y, int tile_x, int tile_y, int tile_index) {
	if (tile_index) {
		TCopySprite(pixel_x, pixel_y, 16, 16, vsp[tileidx[tile_index]]);
	}
}

void TransBlitLayer(char layer_index) {
	if (!layertoggle[layer_index]) return;

	int parallax_x=xwin*layer[layer_index].pmultx/layer[layer_index].pdivx;
	int parallax_y=ywin*layer[layer_index].pmulty/layer[layer_index].pdivy;
	int pixel_x=-(parallax_x&15);
	int pixel_y=-(parallax_y&15);
	int tile_ulx=parallax_x/16;
	int tile_uly=parallax_y/16;
	int pixel_end_x=sx;
	int pixel_end_y=sy;

	clip_render_params(layer_index,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y);
	if (curlayer==0) {
		render_layer_with_callback(layer_index,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y,TransBlitLayer_callback);
	} else {
		render_layer_masked_with_callback(layer_index,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y,TransBlitLayer_callback);
	}
	
	curlayer++;
}

void BlitLayer(char c) {
	if(layer[c].trans) { LucentBlitLayer(c); return; }
	if (curlayer) TransBlitLayer(c);
	else BlitBackLayer(c);
}

static void DrawObsZone_callback(int pixel_x, int pixel_y, int tile_x, int tile_y, int tile_index) {
	if (tile_index) {
		TCopySprite(pixel_x, pixel_y, 16, 16, get_stipple());
	}
}

void DrawObstructions() {
	int parallax_x=xwin*layer[0].pmultx/layer[0].pdivx;
	int parallax_y=ywin*layer[0].pmulty/layer[0].pdivy;
	int pixel_x=-(parallax_x&15);
	int pixel_y=-(parallax_y&15);
	int tile_ulx=parallax_x/16;
	int tile_uly=parallax_y/16;
	int pixel_end_x=sx;
	int pixel_end_y=sy;

	clip_render_params(0,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y);
	render_data_with_callback((unsigned char*)obstruct,layer[0].sizex,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y,DrawObsZone_callback);
	
	curlayer++;
}

void DrawZones() {
	int parallax_x=xwin*layer[0].pmultx/layer[0].pdivx;
	int parallax_y=ywin*layer[0].pmulty/layer[0].pdivy;
	int pixel_x=-(parallax_x&15);
	int pixel_y=-(parallax_y&15);
	int tile_ulx=parallax_x/16;
	int tile_uly=parallax_y/16;
	int pixel_end_x=sx;
	int pixel_end_y=sy;

	clip_render_params(0,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y);
	render_data_with_callback((unsigned char*)zone,layer[0].sizex,pixel_x,pixel_y,tile_ulx,tile_uly,pixel_end_x,pixel_end_y,DrawObsZone_callback);
	
	curlayer++;
}

void DrawEntities()
{ int m;

  for (m=0; m<entities; m++)
    if ((entity[m].x>=(xwin/16)) && (entity[m].x<(xwin/16)+22) &&
        (entity[m].y>=(ywin/16)) && (entity[m].y<(ywin/16)+ty+2))
    {
      if (chrs[entity[m].chrindex].frame)
         TCopySprite((entity[m].x*16)-xwin-chrs[entity[m].chrindex].hx,
                      (entity[m].y*16)-ywin-chrs[entity[m].chrindex].hy,
                       chrs[entity[m].chrindex].fx,
                       chrs[entity[m].chrindex].fy,
                       chrs[entity[m].chrindex].frame);
      else
      {
         TCopySprite(
           (entity[m].x*16)-xwin,(entity[m].y*16)-ywin,16,16,get_stipple());
        //ColorGrid((entity[m].x*16)-xwin+16,
        //  (entity[m].y*16)-ywin+16, 0);
      }
    }
}

// aen
void DrawSelection()
{ int i,j;
  int x1,y1,x2,y2;

  oxw=xwin*layer[el].pmultx/layer[el].pdivx;
  oyw=ywin*layer[el].pmulty/layer[el].pdivy;
  xofs=-(oxw&15);
  yofs=-(oyw&15);
  xtc=oxw>>4;
  ytc=oyw>>4;

  x1=selx1;
  y1=sely1;
  x2=selx2;
  y2=sely2;

  if (x2<x1) x2^=x1,x1^=x2,x2^=x1;
  if (y2<y1) y2^=y1,y1^=y2,y2^=y1;

  for (i=0; i<ty+1; i++)
    for (j=0; j<tx+1; j++)
    {
      if (xtc+j >= x1 && xtc+j <= x2
      && ytc+i >= y1 && ytc+i <= y2)
      {
        TCopySprite((j*16)+xofs, (i*16)+yofs, 16,16, get_stipple());
        //ColorGrid((j*16)+xofs,(i*16)+yofs,7);
      }
    }
}

void RenderMap() {
	curlayer=0;
	for (char* src=rstring; *src; ++src) {
		switch (*src) {
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6': BlitLayer((int)*src-(int)'1'); break;
		}
	}
	if (!curlayer) ClearScreen();
	if (layertoggle[6]) DrawObstructions();
	if (layertoggle[7]) DrawZones();
	if (layertoggle[8]) DrawEntities();
	if (shifted) DrawSelection();
}

void AnimateTile(char i, int l)
{
  switch (vspanim[i].mode)
  {
    case 0: if (tileidx[l]<vspanim[i].finish) tileidx[l]++;
            else tileidx[l]=vspanim[i].start;
            break;
    case 1: if (tileidx[l]>vspanim[i].start) tileidx[l]--;
            else tileidx[l]=vspanim[i].finish;
            break;
    case 2: tileidx[l]=(short)rnd(vspanim[i].start,vspanim[i].finish);
            break;
    case 3: if (flipped[l])
            {
              if (tileidx[l]!=vspanim[i].start) tileidx[l]--;
              else { tileidx[l]++; flipped[l]=0; }
            }
            else
            {
              if (tileidx[l]!=vspanim[i].finish) tileidx[l]++;
              else { tileidx[l]--; flipped[l]=1; }
            }
  }
}

void Animate(char i)
{ static int l;

  vadelay[i]=0;
  for (l=vspanim[i].start; l<=vspanim[i].finish; l++)
    AnimateTile(i,l);
}

void CheckTileAnimation()
{ static char i;

  for (i=0; i<100; i++)
      { if ((vspanim[i].delay) && (vspanim[i].delay<vadelay[i]))
           Animate(i);
        vadelay[i]++; }
}

