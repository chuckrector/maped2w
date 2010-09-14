
#ifndef GUICOMP_INC
#define GUICOMP_INC

/* Graphical User Interface Component Header
 * (C)1998 Richard Lau
 */

extern void stdwindow(int x1, int y1, int x2, int y2);
extern void Window(int x1, int y1, int x2, int y2, char *title);
extern void CheckBox(int x, int y, int checked);
extern void Button(int x, int y, char *str);
extern void ButtonSize(int x, int y, int w, int h, char* str);
extern void BigButton(int x, int y, char *str);
extern void ButtonPressed(int x, int y, char *str);
extern void ButtonPressedSize(int x, int y, int w, int h, char* str);
extern void BigButtonPressed(int x, int y, char *str);
extern void TextField(int x, int y, int width, char *str, int blink);
extern void TextFieldCursor(int max_pixels, int x, int y, int width, char* str, int score, int blink);
extern void TextFieldCursorSel(int max_pixels, int x, int y, int width, char *str, int cursor, int blink,
	int select_start, int select_end);

#endif // GUICOMP_INC
