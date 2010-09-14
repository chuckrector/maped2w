// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                   The VERGE Engine version 2.01                     ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                    Timer / PIC contoller module                     ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CHANGELOG:
// <aen, apr 21>
// + tidied up a bit
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <dos.h>
#include <stdio.h>
#include <conio.h>

#include "render.h"

#include "ssaver.h"
#include "vdriver.h"
#include "log.h"

volatile unsigned int systemtime=0;
volatile unsigned int timer_count=0;
volatile unsigned int tick=0;
volatile int pixel_scroll_timer=0;
volatile int tile_scroll_timer=0;
volatile int backupct=0;
volatile int idlect=0;
volatile int xz=0;
volatile int yz=0;
volatile int xzd=2;
volatile int yzd=1;
volatile int ssaver_enabled=0;

void _maped_timer_handler() {
	systemtime++;
	timer_count++;
	pixel_scroll_timer+=2;
	tile_scroll_timer+=2;
	tick++;
	backupct++;
	idlect++;

	if (ssaver_enabled) {
		if (systemtime&1) {
			yz+=yzd;
			while (yz>=tsy)
				yz-=tsy;
			xz+=xzd;
			while (xz>=tsy)
				xz-=tsy;
		}
	}
}
END_OF_FUNCTION(_maped_timer_handler)


static bool installed=false;

void InitTimer() {
	if (installed) return;

	Log("Initialize timer");

	install_timer();
	LOCK_VARIABLE(systemtime);
	LOCK_VARIABLE(timer_count);
	LOCK_VARIABLE(tick);
	LOCK_VARIABLE(pixel_scroll_timer);
	LOCK_VARIABLE(tile_scroll_timer);
	LOCK_VARIABLE(backupct);
	LOCK_VARIABLE(idlect);
	LOCK_VARIABLE(xz);
	LOCK_VARIABLE(yz);
	LOCK_VARIABLE(xzd);
	LOCK_VARIABLE(yzd);
	LOCK_VARIABLE(ssaver_enabled);
	LOCK_FUNCTION(_maped_timer_handler);
	install_int_ex(_maped_timer_handler, BPS_TO_TIMER(100));
	installed=true;
}

void ShutdownTimer() {
	if (!installed) return;

	remove_int(_maped_timer_handler);
	remove_timer();
	installed=false;
}