
#if !defined(INTERINC)
#define INTERINC

#ifdef __WATCOMC__
  #include <i86.h>
  #define REGISTERS union REGS
  #define SET_AX(r,v) (r).w.ax=(short)(v)
  #define SET_BX(r,v) (r).w.bx=(short)(v)
  #define SET_CX(r,v) (r).w.cx=(short)(v)
  #define SET_DX(r,v) (r).w.dx=(short)(v)
  #define INTERRUPT(i,r) int386((i),&(r),&(r))
#elif defined(__DJGPP__)
  #include <dpmi.h>
  #define REGISTERS __dpmi_regs
  #define SET_AX(r,v) (r).x.ax=(short)(v)
  #define SET_BX(r,v) (r).x.bx=(short)(v)
  #define SET_CX(r,v) (r).x.cx=(short)(v)
  #define SET_DX(r,v) (r).x.dx=(short)(v)
  #define INTERRUPT(i,r) __dpmi_int((i),&(r))
#endif // watcom | djgpp

#endif // INTERINC
