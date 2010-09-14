
#ifndef RENDER_INC
#define RENDER_INC

#include "allegro.h"

#define MAXTILES 10240

extern int oxw, oyw;
extern int xofs, yofs;
extern int xtc, ytc;

extern char curlayer;
extern short tileidx[MAXTILES];
extern char flipped[MAXTILES];

extern short tileidx[MAXTILES];

typedef struct
{
  //char *frame;
  BITMAP* frame;
  int fx, fy;
  int hx, hy;
} chri;

extern chri chrs[100];

extern void DoCHR(int i);
extern void DoCHRdealy();
extern void BlitBackLayer(char l);
extern void LucentBlitLayer(char l);
extern void TransBlitLayer(char l);
extern void BlitLayer(char c);
extern void DrawObstructions();
extern void DrawZones();
extern void DrawEntities();
extern void DrawSelection();
extern void RenderMap();
extern void AnimateTile(char i, int l);
extern void Animate(char i);
extern void CheckTileAnimation();

extern void render_layer_with_callback(int layer_index,
								int& pixel_x, int& pixel_y,
								int tile_ulx, int tile_uly,
								int pixel_end_x, int pixel_end_y,
								void (*callback)(int pixel_x, int pixel_y, int tile_x, int tile_y, int tile_index));

#endif // RENDER_INC
