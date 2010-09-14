
#ifndef MODE13H_INC
#define MODE13H_INC

#include "allegro.h"

extern void SetMode(int mode);
extern int  Mode13hShutdown();
extern int  Mode13hShowPage();
extern int  Mode13hCopySprite(int x, int y, int width, int height, BITMAP *src);
extern int  Mode13hTCopySprite(int x, int y, int width, int height, BITMAP *src);
extern int  Mode13hVLine(int x, int y, int length, int color);
extern int  Mode13hHLine(int x, int y, int width, int color);
extern int  Mode13hFilledBox(int x, int y, int width, int height, int c);
extern int  Mode13hColorGrid(int x, int y, int c);
extern int  Mode13hColorGridSize(int x, int y, int w, int h, int c);
extern int  Mode13hClearScreen();
extern int  Mode13hTCopySpriteLucent(int x, int y, int width, int height, BITMAP *src);
extern void InitMode13h();

#endif // MODE13H_INC
