
#ifndef KEYBOARD_INC
#define KEYBOARD_INC

extern volatile int last_pressed;

extern void InitKeyboard();
extern void ShutdownKeyboard();

#endif // KEYBOARD_INC