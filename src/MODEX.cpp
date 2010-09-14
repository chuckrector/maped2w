// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                   The VERGE Engine version 2.01                     ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                        ModeX 320x240 module                         ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#include <conio.h>
#include <string.h>
#include <malloc.h>

#include "mode13h.h" // SetMode()
#include "modex.h"
#include "vdriver.h"

extern void CheckTimerStuff();

int ModeXShowPage() {
	CheckTimerStuff();

	acquire_bitmap(screen);
	stretch_blit(get_offscreen(), screen, 0, 0, tsx, tsy, 0, 0, SCREEN_W, SCREEN_H);
	release_bitmap(screen);

	return 0;
}

void InitModeX() {
	//...
}