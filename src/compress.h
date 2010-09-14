#ifndef COMPRESS_INC
#define COMPRESS_INC

#include <cstdio>
#include "buffer.h"

// -- map compression

extern char* cb;
extern char* csrc;                      // compression workspace buffer
extern int bufsize;                         // how many bytes need to be written

extern void compress_EmitC(char c);
extern void compress_EmitW (short int w);

extern void WriteCompressedLayer1(unsigned char *p, int len);
extern void ReadCompressedLayer1(unsigned char *dest, int len, FILE *f);
extern void WriteCompressedLayer2(unsigned short *p,int len);
extern void ReadCompressedLayer2(unsigned short *dest, int len, FILE *f);

#endif // COMPRESS_INC