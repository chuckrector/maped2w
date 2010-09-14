
#ifndef SMALFONT_INC
#define SMALFONT_INC

#include "allegro.h"

extern int fontx;
extern int fonty;

extern void GotoXY(int x1, int y1);
extern void print_char(char c);
extern int print_char_score(char c);
extern void printstring(char *str);
extern void printstring(int x, int y, const char* text, ...);
extern void printstring_score(char* str, int cursor);
extern int  pixels(char *str);
extern void dec_to_asciiz(int num, char *buf);
extern void TextColor(unsigned char newc);

void goXY(int x, int y);
void put(char* text);
void putXY(int x, int y, char* text);
void putLine(char* text);
void putLineXY(int x, int y, char* text);
extern void RegenSmallFont();

extern BITMAP **smal_tbl;

#endif // SMALFONT_INC
