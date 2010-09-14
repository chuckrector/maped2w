#include "compress.h"

char* cb;
char* csrc;                      // compression workspace buffer
int bufsize;                         // how many bytes need to be written

void compress_EmitC (char c)
{
  *csrc=c;
  csrc++;
  bufsize++;
}

void compress_EmitW (short int w)
{
  char *ptr;

  ptr=(char *)&w;
  *csrc=*ptr;
  csrc++;
  ptr++;
  *csrc=*ptr;
  csrc++;
  bufsize+=2;
}

void WriteCompressedLayer1(unsigned char *p, int len)
{ int i;
  unsigned char byt,samect;
  unsigned char repcode;

  csrc=cb;
  i=0; bufsize=0;
  do
  {   byt=p[i++];
      samect=1;
      while (samect<254 && i<len && byt==p[i])
      {
         samect++;
         i++;
      }
      if (samect==2 && byt != 0xFF) { compress_EmitC(byt); }
      if (samect==3 && byt != 0xFF) { compress_EmitC(byt); compress_EmitC(byt); }
      if (samect>3 || byt == 0xFF)
      {
         repcode=0xFF;
         compress_EmitC(repcode);
         repcode=(char) samect;
         compress_EmitC(repcode);
      }
      compress_EmitC(byt);
  } while (i<len);
}

void ReadCompressedLayer1(unsigned char *dest, int len, FILE *f)
{ int j,n;
  unsigned char run;
  unsigned char w;

  n=0;
  do
  {
     fread(&w, 1, 1, f);
     if (w==0xFF)
     {
         fread(&run, 1, 1, f);
         fread(&w, 1, 1, f);
         for (j=0; j<run; j++)
             dest[n+j]=w;
         n+=run;
     }
     else
      {
         dest[n]=w;
         n++;
      }
  } while (n<len);
}

void WriteCompressedLayer2(unsigned short *p,int len)
{ int i;
  unsigned short byt,samect;
  unsigned char repcode;

  csrc=cb;
  i=0; bufsize=0;
  do
  {   byt=p[i++];
      samect=1;
      while (samect<255 && i<len && byt==p[i])
      {
         samect++;
         i++;
      }
      if (samect>1)
      {
         repcode=(char) samect;
         compress_EmitC(repcode);
         repcode=0xFF;
         compress_EmitC(repcode);
      }
      compress_EmitW(byt);
  } while (i<len);
}

void ReadCompressedLayer2(unsigned short *dest, int len, FILE *f)
{ int j,n;
  unsigned char run;
  unsigned short w;

  n=0;
  do
  {
     fread(&w, 1, 2, f);
     if ((w & 0xFF00)==0xFF00)
     {
        run=(char) (w & 0x00FF);
        fread(&w, 1, 2, f);
        for (j=0; j<run; j++)
            dest[n+j]=w;
        n+=run;
     }
     else
     {
        dest[n]=w;
        n++;
     }
  } while (n<len);
}
