// maped2w, version 0.1
// Original code: Copyright (C)1998 BJ Eirich (aka vecna)
// Revamped code: Charles Rector (aka aen)

#include "allegro.h"
#include "timer.h"

volatile int last_pressed = 0;

int _maped_keyboard_callback(int key) {
	idlect=0; // so playing columns won't eventually trigger screensaver
	return last_pressed=key;
}
END_OF_FUNCTION(_maped_keyboard_callback)

void InitKeyboard() {
	static bool installed=false;
	if (installed) {
		return;
	}
	install_keyboard();
	LOCK_VARIABLE(last_pressed);
	LOCK_FUNCTION(_maped_keyboard_callback);
	keyboard_callback=_maped_keyboard_callback;
	installed=true;
}

void ShutdownKeyboard() {
	remove_keyboard();
}