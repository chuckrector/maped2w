
#ifndef MOUSE_INC
#define MOUSE_INC

#include "allegro.h"

extern BITMAP* mousepic; // blah
extern char _mousepic[];

//extern int mx,my,mb;
extern int rb,wb;
extern int lmx,lmy,lmb;

extern void PutMouse(int x, int y);
extern void InitMouse();
extern void ReadMouse();
extern void DrawMouse();
extern void WaitRelease();
extern void RegenMouseCursor();

#endif // MOUSE_INC
