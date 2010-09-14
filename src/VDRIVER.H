
#ifndef VDRIVER_INC
#define VDRIVER_INC

#include "inter.h"
#include "allegro.h"

extern int AllegroShutdown();
extern int AllegroShowPage();
extern int AllegroPutPixel(int x, int y, int color);
extern int AllegroGetPixel(int x, int y);
extern int AllegroCopySprite(int x, int y, int width, int height, BITMAP *src);
extern int AllegroTCopySprite(int x, int y, int width, int height, BITMAP *src);
extern int AllegroVLine(int x, int y, int length, int color);
extern int AllegroHLine(int x, int y, int width, int color);
extern int AllegroFilledBox(int x, int y, int width, int height, int color);
extern int AllegroColorGridSize(int x, int y, int w, int h, int color);
extern int AllegroColorGrid(int x, int y, int color);
extern int AllegroClearScreen();
extern int AllegroTCopySpriteLucent(int x, int y, int width, int height, BITMAP *src);

extern int (*ShutdownVideo)		();
extern int (*ShowPage)			();
extern int (*CopySprite)		(int x, int y, int width, int height, BITMAP *src);
extern int (*TCopySprite)		(int x, int y, int width, int height, BITMAP *src);
extern int (*TCopySpriteLucent)	(int x, int y, int width, int height, BITMAP *src);
extern int (*FilledBox)			(int x, int y, int width, int height, int color);
extern int (*VLine)				(int x, int y, int height, int color);
extern int (*HLine)				(int x, int y, int width, int color);
extern int (*ColorGrid)			(int x, int y, int color);
extern int (*ColorGridSize)		(int x, int y, int w, int h, int color);
extern int (*ClearScreen)		();

extern void SetRenderDest(BITMAP* dest);
extern void RestoreRenderDest();

extern PALETTE pal;
extern PALETTE pal2;
extern BITMAP* get_offscreen();

extern int sx;
extern int sy;
extern int tx;
extern int ty;
extern int tsx;
extern int tsy;
extern int map_scroll_x;
extern int map_scroll_y;

extern void InitVideo(int xres, int yres, int refresh_rate, bool windowed);
extern void SetPalette(PALETTE pal);
extern void GetPalette(void);
extern void set_intensity(unsigned int n);

extern int cx1, cy1, cx2, cy2;

extern BITMAP* get_stipple();

#endif // VDRIVER_INC
