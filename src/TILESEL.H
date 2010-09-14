
#ifndef TILESEL_INC
#define TILESEL_INC

#include "allegro.h"
#include "pcx.h"

extern int selofs;
extern BITMAP* ttile;//[256];
extern BITMAP* ttile2;//[256];
extern int curastd;
extern int curmode;
extern BITMAP** ovsp;

extern int curtileptr(int row);
extern void NormalButton(int x1, int y1, int x2, int y2, char *str);
extern void PressdButton(int x1, int y1, int x2, int y2, char *str);
extern void RenderSelector();
extern void FlipX(unsigned short edtile);
extern void FlipY(unsigned short edtile);
extern void FlipZ(unsigned short edtile);
extern void DrawHighlight(int x, int y);
extern void DrawBigHighlight(int x, int y);
extern void TileSelector();
extern void VSPAnimDialog();
extern void AddTilesDialog();
extern void RenderImport();
extern void ImportTiles();
extern void ImportVSP();
extern void OpenPCXas();

extern void ReadPCXLine(unsigned char *dest);
extern void LoadPCXHeader(char *fname);
extern void LoadPCXHeaderNP(char *fname);
extern int  GrabTileAt(PCX* pcx, int x, int y, int z, int check_dup);
extern void ImportPCX();

#endif // TILESEL_INC
