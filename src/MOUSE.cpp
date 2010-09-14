// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                   The VERGE Engine version 2.01                     ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                          Mouse module                               ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#include <stdio.h>
//#include <i86.h>
#include <string.h>

#include "timer.h"
#include "vdriver.h"
#include "log.h"

BITMAP* mousepic = 0;
static int mouse_hotspot_x=1;
static int mouse_hotspot_y=1;
char _mousepic[] = { 01,01,00,00,00,00,
	                 01,31,01,00,00,00,
                     01,31,31,01,00,00,
                     01,31,31,31,01,00,
                     01,31,31,31,31,01,
                     01,01,01,31,01,01,
                     00,00,01,31,01,00,
                     00,00,01,01,01,00};

int rb=0;
int wb=0;
int lmx=0;
int lmy=0;
int lmb=0;

// ================================= Code ====================================

void PutMouse(int x, int y) {
	position_mouse(x,y);
}

void InitMouse() {
	install_mouse();
	PutMouse(320/2, 240/2);
}

void ReadMouse() {
	//Log("ReadMouse()");
	poll_mouse();

	rb=mouse_b;

	if (rb && wb) {
		mouse_b=0;
		return;
	}
	if (wb && !rb) {
		wb=0;
	}
	mouse_b=rb;

	if ((lmx!=mouse_x) || (lmy!=mouse_y) || (lmb!=mouse_b)) {
		idlect=0;
	}

	lmx=mouse_x;
	lmy=mouse_y;
	lmb=mouse_b;
}

void RegenMouseCursor() {
	//Log("RegenMouseCursor()");
	if (mousepic) {
		destroy_bitmap(mousepic);
	}
	mousepic=create_bitmap(6,8);
	for (int y=0; y<8; y++) {
		for (int x=0; x<6; x++) {
			int value=_mousepic[y*6+x];
			if (value==1) {
				int col=makecol(0,0,0);
				if (col==0) col=1;
				putpixel(mousepic,x,y,col);
			} else if (value==31) {
				int col=makecol(255,255,255);
				if (col==0) col=2;
				putpixel(mousepic,x,y,col);
			} else {
				putpixel(mousepic,x,y,0);
			}
		}
	}
}

void DrawMouse()
{
	ReadMouse();
	
	if (!mousepic) {
		RegenMouseCursor();
	}

	//Log("DrawMouse::TCopySprite");
	TCopySprite(mouse_x-mouse_hotspot_x,mouse_y-mouse_hotspot_y,mousepic->w,mousepic->h,mousepic);
	//Log("DrawMouse::end");
}

void WaitRelease() {
	wb=1;
	mouse_b=0;
}