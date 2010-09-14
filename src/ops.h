#ifndef OPS_INC
#define OPS_INC

extern int InsertTiles(int here, int count);
extern int DeleteTiles(int here, int count);

extern void ScreenShot(char* fname=0);
extern void GetSequencedFilename(char* dest, char* extension);

#endif // OPS_INC